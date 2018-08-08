/**********************************************************************
* Copyright(c) 2016 by Hanwha Techwin, Inc.
*
* This software is copyrighted by, and is the sole property
* of Hanwha Techwin.
*
* Hanwha Techwin, Co., Ltd.
* http://www.hanwhatechwin.co.kr
*********************************************************************/
/**
@file video_stream_ipc_executor.cpp
@brief VideoStreamIpcExecutor class implementation
*/
#include "video_stream_ipc_executor.h"
#include "setting/setting_common/setting_changer.h"
#include "utility/utility_common/utility_wrapper.h"
#include "audio_video/audio_video_common/profile_handler.h"
#include "connection_manager.h"
#include "communication/network/network_interface/network_interface.h"
#include "framework/scheduler/component_scheduler.h"
#include "network_server/rtp/rtp_streamer.h"
#include "record/onvif_record_setting_manager.h"

#include "socket/bsdsock.h"
#include "framework/frame/frame.h"

// JOB_ID_5 is reserved for IPC_EXEC_TID_INIT_RUN
#define VIDEO_STREAM_IPC_TID_FINISH     EventSource::JOB_ID_0
#define VIDEO_STREAM_IPC_TID_TIMEOUT    EventSource::JOB_ID_1
#define VIDEO_STREAM_IPC_TID_SEND_IMAGE EventSource::JOB_ID_2	// because this class sends only 1 stream, postpone event is not required
#define VIDEO_STREAM_IPC_MULTICAST_RETRY_REQUEST EventSource::JOB_ID_3
/**
@brief constructor
@param request - IPC request message to handle
*/
VideoStreamIpcExecutor::VideoStreamIpcExecutor(const IpcMessage* request)
    : IpcExecutor(request, "VideoStreamIpcExec", TT_LIVE),
      profileHandler(NULL), connectionManager(NULL), networkInterface(NULL),
      audio(NULL), scheduler(NULL), frontChannel(NULL), onvif_record_setting(0),
      simo_ch(-1), simo_pout(-1), frame_mode(1), last_audio_frame_num(0), last_video_frame_num(0), profileID(0), requested_profile_num(-1)
{
}

/**
@brief destructor
*/
VideoStreamIpcExecutor::~VideoStreamIpcExecutor(void)
{
//	if (simo_ch >= 0)
//	{
//		throw_taskevent(EV_DELETE_PROFILE, TT_PROFILE_HANDLER, simo_ch);
//	}
}

/**
@brief initialize
@param sock - BSDSOCK
@param profileHandler - ProfileHandler
*/
void VideoStreamIpcExecutor::initialize(BSDSOCK* sock,
                                        ProfileHandler* profileHandler, ConnectionManager* connectionManager,
                                        NetworkInterface* networkInterface, Audio* audio,
                                        ComponentScheduler* scheduler, FrontChannel* frontChannel, OnvifRecordSettingManager* onvif_record_setting)
{
    this->profileHandler = profileHandler;
    this->connectionManager = connectionManager;
    this->networkInterface = networkInterface;
    this->audio = audio;
    this->scheduler = scheduler;
    this->frontChannel = frontChannel;
    this->onvif_record_setting = onvif_record_setting;
    IpcExecutor::initialize(sock);

    set_eventmask(EV_IMAGE_ENQUEUED);
    set_eventmask(EV_PROFILE_ADDED);
}

/**
@brief del_profile
@param profileNum
*/
void VideoStreamIpcExecutor::del_profile(int profileNum)
{
    if (profileNum >= 1 && simo_ch >= 0)
    {
        long profile_simo = ((profileNum - 1) << 16) | (simo_ch & 0xFFFF);
        DP_LOG("Delete Profile (profile_num: %d, simo_ch: %d)", profileNum - 1, simo_ch);
        throw_taskevent(EV_DELETE_PROFILE, TT_PROFILE_HANDLER, profile_simo);
        requested_profile_num = -1;
        simo_ch = -1;
        register_delayedjob(300, VIDEO_STREAM_IPC_TID_FINISH);
    }
    else
    {
        DP_ERR("Invalid variables (profileNum: %d, simo_ch: %d)", profileNum, simo_ch);
    }
}

/**
@brief process task event
@param event - TaskEvent
@return true
*/
bool VideoStreamIpcExecutor::process_event(TaskEvent* event)
{
    if (IpcExecutor::process_event(event) == true)
    {
        return true;
    }

    switch (event->get_eventtype())
    {
        case EV_PROFILE_ADDED:
            simo_ch = event->get_data();
            break;
        case EV_IMAGE_ENQUEUED:
            if (((event->get_data() >> 16) & 0xff) == simo_ch)
            {
                send_image();
            }
            break;
        case DELAYEDJOB_EVENT(VIDEO_STREAM_IPC_TID_SEND_IMAGE):
            send_image();
            break;
        case DELAYEDJOB_EVENT(VIDEO_STREAM_IPC_TID_TIMEOUT):
            DP_ERR("snapshot timeout");
            del_profile(requested_profile_num);
            // fall through
            break;
        case DELAYEDJOB_EVENT(VIDEO_STREAM_IPC_TID_FINISH):
            suicide();
            suspend();
            break;
        case DELAYEDJOB_EVENT(VIDEO_STREAM_IPC_MULTICAST_RETRY_REQUEST):
            handle_request();
            break;
        default:
            break;
    }
    return true;
}

/**
@brief handle IPC request
*/
void VideoStreamIpcExecutor::handle_request(void)
{
    switch (request->header.command)
    {
        case CGI_CMD_VIDEO_SNAPSHOT:
            response.header.errorCode = process_videoSnapshot();
            break;
        case CGI_CMD_VIDEO_STREAM:
            response.header.errorCode = process_videoStreaming();
            break;
        case CGI_CMD_MULTICAST_STREAMING:
            response.header.errorCode = process_multicastStreaming();
            break;
        default:
            response.header.errorCode = APP_ERR_SERVICE_NOT_SUPPORTED;
            break;
    }

    if (response.header.errorCode == APP_NONE)
    {
        response.header.errorCode = APP_OK;
    }
    else
    {
        response.send_to_bsdsock(sock);
        register_delayedjob(10, VIDEO_STREAM_IPC_TID_FINISH);
    }
}

/**
@brief process CGI_CMD_VIDEO_SNAPSHOT request. START action is available
@return IPC_ERROR_CODE (return APP_NONE when the reponse should not be sent immediately)
*/
int VideoStreamIpcExecutor::process_videoSnapshot(void)
{
    if (request->header.action == IPC_ACTION_START)
    {
        CHECK_RX_BODY_SIZE(sizeof(VideoSnapshotStartRequest));
        VideoSnapshotStartRequest *pReq = (VideoSnapshotStartRequest *) request->body;

        if (prepare_socketbuf() == 0)
        {
            return APP_ERR_FAILED;
        }

        int ret = prepare_profile(pReq->profile - 1, -1, -1, -1, -1);
        if (ret != APP_OK)
        {
            return ret;
        }

        register_delayedjob(15000, VIDEO_STREAM_IPC_TID_TIMEOUT);
        DP_LOG("Add Profile (profile: %d)", pReq->profile - 1);
        throw_taskevent(EV_ADD_PROFILE, TT_PROFILE_HANDLER, pReq->profile - 1);
        requested_profile_num = pReq->profile;

        return APP_NONE;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process CGI_CMD_VIDEO_STREAM request. START action is available
@return IPC_ERROR_CODE (return APP_NONE when the reponse should not be sent immediately)
*/
int VideoStreamIpcExecutor::process_videoStreaming(void)
{
    if (request->header.action == IPC_ACTION_START)
    {
        CHECK_RX_BODY_SIZE(sizeof(VideoStreamStartRequest));
        VideoStreamStartRequest* pReq = (VideoStreamStartRequest*) request->body;

        if (prepare_socketbuf() == 0)
        {
            return APP_ERR_FAILED;
        }

        int ret = prepare_profile(pReq->profile - 1, pReq->width, pReq->height, pReq->framerate, pReq->compression);
        if (ret != APP_OK)
        {
            return ret;
        }

        // TODO: increase conmanager stream count
        DP_LOG("Add Profile (profile: %d)", pReq->profile - 1);
        throw_taskevent(EV_ADD_PROFILE, TT_PROFILE_HANDLER, pReq->profile - 1);
        requested_profile_num = pReq->profile;

        return APP_NONE;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process CGI_CMD_MULTICAST_STREAMING request. START/STOP actions are avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int VideoStreamIpcExecutor::process_multicastStreaming(void)
{
    static int retry_count = 0;
    if (request->header.action == IPC_ACTION_START)
    {
        CHECK_RX_BODY_SIZE(sizeof(MulticastStreamingStartRequest));
        MulticastStreamingStartRequest *pReq = (MulticastStreamingStartRequest *) request->body;
        if (findprofileIdbytoken(&profileID, pReq->profileToken) == 0)
        {
            return APP_ERR_PROFILE_NOT_EXIST;
        }
        int ret = RtpStreamer::start_multicast(profileID, connectionManager, profileHandler, audio, scheduler, networkInterface, frontChannel, onvif_record_setting);
        if (ret < 0)
        {
            retry_count = 0;
            DP_ERR("check multicast start status(profile:%d)", profileID);
            return APP_ERR_FAILED;
        }
        else if(ret == 0)
        {
            if(retry_count < VIDEO_STREAM_IPC_RTSP_RETRY_COUNT)
            {
                retry_count++;
                DP_LOG("multicast start retry[%d]!!", retry_count);
                register_delayedjob(VIDEO_STREAM_IPC_RTSP_PENDING_CHECK, VIDEO_STREAM_IPC_MULTICAST_RETRY_REQUEST);
                return APP_NONE;
            }
            else
            {
                DP_ERR("fail multicastStreaming profile :%d", profileID);
                return APP_ERR_FAILED;
            }
        }
        retry_count = 0;
        return APP_OK;
    }
    else if (request->header.action == IPC_ACTION_STOP)
    {
        CHECK_RX_BODY_SIZE(sizeof(MulticastStreamingStopRequest));
        MulticastStreamingStopRequest *pReq = (MulticastStreamingStopRequest *) request->body;
        if (findprofileIdbytoken(&profileID, pReq->profileToken) == 0)
        {
            return APP_ERR_PROFILE_NOT_EXIST;
        }
        if (RtpStreamer::stop_multicast(profileID, connectionManager) == 0)
        {
            DP_ERR("check multicast stop status(profile:%d)", profileID);
            return APP_ERR_FAILED;
        }
        return APP_OK;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief increase socket tx buf size
@return 1 on success, 0 on fail
*/
int VideoStreamIpcExecutor::prepare_socketbuf(void)
{
    if (sock->create_readyqueue(-1, UtilityWrapper::get_modelInfo(MODELINFO_MAX_VIDEO_PACKET_SIZE)) == 0)	// change only tx size
    {
        DP_ERR("CHANGE_SOCKBUFF_SIZE ERROR!!");
        sock->create_readyqueue(-1, 1024);	// no tx buf on failure.. create buf for error return
        return 0;
    }
    return 1;
}

/**
@brief prepare profile setting as requested.
       if \a width, \a height, \a framerate, \a compression is -1, then the field will not be changed
@param profile - profile index
@param width - width
@return IPC_ERROR_CODE
*/
int VideoStreamIpcExecutor::prepare_profile(int profile, int width, int height, int framerate, int compression)
{
    DP_LOG("PREPARE_PROFILE profile %d width %d height %d fps %d comp %d",
           profile, width, height, framerate, compression);
    if (profile < 0 || profile >= MAX_PROFILE_COUNT)
    {
        DP_ERR("invalid profile %d", profile);
        return APP_ERR_FAILED;
    }

    Utility::SmartPointerUtils::shared_ptr<VideoProfileSettingList> profileList = Utility::clone_sharedptr(profileHandler->get_profileList());

    ProfileSetting* targetProfile = &profileList->profile[profile];
    VideoEncoderSetting* targetEncoder = NULL;
    int need_set = 0;

    if (targetProfile->isEnable)
    {
        targetEncoder = &profileList->videoEncoder[(int)targetProfile->videoEncoderIndex];
    }
    else	// need to add a profile
    {
        need_set = 1;
        profileHandler->get_defaultProfileSetting(targetProfile, profile, NULL);
        for (int i = 0; i < MAX_PROFILE_COUNT; i++)
        {
            if (profileList->profile[i].hasVideoEncoder)
            {
                profileList->get_encoderOfProfile(i)->useCount++;
            }
        }
        for (int i = 0; i < MAX_PROFILE_COUNT; i++)
        {
            if (profileList->videoEncoder[i].useCount == 0)
            {
                targetProfile->videoEncoderIndex = i;
                targetEncoder = &profileList->videoEncoder[i];
                profileHandler->get_defaultVideoEncoderSetting(targetEncoder, i, VIDEO_CODEC_JPEG, false, NULL);
                break;
            }
        }
    }

    if (targetEncoder == NULL)
    {
        DP_ERR("crazy no empty encoder. this error MUST NOT happen");
        return APP_ERR_FAILED;
    }
    if (targetEncoder->codec != VIDEO_CODEC_JPEG)
    {
        DP_ERR("not a jpeg profile");
        return APP_ERR_FAILED;
    }

    if (width != -1 && (targetEncoder->width != width || targetEncoder->height != height))
    {
        need_set = 1;
        targetEncoder->width = width;
        targetEncoder->height = height;

        VideoSettingCapability cap;
        profileHandler->get_videoSettingCapability(&cap);
        int framerateLimit = 0;
        for (int i = 0; i < cap.resolution_count; i++)
        {
            if (cap.resolution[i].width == width && cap.resolution[i].height == height)
            {
                framerateLimit = cap.resolution[i].max_jpeg_fps;
                targetEncoder->rate.bitrateLimit = cap.resolution[i].default_jpeg_bps;
            }
        }
        if (framerateLimit == 0)
        {
            DP_ERR("invalid resolution %dx%d", width, height);
            return APP_ERR_FAILED;
        }
        if (targetEncoder->rate.framerateLimit > framerateLimit)
        {
            targetEncoder->rate.framerateLimit = framerateLimit;
        }
    }
    if (framerate != -1 && targetEncoder->rate.framerateLimit != framerate)
    {
        need_set = 1;
        targetEncoder->rate.framerateLimit = framerate;
    }
    if (compression != -1 && targetEncoder->compression != compression)
    {
        need_set = 1;
        targetEncoder->compression = compression;
    }

    if (need_set)
    {
        SettingChanger changer;
        changer.ADD_CHANGE(profileHandler, &ProfileHandler::set_profileList, profileList);
        throw_taskevent(changer.change_setting(get_classID(), get_objectID()));
        return changer.get_result();
    }
    return APP_OK;
}

/**
@brief handle an EV_IMAGE_ENQUEUED event
@param event - event
*/
void VideoStreamIpcExecutor::send_image(void)
{
    if (sock->is_rqreadyfordata(RINGQUEUE_TX) > 0)
    {
        register_delayedjob(10, VIDEO_STREAM_IPC_TID_SEND_IMAGE);
        return;
    }

    int oldpout;
    Frame* fpacket = get_videoFrame(&oldpout);
    if (fpacket == NULL)
    {
        return;
    }

    int image_sent = send_videoFrame(fpacket->get_buf(), fpacket->get_PESsize());

    int more_data = release_videoFrame(oldpout);

    if (request->header.command == CGI_CMD_VIDEO_SNAPSHOT && image_sent)
    {
        VideoSnapshotStartRequest *pReq = (VideoSnapshotStartRequest *) request->body;
        del_profile(pReq->profile);
    }
    else if (request->header.command == CGI_CMD_VIDEO_STREAM && more_data)
    {
        register_delayedjob(10, VIDEO_STREAM_IPC_TID_SEND_IMAGE);
    }
}

/**
@brief dequeue a video frame from SIMO
@param[out] oldpout - get pout value from simo (used by release)
@return Frame (must be released later)
*/
Frame* VideoStreamIpcExecutor::get_videoFrame(int* oldpout)
{
    Frame* fpacket;
    time_t biastime = UtilityWrapper::get_milisecond();
    int retsize = 0;
    unsigned char data_type = 0;

    while (1)
    {
        *oldpout = 0;
        fpacket = (Frame*) profileHandler->dequeue_for_cms(&simo_pout, oldpout, &retsize, &frame_mode, biastime,
                  &last_audio_frame_num, &last_video_frame_num, &data_type, 0, simo_ch);

        if (retsize <= 0 || fpacket == NULL || data_type > RB_FRAME_OTHER || simo_pout == -1)
        {
            if (frame_mode == 0x2071)
            {
                frame_mode = 1;
            }
            return NULL;
        }
        if (fpacket->get_codecformat() == DT_VIDEO)
        {
            return fpacket;
        }
        profileHandler->release_usecnt(*oldpout, simo_ch);	// TODO: channel id?
    }
}

/**
@brief send video frame
@param data - jpeg data
@param size - size of \a data
@return 1 on success, 0 on fail
*/
int VideoStreamIpcExecutor::send_videoFrame(unsigned char* data, int size)
{
    response.header.payloadLength = size;
    response.body = data;

    int ret = response.send_to_bsdsock(sock, 0);

    response.header.payloadLength = 0;
    response.body = NULL;

    return ret;
}

/**
@brief release video frame
@param oldpout - pout value from get liveframe
@param fpacket - Frame
@return return 1 if more frames are remaining in simo, else return 0
*/
int VideoStreamIpcExecutor::release_videoFrame(int oldpout)
{
    int last_pout = 0;
    profileHandler->release_usecnt(oldpout, simo_ch);
    last_pout = profileHandler->get_initial_pout(simo_ch);
    return (last_pout != 0 && last_pout != oldpout);
}

/**
@brief find the profile index from profile token
@param[out] profileId - profile index
@param profileToken - profile token
@return if there is a profile with the token return 1, else return 0
*/
int VideoStreamIpcExecutor::findprofileIdbytoken(int* profileId, const char* profileToken)
{
    Utility::SmartPointerUtils::shared_ptr<const VideoProfileSettingList> profileList = profileHandler->get_profileList();
    for (int i = 0; i < MAX_PROFILE_COUNT; i++)
    {
        if (strcmp(profileList->profile[i].token, profileToken) == 0)
        {
            *profileId = i;
            return 1;
        }
    }
    return 0;
}
