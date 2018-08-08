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
@file video_stream_ipc_executor.h
@brief VideoStreamIpcExecutor class declaration
*/
#ifndef _VIDEO_STREAM_IPC_HANDLER_H_
#define _VIDEO_STREAM_IPC_HANDLER_H_
#include "communication/datalink/ipc/ipc_executor.h"

class ProfileHandler;
class ConnectionManager;
class NetworkInterface;
class Audio;
class ComponentScheduler;
class Frame;
class FrontChannel;
class OnvifRecordSettingManager;

#define VIDEO_STREAM_IPC_RTSP_PENDING_CHECK 10
#define VIDEO_STREAM_IPC_RTSP_RETRY_COUNT 50

/**
@class VideoStreamIpcExecutor
@brief perform an IPC request related to video data, and send result to the client
       each instance of this class handles only 1 IPC request.
*/
class VideoStreamIpcExecutor : public IpcExecutor
{
public:
    VideoStreamIpcExecutor(const IpcMessage* request);
    virtual ~VideoStreamIpcExecutor(void);

    void initialize(BSDSOCK* sock, ProfileHandler* profileHandler,
                    ConnectionManager* connectionManager,
                    NetworkInterface* networkInterface, Audio* audio,
                    ComponentScheduler* scheduler, FrontChannel* frontChannel, OnvifRecordSettingManager* onvif_record_setting);

protected:
    virtual bool process_event(TaskEvent* event);
    virtual void handle_request(void);

    int process_videoSnapshot(void);
    int process_videoStreaming(void);
    int process_multicastStreaming(void);

    int prepare_socketbuf(void);
    int prepare_profile(int profile, int width, int height, int framerate, int compression);
    void send_image(void);
    Frame* get_videoFrame(int* oldpout);
    int send_videoFrame(unsigned char* data, int size);
    int release_videoFrame(int oldpout);
    int findprofileIdbytoken(int* profileId, const char* profileToken);
    void del_profile(int profileNum);

private:
    ProfileHandler* profileHandler;
    ConnectionManager* connectionManager;
    NetworkInterface* networkInterface;
    Audio* audio;
    ComponentScheduler* scheduler;
    FrontChannel* frontChannel;
    OnvifRecordSettingManager* onvif_record_setting;

    int simo_ch;
    int simo_pout;
    int frame_mode;
    unsigned int last_audio_frame_num;
    unsigned int last_video_frame_num;
    int profileID;
    int requested_profile_num;
};

#endif


