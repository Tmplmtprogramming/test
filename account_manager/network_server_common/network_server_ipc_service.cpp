/*********************************************************************************
 * Copyright(c) 2015 by Samsung Techwin, Inc.
 *
 * This software is copyrighted by, and is the sole property of Samsung Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Samsung Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Samsung Techwin.
 *
 * Samsung Techwin reserves the right to modify this software without notice.
 *
 * Samsung Techwin, Inc.
 * KOREA
 * http://www.samsungtechwin.co.kr
 *********************************************************************************/
/**
@file network_server_ipc_service.cpp
@brief NetworkServerIpcService class implementation
*/
#include "network_server_ipc_service.h"

#include "utility/utility_common/utility_wrapper.h"
#include "ipc/ipc_message.h"
#include "listener.h"
#include "framework/component/task_event.h"
#include "setting/setting_common/setting_changer.h"
#include "connection_manager.h"
#include "audio_video/audio_video_common/profile_handler.h"
#include "communication/network/network_interface/network_interface.h"
#include "communication/application/network_service_manager/network_service_manager.h"
#include "communication/application/ntp_client/ntp_client.h"
#include "network_server/rtp/rtp_streamer.h"
#include "video_stream_ipc_executor.h"
#include "framework/scheduler/component_scheduler.h"
#include "communication/application/smtp_client/smtp_client.h"
#include "communication/application/ftp_client/ftp_client.h"
#include "communication/application/ddns_client/ddns_client.h"
#include "communication/application/upnp_client/quick_connect_manager.h"
#include "record/onvif_record_setting_manager.h"

/**
@brief create a new instance
@return new instance
*/
NetworkServerIpcService* NetworkServerIpcService::createnew(void)
{
    return new NetworkServerIpcService();
}

/**
@brief constructor
*/
NetworkServerIpcService::NetworkServerIpcService(void)
    : IpcService("ServerIpc", TT_NETWORK_SERVER_IPC),
      accountManager(NULL), profileHandler(NULL), connectionManager(NULL),
      networkInterface(NULL), networkService(NULL), audio(NULL),
      scheduler(NULL), listener(NULL), frontChannel(NULL), ntpClient(0), smtpClient(0),
      ftpClient(0), ddns_client(0), onvifRecordSettings(NULL), quickConnect(NULL)
{
    set_eventmask(EV_IPINSTALLER_NET_SERVER_SETTING_CHANGE_REQUEST);
}

/**
@brief destructor
*/
NetworkServerIpcService::~NetworkServerIpcService(void)
{
}

/**
@brief initialize association
@param accountManager - AccountManager
@param profileHandler - ProfileHandler
@param connectionManager - ConnectionManager
@param networkInterface - NetworkInterface
@param networkService - NetworkServiceManager
@param audio - Audio
@param scheduler - ComponentScheduler
@param listener - Listener
@param ntpClient - NtpClient
@param smtpClient - SmtpClient
@param ftpClient - FtpClient
@param ddns_client - DdnsClient
@param onvifRecordSettings - OnvifRecordSettingManager
@param quickConnect - QuickConnectManager
@return 1
*/
int NetworkServerIpcService::initialize(AccountManager* accountManager,
                                        ProfileHandler* profileHandler, ConnectionManager* connectionManager,
                                        NetworkInterface* networkInterface,
                                        NetworkServiceManager* networkService, Audio* audio,
                                        ComponentScheduler* scheduler, Listener* listener,FrontChannel *frontChannel, NtpClient* ntpClient,
                                        SmtpClient* smtpClient, FtpClient* ftpClient, DdnsClient* ddns_client,
                                        OnvifRecordSettingManager* onvifRecordSettings,
                                        QuickConnectManager* quickConnect)
{
    this->accountManager = accountManager;
    this->connectionManager = connectionManager;
    this->profileHandler = profileHandler;
    this->networkInterface = networkInterface;
    this->networkService = networkService;
    this->audio = audio;
    this->scheduler = scheduler;
    this->listener = listener;
    this->frontChannel = frontChannel;
    this->ntpClient = ntpClient;
    this->smtpClient = smtpClient;
    this->ftpClient = ftpClient;
    this->ddns_client = ddns_client;
    this->onvifRecordSettings = onvifRecordSettings;
    this->quickConnect = quickConnect;

    IpcService::initialize();
    return 1;
}

/**
@brief process TaskEvent.
       only EV_IPINSTALLER_NET_SERVER_SETTING_CHANGE_REQUEST is handled in this function. other events will be handled in IpcService::process_event()
@param event - TaskEvent
@return true
*/
bool NetworkServerIpcService::process_event(TaskEvent* event)
{
    switch(event->get_eventtype())
    {
        case EV_IPINSTALLER_NET_SERVER_SETTING_CHANGE_REQUEST:
        {
            NetworkServerSetting* received = event->steal_sharedData<NetworkServerSetting>();
            Utility::SmartPointerUtils::shared_ptr<NetworkServerSetting> setting(received);
            Utility::SmartPointerUtils::shared_ptr<const HttpServerSetting> webserver = networkService->get_setting<HttpServerSetting>();

            if(!(networkInterface->check_overlap_port(webserver->http.port, webserver->https.port, setting->rtsp.port, setting->vnp.port)))
            {
                DP_ERR("[%s] process_event is failed!",__FUNCTION__);
                return false;
            }

            SettingChanger changer;
            changer.ADD_CHANGE(listener, &Listener::set_serverSetting, setting);
            throw_taskevent(changer.change_setting(get_classID(), get_objectID()));
            break;
        }
        default:
            IpcService::process_event(event);
            break;
    }
    return true;
}


/**
@brief handle IPC request, and generate a new response message
@param request - request message
@return new reponse message (must be deleted later)
*/
IpcMessage* NetworkServerIpcService::handle_request(const IpcMessage* request)
{
    IpcMessage* response = new IpcMessage(request);

    switch (request->header.command)
    {
        case CGI_CMD_NETWORK_PROTOCOL:
            response->header.errorCode = process_protocol(response, request);
            break;
        case CGI_CMD_USER:
            response->header.errorCode = process_user(response, request);
            break;
        case CGI_CMD_STREAM_URI:
            response->header.errorCode = process_streamUri(response, request);
            break;
        case CGI_CMD_SNAPSHOT_URI:
            response->header.errorCode = process_snapshotUri(response, request);
            break;
        case CGI_CMD_STREAM_URI_V20:
            response->header.errorCode = process_streamUri20(response, request);
            break;
        case CGI_CMD_PROFILE_ACCESS_INFORMATION:
            response->header.errorCode = process_accessInfo(response, request);
            break;
        case CGI_CMD_NETWORK_CONFIGURATION:
            response->header.errorCode = process_networkConfig(response, request);
            break;
        case CGI_CMD_SYNC_POINT:
            response->header.errorCode = process_syncPoint(response, request);
            break;
        case CGI_CMD_SECURITY_CONFIGURATION:
            response->header.errorCode = process_securityConfig(response, request);
            break;
        case CGI_CMD_MULTICAST_STREAMING:
        case CGI_CMD_VIDEO_SNAPSHOT:
        case CGI_CMD_VIDEO_STREAM:
            DP_CGI("ask bsdsock for command %d", request->header.command);
            delete response;
            return NULL;
        case PXY_RP_GET_REPLAY_URI:
            response->header.errorCode = process_replayURI(response, request);
            break;
        case CGI_CMD_NETWORK_CONFIGURATION_EXTERNAL:
        default:
            response->header.errorCode = APP_ERR_SERVICE_NOT_SUPPORTED;
            break;
    }

    return response;
}

/**
@brief handle IPC request event, and send a response to the socket directly
@param request - request message
@param sock - connection socket
@caution \a request and \a sock must be deleted after sending the response
@see IpcService::handle_request()
*/
void NetworkServerIpcService::handle_request_with_bsdsock(IpcMessage* request, BSDSOCK* sock)
{
    switch (request->header.command)
    {
        case CGI_CMD_VIDEO_SNAPSHOT:
        case CGI_CMD_VIDEO_STREAM:
        case CGI_CMD_MULTICAST_STREAMING:
        {
            VideoStreamIpcExecutor* ipc = new VideoStreamIpcExecutor(request);
            ipc->initialize(sock, profileHandler, connectionManager, networkInterface, audio, scheduler, frontChannel, onvifRecordSettings);
            scheduler->add_task(ipc);
            break;
        }
        default:
            DP_ERR("unhandled request %d %d with bsdsock", request->header.command, request->header.action);
            delete sock;
            delete request;
            break;
    }
}


/**
@brief process CGI_CMD_NETWORK_PROTOCOL request. GET/SET actions are avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_protocol(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(0, sizeof(ProtocolGetResponse));
        ProtocolGetResponse* pRes = (ProtocolGetResponse *) response->body;
        Utility::SmartPointerUtils::shared_ptr<const HttpServerSetting> webserverSetting =
            networkService->get_setting<HttpServerSetting>();
        Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting> serverSetting =
            listener->get_serverSetting();

        memcpy(&pRes->http, &webserverSetting->http, sizeof(SimpleProtocolSetting));
        memcpy(&pRes->https, &webserverSetting->https, sizeof(SimpleProtocolSetting));
        memcpy(&pRes->rtsp, &serverSetting->rtsp, sizeof(SimpleProtocolSetting));
        memcpy(&pRes->vnp, &serverSetting->vnp, sizeof(SimpleProtocolSetting));
        memcpy(&pRes->webviewer, &serverSetting->vnp, sizeof(SimpleProtocolSetting));
        pRes->ignoreRtspTimeout = serverSetting->ignoreRtspTimeout;
        return APP_OK;
    }
    else if (request->header.action == IPC_ACTION_SET)
    {
        CHECK_IPC_SIZE(sizeof(ProtocolSetRequest), 0);
        ProtocolSetRequest* pReq = (ProtocolSetRequest*) request->body;
        Utility::SmartPointerUtils::shared_ptr<NetworkServerSetting> serverSetting =
            Utility::clone_sharedptr(listener->get_serverSetting());
        memcpy(&serverSetting->vnp, &pReq->vnp, sizeof(SimpleProtocolSetting));
        memcpy(&serverSetting->rtsp, &pReq->rtsp, sizeof(SimpleProtocolSetting));
        serverSetting->ignoreRtspTimeout = pReq->ignoreRtspTimeout;

        Utility::SmartPointerUtils::shared_ptr<HttpServerSetting> webserverSetting =
            Utility::clone_sharedptr(networkService->get_setting<HttpServerSetting>());
        memcpy(&webserverSetting->http, &pReq->http, sizeof(SimpleProtocolSetting));
        memcpy(&webserverSetting->https, &pReq->https, sizeof(SimpleProtocolSetting));

        if(!(networkInterface->check_overlap_port(webserverSetting->http.port, webserverSetting->https.port, serverSetting->rtsp.port, serverSetting->vnp.port)))
        {
            DP_ERR("[%s]process_protocol is failed!", __FUNCTION__);
            return APP_ERR_FAILED;
        }

        SettingChanger changer;
        changer.ADD_CHANGE(listener, &Listener::set_serverSetting, serverSetting);
        changer.ADD_TEMPLATE_CHANGE(networkService, NetworkServiceManager, set_setting, HttpServerSetting, webserverSetting);
        throw_taskevent(changer.change_setting(get_classID(), get_objectID(), request->header.peerAccount, request->header.peerIp));

        throw_taskevent(TASKEVENT_NW_PORT, TT_OPENSDK_MANAGER, 1);
        return changer.get_result();
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}


/**
@brief process CGI_CMD_USER request. GET_ALL/GET/SET/ADD/REMOVE actions are avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_user(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET_ALL)
    {
        CHECK_IPC_SIZE(0, sizeof(UserGetAllResponse));
        return get_users((UserGetAllResponse*) response->body);
    }
    else if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(sizeof(UserGetRequest), sizeof(UserGetResponse));
        return get_user((UserGetResponse*) response->body, (UserGetRequest*) request->body);
    }
    else if (request->header.action == IPC_ACTION_SET)
    {
        if (request->header.payloadLength == sizeof(UserSetRequest))
        {
            return set_user((UserSetRequest*) request->body);
        }
        else if (request->header.payloadLength == sizeof(UsersSetRequest))
        {
            return set_users((UsersSetRequest*) request->body);
        }
        else
        {
            DP_ERR("%d %d request size %d (%d or %d expected)", request->header.command, request->header.action, request->header.payloadLength, sizeof(UserSetRequest), sizeof(UsersSetRequest));
        }
        return APP_ERR_FAILED;
    }
    else if (request->header.action == IPC_ACTION_ADD)
    {
        CHECK_IPC_SIZE(sizeof(UserAddRequest), 0);
        return add_user((UserAddRequest*) request->body);
    }
    else if (request->header.action == IPC_ACTION_REMOVE)
    {
        CHECK_IPC_SIZE(sizeof(UserRemoveRequest), 0);
        return remove_user((UserRemoveRequest*) request->body);
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process get all users request(CGI_CMD_USER - IPC_ACTION_GET_ALL)
@param[out] pRes - response message body
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::get_users(UserGetAllResponse* pRes)
{
    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> accountSetting =
        accountManager->get_accountSetting();
    memcpy(&pRes->account, accountSetting.get(), sizeof(AccountSetting));
    return APP_OK;
}

/**
@brief process get user request(CGI_CMD_USER - IPC_ACTION_GET)
@param[out] pRes - response message body
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::get_user(UserGetResponse* pRes, UserGetRequest* pReq)
{
    if (accountManager->get_userSetting(&pRes->user, pReq->id) == 0)
    {
        return APP_ERR_USERNAME_NOT_FOUND;
    }
    else
    {
        return APP_OK;
    }
}

/**
@brief process set user request(CGI_CMD_USER - IPC_ACTION_SET - payload UserSetRequest)
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::set_user(UserSetRequest* pReq, const char* peerAccount, const char* peerIp)
{
#if 0
    Utility::SmartPointerUtils::shared_ptr<AccountSetting> account =
        Utility::clone_sharedptr(accountManager->get_accountSetting());
    UserSetting* target = find_userSetting(account.get(), pReq->user.id);
#else
    Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable> account =
        Utility::clone_sharedptr(accountManager->get_accountSettingSerializable());
    AccountManager::UserSettingSerializable* target = find_userSetting(account.get(), pReq->user.id);
#endif

    if (target == NULL)
    {
        return APP_ERR_USERNAME_NOT_FOUND;
    }
    else
    {
#if 0
        memcpy(target, &pReq->user, sizeof(UserSetting));
        return set_accountSetting(account, peerAccount, peerIp);
#else
        *target = pReq->user;
        return set_accountSetting(account, peerAccount, peerIp);
#endif
    }
}

/**
@brief process set users request(CGI_CMD_USER - IPC_ACTION_SET - payload UsersSetRequest)
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::set_users(UsersSetRequest* pReq, const char* peerAccount, const char* peerIp)
{
    Utility::SmartPointerUtils::shared_ptr<AccountSetting> account =
        Utility::clone_sharedptr(accountManager->get_accountSetting());
    memcpy(account.get(), &pReq->account, sizeof(AccountSetting));
    return set_accountSetting(account, peerAccount, peerIp);
}

/**
@brief process add user request(CGI_CMD_USER - IPC_ACTION_ADD)
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::add_user(UserAddRequest* pReq, const char* peerAccount, const char* peerIp)
{
    pReq->user.isEnable = 1;

    Utility::SmartPointerUtils::shared_ptr<AccountSetting> account =
        Utility::clone_sharedptr(accountManager->get_accountSetting());

    UserSetting* emptyUser = find_emptyUserSetting(account.get());

    if (emptyUser == NULL)
    {
        return APP_ERR_USER_MAX_LIMIT;
    }
    else
    {
        memcpy(emptyUser, &pReq->user, sizeof(UserSetting));
        return set_accountSetting(account, peerAccount, peerIp);
    }
}

/**
@brief process remove user request(CGI_CMD_USER - IPC_ACTION_REMOVE)
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::remove_user(UserRemoveRequest* pReq, const char* peerAccount, const char* peerIp)
{
    if (strlen(pReq->user.id) == 0 || strcmp(pReq->user.id, "admin") == 0 || strcmp(pReq->user.id, "guest") == 0)
    {
        DP_ERR("invalid userid: %s", pReq->user.id);
        return APP_ERR_DELETE_FIXED_USER;
    }
    Utility::SmartPointerUtils::shared_ptr<AccountSetting> account =
        Utility::clone_sharedptr(accountManager->get_accountSetting());

    UserSetting* target = find_userSetting(account.get(), pReq->user.id);

    if (target)
    {
        memset(target, 0, sizeof(UserSetting));
        target->level = USER_LEVEL_USER;
        return set_accountSetting(account, peerAccount, peerIp);
    }
    else
    {
        return APP_ERR_USERNAME_NOT_FOUND;
    }
}

/**
@brief find user setting of a certain id from account setting
@param accountSetting - AccountSetting
@param userId - user id
@return pointer of UserSetting in the \a accountSetting, or NULL if the id does not exist
*/
UserSetting* NetworkServerIpcService::find_userSetting(AccountSetting* accountSetting, const char* userId)
{
    if (strcmp(userId, "admin") == 0)
    {
        return &accountSetting->admin;
    }
    else if (strcmp(userId, "guest") == 0)
    {
        return &accountSetting->guest;
    }
    else
    {
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            if (strcmp(userId, accountSetting->user[i].id) == 0)
            {
                return &accountSetting->user[i];
            }
        }
    }
    return NULL;
}

/**
@brief find user setting of a certain id from account setting
@param accountSetting - AccountSettingSerializable
@param userId - user id
@return pointer of UserSettingSerializable in the \a accountSetting, or NULL if the id does not exist
*/
AccountManager::UserSettingSerializable* NetworkServerIpcService::find_userSetting(AccountManager::AccountSettingSerializable* accountSetting, const char* userId)
{
    if (strcmp(userId, "admin") == 0)
    {
        return &accountSetting->admin;
    }
    else if (strcmp(userId, "guest") == 0)
    {
        return &accountSetting->guest;
    }
    else
    {
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            if (strcmp(userId, accountSetting->user[i].id) == 0)
            {
                return &accountSetting->user[i];
            }
        }
    }
    return NULL;
}

/**
@brief find an empty user setting slot from account setting
@param accountSetting - AccountSetting
@return pointer of UserSetting in the \a accountSetting, or NULL if all user slots are in use
*/
UserSetting* NetworkServerIpcService::find_emptyUserSetting(AccountSetting* accountSetting)
{
    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        if (strlen(accountSetting->user[i].id) == 0)
        {
            return &accountSetting->user[i];
        }
    }

    // no empty usersetting. looking for a disabled user
    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        if (accountSetting->user[i].isEnable == 0)
        {
            return &accountSetting->user[i];
        }
    }

    return NULL;
}


/**
@brief process CGI_CMD_STREAM_URI request. GET action is avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_streamUri(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(sizeof(StreamUriGetRequest), sizeof(StreamUriGetResponse));
        StreamUriGetRequest *pReq = (StreamUriGetRequest *) request->body;
        StreamUriGetResponse *pRes = (StreamUriGetResponse *) response->body;

        bool isMulticastRequest = (pReq->stream == STREAM_URI_REQUEST_MULTICAST);
        bool isHttpRequest = (pReq->protocol == STREAM_URI_REQUEST_HTTP ||
                              pReq->tunnel1 == STREAM_URI_REQUEST_HTTP ||
                              pReq->tunnel2 == STREAM_URI_REQUEST_HTTP);
        // tcp uri and udp uri are same

        if (is_validStreamingMethod(isMulticastRequest, pReq->protocol, pReq->tunnel1, pReq->tunnel2) == 0)
        {
            return APP_ERR_INVALID_STREAM_SETUP;
        }

        int profileIndex;
        if (find_profileId_by_token(&profileIndex, pReq->profileToken) == 0)
        {
            return APP_ERR_PROFILE_NOT_EXIST;
        }

        char prefix[64];
        generate_uriPrefix(prefix, sizeof(prefix), isHttpRequest);
        if (isMulticastRequest)
        {
            snprintf(pRes->uri, sizeof(pRes->uri), "%s/onvif/multicast/profile%d/media.smp", prefix, profileIndex + 1);
        }
        else
        {
            snprintf(pRes->uri, sizeof(pRes->uri), "%s/onvif/profile%d/media.smp", prefix, profileIndex + 1);
        }
        pRes->invalidAfterReboot = 1;
        return APP_OK;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process CGI_CMD_STREAM_URI_V20 request. GET action is avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_streamUri20(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(sizeof(StreamUriV20GetRequest), sizeof(StreamUriGetResponse));
        StreamUriV20GetRequest *pReq = (StreamUriV20GetRequest *) request->body;
        StreamUriGetResponse *pRes = (StreamUriGetResponse *) response->body;

        bool isMulticastRequest = (pReq->streamType == STREAM_URI_REQUEST_MULTICAST);
        bool isHttpRequest = (pReq->RTSPOverHTTP == 1);
        // tcp uri and udp uri are same

        if (isMulticastRequest == true && pReq->protocol != STREAM_URI_REQUEST_UDP && pReq->protocol != STREAM_URI_REQUEST_NONE)
        {
            return APP_ERR_INVALID_STREAM_SETUP;
        }

        int profileIndex;
        if (find_profileId_by_token(&profileIndex, pReq->profileToken) == 0)
        {
            return APP_ERR_PROFILE_NOT_EXIST;
        }

        char prefix[64];
        generate_uriPrefix(prefix, sizeof(prefix), isHttpRequest);
        if(pReq->mediaType == STREAM_URI_MEDIATYPE_LIVE)
        {
            if (isMulticastRequest)
            {
                snprintf(pRes->uri, sizeof(pRes->uri), "%s/multicast/profile%d/media.smp", prefix, profileIndex + 1);
            }
            else
            {
                snprintf(pRes->uri, sizeof(pRes->uri), "%s/profile%d/media.smp", prefix, profileIndex + 1);
            }
        }
        else if(pReq->mediaType == STREAM_URI_MEDIATYPE_BACKUP)
        {
            snprintf(pRes->uri, sizeof(pRes->uri), "%s/recording/backup.smp", prefix);
        }
        else	// STREAM_URI_MEDIATYPE_SEARCH
        {
            snprintf(pRes->uri, sizeof(pRes->uri), "%s/recording/play.smp", prefix);
        }
        pRes->invalidAfterReboot = 1;

        return APP_OK;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process PXY_RP_GET_REPLAY_URI request. GET action is avaliable
	Get the ONVIF Profile-G Replay URI based on the request parameters
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_replayURI(IpcMessage* response, const IpcMessage* request)
{
    int retVal = APP_OK;
    bool isHttpRequest = false;

    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(sizeof(ReplayUriGetRequest), sizeof(ReplayUriGetResponse));

        ReplayUriGetRequest *pReq = (ReplayUriGetRequest *) request->body;
        ReplayUriGetResponse *pRes = (ReplayUriGetResponse *) response->body;

        do
        {
            /** Identifying Mulitcast or HTTP request */
            bool isMulticastRequest = (pReq->Stream == STREAM_URI_REQUEST_MULTICAST);

            isHttpRequest = (pReq->Protocol == STREAM_URI_REQUEST_HTTP ||
                             pReq->Tunnel1 == STREAM_URI_REQUEST_HTTP ||
                             pReq->Tunnel2 == STREAM_URI_REQUEST_HTTP);


            /** tcp uri and udp uri are same multicast is not supported for replay */
            if (isMulticastRequest == true )
            {
                retVal = APP_ERR_INVALID_STREAM_SETUP;
                break;
            }

            /** Is it Valid Stream Setup??? */
            /* VALID SET: {NONE NONE NONE}, {UDP NONE NONE}, {TCP NONE NONE},
            			  {TCP RTSP NONE}, {TCP RTSP HTTP}, {TCP HTTP NONE},
            			  {RTSP NONE NONE}, {RTSP HTTP NONE}, {HTTP NONE NONE}*/

            if (pReq->Tunnel2 == STREAM_URI_REQUEST_HTTP)
            {
                if (pReq->Tunnel1 == STREAM_URI_REQUEST_RTSP && pReq->Protocol == STREAM_URI_REQUEST_TCP)
                {
                    break;
                }
            }
            else if (pReq->Tunnel2 == STREAM_URI_REQUEST_NONE)
            {
                if (pReq->Tunnel1 == STREAM_URI_REQUEST_HTTP)
                {
                    if (pReq->Protocol == STREAM_URI_REQUEST_TCP || pReq->Protocol == STREAM_URI_REQUEST_RTSP)
                    {
                        break;
                    }
                }
                else if (pReq->Tunnel1 == STREAM_URI_REQUEST_RTSP)
                {
                    if (pReq->Protocol == STREAM_URI_REQUEST_TCP)
                    {
                        break;
                    }
                }
                else if (pReq->Tunnel1 == STREAM_URI_REQUEST_NONE)
                {
                    if (pReq->Protocol == STREAM_URI_REQUEST_NONE || pReq->Protocol == STREAM_URI_REQUEST_UDP ||
                            pReq->Protocol == STREAM_URI_REQUEST_TCP || pReq->Protocol == STREAM_URI_REQUEST_RTSP ||
                            pReq->Protocol == STREAM_URI_REQUEST_HTTP)
                    {

                        break;
                    }
                }
            }
            retVal = APP_ERR_INVALID_STREAM_SETUP;

        }
        while (0);

        if(retVal != APP_ERR_INVALID_STREAM_SETUP)
        {
            /** Validate recording token */
            if (onvifRecordSettings->IsValidRecordingToken(pReq->RecordToken) != APP_OK)
            {
                retVal = APP_ERR_INVALID_VALUE;
            }
            else
            {
                char prefix[64];
                generate_uriPrefix(prefix, sizeof(prefix), isHttpRequest);

                snprintf(pRes->Uri, sizeof(pRes->Uri), "%s/ProfileG/%s/recording/play.smp", prefix, pReq->RecordToken);
            }
        }

    }
    else
    {
        retVal = APP_ERR_SERVICE_NOT_SUPPORTED;
    }

    return retVal;
}


/**
@brief process CGI_CMD_SNAPSHOT_URI request. GET action is avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_snapshotUri(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(sizeof(SnapshotUriGetRequest), sizeof(SnapshotUriGetResponse));
        SnapshotUriGetRequest *pReq = (SnapshotUriGetRequest *) request->body;
        SnapshotUriGetResponse *pRes = (SnapshotUriGetResponse *) response->body;

        int profileIndex;
        if (find_profileId_by_token(&profileIndex, pReq->profileToken) == 0)
        {
            return APP_ERR_PROFILE_NOT_EXIST;
        }
        pRes->invalidAfterConnect = 0;
        pRes->invalidAfterReboot = 0;
        pRes->timeout = 0;


        char prefix[128];
        generate_uriPrefix(prefix, sizeof(prefix), true);
        snprintf(pRes->uri, sizeof(pRes->uri), "%s/stw-cgi/video.cgi?msubmenu=snapshot&Profile=%d&action=view", prefix,profileIndex+1);
        return APP_OK;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief uri prefix (protocol, ip, and port part)
@param[out] buf - uri buf
@param buflen - size of \a buf
@param isRtsp - true if http/https, false if rtsp
*/
void NetworkServerIpcService::generate_uriPrefix(char* buf, int buflen, bool isHttp)
{
    const char* method;
    char ip[16] = "";
    SimpleProtocolSetting protocolSetting;

    if (isHttp)
    {
        Utility::SmartPointerUtils::shared_ptr<const HttpServerSetting> webserverSetting =
            networkService->get_setting<HttpServerSetting>();
        if (webserverSetting->http.isEnable)
        {
            memcpy(&protocolSetting, &webserverSetting->http, sizeof(SimpleProtocolSetting));
        }
        else
        {
            memcpy(&protocolSetting, &webserverSetting->https, sizeof(SimpleProtocolSetting));
        }

        if (protocolSetting.protocolType == PROTOCOL_TYPE_HTTP)
        {
            method = "http";
        }
        else
        {
            method = "https";
        }
    }
    else
    {
        memcpy(&protocolSetting, &listener->get_serverSetting()->rtsp, sizeof(SimpleProtocolSetting));
        method = "rtsp";
    }
    networkInterface->get_currentIpv4Address(ip, NULL);

    snprintf(buf, buflen, "%s://%s:%d", method, ip, protocolSetting.port);
}






/**
@brief check if the streaming method is valid or not
	valid methods are:
		{NONE NONE NONE}, {UDP NONE NONE}, {TCP NONE NONE},
		{TCP RTSP NONE}, {TCP RTSP HTTP}, {TCP HTTP NONE},
		{RTSP NONE NONE}, {RTSP HTTP NONE}, {HTTP NONE NONE}
	in multicast request, protocol must be either UDP or NONE
@param protocol - main protocol
@param tunnel1 - first tunneling protocol
@param tunnel2 - secondary tunneling protocol
@return if valid return 1, else return 0
*/
int NetworkServerIpcService::is_validStreamingMethod(bool isMulticastRequest, int protocol, int tunnel1, int tunnel2)
{
    if (isMulticastRequest)
    {
        if (protocol != STREAM_URI_REQUEST_UDP && protocol != STREAM_URI_REQUEST_NONE)
        {
            return 0;
        }
    }

    if (tunnel2 == STREAM_URI_REQUEST_HTTP)
    {
        if (tunnel1 == STREAM_URI_REQUEST_RTSP && protocol == STREAM_URI_REQUEST_TCP)
        {
            return 1;
        }
    }
    else if (tunnel2 == STREAM_URI_REQUEST_NONE)
    {
        if (tunnel1 == STREAM_URI_REQUEST_HTTP)
        {
            if (protocol == STREAM_URI_REQUEST_TCP || protocol == STREAM_URI_REQUEST_RTSP)
            {
                return 1;
            }
        }
        else if (tunnel1 == STREAM_URI_REQUEST_RTSP)
        {
            if (protocol == STREAM_URI_REQUEST_TCP)
            {
                return 1;
            }
        }
        else if (tunnel1 == STREAM_URI_REQUEST_NONE)
        {
            if (protocol == STREAM_URI_REQUEST_NONE || protocol == STREAM_URI_REQUEST_UDP ||
                    protocol == STREAM_URI_REQUEST_TCP || protocol == STREAM_URI_REQUEST_RTSP ||
                    protocol == STREAM_URI_REQUEST_HTTP)
            {
                return 1;
            }
        }
    }

    return 0;
}

/**
@brief process CGI_CMD_PROFILE_ACCESS_INFORMATION request. GET action is avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_accessInfo(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(0, sizeof(AccessInformationGetResponse));
        AccessInformationGetResponse *pRes = (AccessInformationGetResponse *) response->body;

        Utility::SmartPointerUtils::shared_ptr<const VideoProfileSettingList> profiles = profileHandler->get_profileList();

        NetworkStreamingInfo output;
        connectionManager->get_networkStreamingInfo(&output);

        int realBitrate=0, realFramerate=0, atcMode=0, atcRate=0;
        int usedProfileMask = 0x00;
        bool isConnected = false;

        for (int i = 0; i < MAX_PROFILE_COUNT; i++)
        {
            realBitrate = realFramerate = atcMode = atcRate = 0;
            if(strlen(profiles->profile[i].name) != 0)
            {
                profileHandler->get_profileRunningInfoByProfileNum(i,realBitrate, realFramerate, atcMode, atcRate);
                int encoderId = profiles->profile[i].videoEncoderIndex;
                strlcpy(pRes->profileAccessInfo[i].profileName, profiles->profile[i].name, 16);
                pRes->profileAccessInfo[i].realBitrate = realBitrate/1024;
                pRes->profileAccessInfo[i].realFramerate = realFramerate;
                pRes->profileAccessInfo[i].atcRate = atcRate;
                pRes->profileAccessInfo[i].setBitrate = profiles->videoEncoder[encoderId].rate.bitrateLimit/1024;
                pRes->profileAccessInfo[i].setFramerate = profiles->videoEncoder[encoderId].rate.framerateLimit / 1000;
                pRes->profileAccessInfo[i].userCnt = 0;
            }

        }
        for (int i = 0; i < MAX_STREAM_COUNT; i++)
        {
            if (output.connection[i].occupied)
            {
                int profile = 0;
                usedProfileMask = 0x00;
                isConnected = false;

                for (int j = 0; j < MAX_STREAM_PER_CONNECTION; j++)
                {
                    profile = output.connection[i].usedProfile[j];
                    //exclude unused(-1) and setting preview profiles
                    if( profile == -1 || profile == EXTRA_SETTING_PROFILE_NUM || profile == DPTZ_SETTING_PROFILE_NUM || profile == DIS_SETTING_PROFILE_NUM)
                    {
                        continue;
                    }
                    usedProfileMask |= (1 << (profile));
                    pRes->profileAccessInfo[profile].userCnt++;
                    isConnected = true;
                }

                if ((isConnected) && usedProfileMask < (1 << MAX_PROFILE_COUNT))
                {
                    pRes->userAccessInfo[i].connectedProfile = usedProfileMask;
                    strlcpy(pRes->userAccessInfo[i].ip, output.connection[i].peerIp, 40);
                    pRes->userAccessInfo[i].realBitrate = output.connection[i].currentBitrate / 1024;
                    pRes->userAccessInfo[i].networkStatus = output.connection[i].networkStatus;
                }
            }
        }
        return APP_OK;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process CGI_CMD_NETWORK_CONFIGURATION request. GET/SET actions are avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_networkConfig(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(0, sizeof(NetworkGetResponse));
        return get_networkConfig((NetworkGetResponse*) response->body);
    }
    else if (request->header.action == IPC_ACTION_SET)
    {
        CHECK_IPC_SIZE(sizeof(NetworkSetRequest), sizeof(NetworkSetResponse));
        return set_networkConfig((NetworkSetResponse*) response->body, (NetworkSetRequest*) request->body,
                                 request->header.peerAccount, request->header.peerIp);
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process CGI_CMD_SYNC_POINT request. SET action is avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_syncPoint(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_SET)
    {
        CHECK_IPC_SIZE(sizeof(SyncPointGetRequest), 0);
        SyncPointGetRequest *pReq = (SyncPointGetRequest *) request->body;

        if (pReq->mode == SYNC_POINT_CMD_MODE_MEDIA)
        {
            int profile;
            if (find_profileId_by_token(&profile, pReq->profileToken) == 0)
            {
                return APP_ERR_PROFILE_NOT_EXIST;
            }
            throw_taskevent(EV_MEDIA_SYNC_REQUESTED, BROADCAST, profile);
        }
        else if (pReq->mode == SYNC_POINT_CMD_MODE_EVENT)
        {
            throw_taskevent(EV_USER_EVENT_SYNC_REQUESTED, BROADCAST);
        }
        else
        {
            DP_ERR("invalid mode: %d", pReq->mode);
            return APP_ERR_FAILED;
        }
        return APP_OK;
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

/**
@brief process network config get request (CGI_CMD_NETWORK_CONFIGURATION - IPC_ACTION_GET)
@param[out] pRes - response message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::get_networkConfig(NetworkGetResponse* pRes)
{
    Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting> serverSetting =
        listener->get_serverSetting();
    Utility::SmartPointerUtils::shared_ptr<const MiscNetworkInterfaceSetting> miscNetwork =
        networkInterface->get_miscNetworkInterfaceSetting();
    Utility::SmartPointerUtils::shared_ptr<const HttpServerSetting> webserver =
        networkService->get_setting<HttpServerSetting>();

    memcpy(&pRes->hostname, &miscNetwork->hostname, sizeof(HostnameSetting));
    memcpy(&pRes->dns, networkInterface->get_dnsSetting().get(), sizeof(DnsSetting));
    if (pRes->dns.dnsType != DNS_TYPE_MANUAL)
    {
        memcpy(&pRes->dns.autoDns, &pRes->dns.manualDns, sizeof(pRes->dns.manualDns));
    }
    memcpy(&pRes->eth, networkInterface->get_networkInterfaceSetting().get(), sizeof(NetworkInterfaceSetting));
    networkInterface->fill_dynamicNetworkInterfaceSetting(&pRes->eth);
    memcpy(&pRes->gateway, networkInterface->get_gatewaySetting().get(), sizeof(GatewaySetting));
    strlcpy(pRes->gateway.v4, networkInterface->get_gatewayInfo()->gatewayIp, 16);
    memcpy(&pRes->filter, networkService->get_setting<IpFilterSetting>().get(), sizeof(IpFilterSetting));
    memcpy(&pRes->zeroConfig, networkService->get_setting<ZeroConfigSetting>().get(), sizeof(ZeroConfigSetting));
    strlcpy(pRes->zeroConfig.token, pRes->eth.token, 64);
    memcpy(&pRes->http, &webserver->http, sizeof(SimpleProtocolSetting));
    memcpy(&pRes->https, &webserver->https, sizeof(SimpleProtocolSetting));
    memcpy(&pRes->rtsp, &serverSetting->rtsp, sizeof(SimpleProtocolSetting));
    memcpy(&pRes->vnp, &serverSetting->vnp, sizeof(SimpleProtocolSetting));
    memcpy(&pRes->webviewer, &serverSetting->vnp, sizeof(SimpleProtocolSetting));
    pRes->ignoreRtspTimeout = serverSetting->ignoreRtspTimeout;

    memcpy(&pRes->ddns, ddns_client->get_setting().get(), sizeof(DdnsSetting));
    memcpy(&pRes->ntp, ntpClient->get_setting().get(), sizeof(NtpSetting));
    memcpy(&pRes->smtp, smtpClient->get_setting().get(), sizeof(SmtpSetting));
    memcpy(&pRes->ftp, ftpClient->get_setting().get(), sizeof(FtpSetting));
    pRes->useQuickConnect = quickConnect->get_setting()->useQuickConnect;

    strlcpy(pRes->searchDomain, miscNetwork->searchDomain.searchDomain, 64);
    pRes->ipv6AcceptRouterAdvert = miscNetwork->networkDetail.IPv6RouterAdvertisement;

    return APP_OK;
}

/**
@brief process network config set request (CGI_CMD_NETWORK_CONFIGURATION - IPC_ACTION_SET)
@param[out] pRes - response message body
@param pReq - request message body
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::set_networkConfig(NetworkSetResponse* pRes, NetworkSetRequest* pReq, const char* peerAccount, const char* peerIp)
{
    SettingChanger changer;

    Utility::SmartPointerUtils::shared_ptr<MiscNetworkInterfaceSetting> miscNetwork =
        Utility::clone_sharedptr(networkInterface->get_miscNetworkInterfaceSetting());
    memcpy(&miscNetwork->hostname, &pReq->hostname, sizeof(HostnameSetting));
    strlcpy(miscNetwork->searchDomain.searchDomain, pReq->searchDomain, 64);
    miscNetwork->networkDetail.IPv6RouterAdvertisement = pReq->ipv6AcceptRouterAdvert;
    changer.ADD_CHANGE(networkInterface, &NetworkInterface::set_miscNetworkInterfaceSetting, miscNetwork);

    Utility::SmartPointerUtils::shared_ptr<NetworkServerSetting> serverSetting =
        Utility::clone_sharedptr(listener->get_serverSetting());
    memcpy(&serverSetting->rtsp, &pReq->rtsp, sizeof(SimpleProtocolSetting));
    memcpy(&serverSetting->vnp, &pReq->vnp, sizeof(SimpleProtocolSetting));
    serverSetting->ignoreRtspTimeout = pReq->ignoreRtspTimeout;

    Utility::SmartPointerUtils::shared_ptr<HttpServerSetting> webserver =
        Utility::clone_sharedptr(networkService->get_setting<HttpServerSetting>());
    memcpy(&webserver->http, &pReq->http, sizeof(SimpleProtocolSetting));
    memcpy(&webserver->https, &pReq->https, sizeof(SimpleProtocolSetting));

    if(!(networkInterface->check_overlap_port(webserver->http.port, webserver->https.port, serverSetting->rtsp.port, serverSetting->vnp.port)))
    {
        DP_ERR("[%s]set_networkConfig is failed! ", __FUNCTION__);
        return APP_ERR_FAILED;
    }
    changer.ADD_CHANGE(listener, &Listener::set_serverSetting, serverSetting);
    changer.ADD_TEMPLATE_CHANGE(networkService, NetworkServiceManager, set_setting, HttpServerSetting, webserver);

    Utility::SmartPointerUtils::shared_ptr<DnsSetting> dns(new DnsSetting(pReq->dns));
    changer.ADD_CHANGE(networkInterface, &NetworkInterface::set_dnsSetting, dns);

    Utility::SmartPointerUtils::shared_ptr<NetworkInterfaceSetting> eth(new NetworkInterfaceSetting(pReq->eth));
    changer.ADD_CHANGE(networkInterface, &NetworkInterface::set_networkInterfaceSetting, eth);

    Utility::SmartPointerUtils::shared_ptr<GatewaySetting> gateway(new GatewaySetting(pReq->gateway));
    changer.ADD_CHANGE(networkInterface, &NetworkInterface::set_gatewaySetting, gateway);

    Utility::SmartPointerUtils::shared_ptr<IpFilterSetting> filter(new IpFilterSetting(pReq->filter));
    changer.ADD_TEMPLATE_CHANGE(networkService, NetworkServiceManager, set_setting, IpFilterSetting, filter);

    Utility::SmartPointerUtils::shared_ptr<ZeroConfigSetting> zeroConfig(new ZeroConfigSetting(pReq->zeroConfig));
    changer.ADD_TEMPLATE_CHANGE(networkService, NetworkServiceManager, set_setting, ZeroConfigSetting, zeroConfig);

    Utility::SmartPointerUtils::shared_ptr<NtpSetting> ntp(new NtpSetting(pReq->ntp));
    changer.ADD_CHANGE(ntpClient, &NtpClient::set_setting, ntp);

    Utility::SmartPointerUtils::shared_ptr<DdnsSetting> ddns(new DdnsSetting(pReq->ddns));
    changer.ADD_CHANGE(ddns_client, &DdnsClient::set_setting, ddns);

    Utility::SmartPointerUtils::shared_ptr<SmtpSetting> smtp(new SmtpSetting(pReq->smtp));
    changer.ADD_CHANGE(smtpClient, &SmtpClient::set_setting, smtp);

    Utility::SmartPointerUtils::shared_ptr<FtpSetting> ftp(new FtpSetting(pReq->ftp));
    changer.ADD_CHANGE(ftpClient, &FtpClient::set_setting, ftp);

    Utility::SmartPointerUtils::shared_ptr<QuickConnectSetting> quickconnect =
        Utility::clone_sharedptr(quickConnect->get_setting());
    quickconnect->useQuickConnect = pReq->useQuickConnect;
    changer.ADD_CHANGE(quickConnect, &QuickConnectManager::set_setting, quickconnect);

    throw_taskevent(changer.change_setting(get_classID(), get_objectID(), peerAccount, peerIp));
    return changer.get_result();
}

/**
@brief set accountSetting
@param accountSetting - new setting data
@return IPC_ERR_CODE
*/
int NetworkServerIpcService::set_accountSetting(Utility::SmartPointerUtils::shared_ptr<AccountSetting>& accountSetting, const char* peerAccount, const char* peerIp)
{
    SettingChanger changer;

    changer.ADD_CHANGE(accountManager, &AccountManager::set_accountSetting, accountSetting);
    throw_taskevent(changer.change_setting(get_classID(), get_objectID(), peerAccount, peerIp));

    return changer.get_result();
}

/**
@brief set accountSetting
@param accountSetting - new setting data
@return IPC_ERR_CODE
*/
int NetworkServerIpcService::set_accountSetting(Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable>& accountSetting, const char* peerAccount, const char* peerIp)
{
    SettingChanger changer;

#if 1
    Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable> account_serializable;
    {
        account_serializable.reset(new AccountManager::AccountSettingSerializable);
        *(account_serializable.get()) = *(accountSetting.get());
    }

    changer.ADD_CHANGE(accountManager, &AccountManager::set_accountSettingSerializable, account_serializable);

    throw_taskevent(changer.change_setting(get_classID(), get_objectID(), peerAccount, peerIp));
#endif

    return changer.get_result();
}

/**
@brief find the profile index from profile token
@param[out] profileId - profile index
@param profileToken - profile token
@return if there is a profile with the token return 1, else return 0
*/
int NetworkServerIpcService::find_profileId_by_token(int* profileId, const char* profileToken)
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

/**
@brief process CGI_CMD_SECURITY_CONFIGURATION request. GET/SET actions are avaliable
@param[out] response - response message
@param request - request message
@return IPC_ERROR_CODE
*/
int NetworkServerIpcService::process_securityConfig(IpcMessage* response, const IpcMessage* request)
{
    if (request->header.action == IPC_ACTION_GET)
    {
        CHECK_IPC_SIZE(0, sizeof(SecurityGetResponse));
        SecurityGetResponse* pRes = (SecurityGetResponse*) response->body;
        memcpy(&pRes->account, accountManager->get_accountSetting().get(), sizeof(AccountSetting));
        memcpy(&pRes->tls, networkService->get_setting<TlsSetting>().get(), sizeof(TlsSetting));
        return APP_OK;
    }
    else if (request->header.action == IPC_ACTION_SET)
    {
        CHECK_IPC_SIZE(sizeof(SecuritySetRequest), 0);
        SecuritySetRequest* pReq = (SecuritySetRequest*) request->body;

        SettingChanger changer;
        Utility::SmartPointerUtils::shared_ptr<AccountSetting> account;
        Utility::SmartPointerUtils::shared_ptr<TlsSetting> tls;

        account.reset(new AccountSetting(pReq->account));
#if 0
        changer.ADD_CHANGE(accountManager, &AccountManager::set_accountSetting, account);
#else
        Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable> account_serializable(new AccountManager::AccountSettingSerializable);
        *(account_serializable.get()) = *(account.get());
        changer.ADD_CHANGE(accountManager, &AccountManager::set_accountSettingSerializable, account_serializable);
#endif

        tls.reset(new TlsSetting(pReq->tls));
        changer.ADD_TEMPLATE_CHANGE(networkService, NetworkServiceManager, set_setting, TlsSetting, tls);

        throw_taskevent(changer.change_setting(get_classID(), get_objectID(), request->header.peerAccount, request->header.peerIp));

        return changer.get_result();
    }
    return APP_ERR_SERVICE_NOT_SUPPORTED;
}

