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
@file network_server_ipc_service.h
@brief NetworkServerIpcService class declaration
*/
#ifndef _NETWORK_SERVER_IPC_SERVICE_H_
#define _NETWORK_SERVER_IPC_SERVICE_H_
#include "utility/utility_common/utility_wrapper.h"
#include "ipc/cgiipcdefine.h"
#include "framework/component/ipc_service.h"
#include "account_manager.h"

class AccountManager;
class ProfileHandler;
class ConnectionManager;
class NetworkInterface;
class NetworkServiceManager;
class Audio;
class ComponentScheduler;
class Listener;
class NtpClient;
class SmtpClient;
class FtpClient;
class DdnsClient;
class OnvifRecordSettingManager;
class QuickConnectManager;
class FrontChannel;

/**
@class NetworkServerIpcService
@brief handle IPC request related to network server component
*/
class NetworkServerIpcService : public IpcService
{
public:
    static NetworkServerIpcService* createnew(void);
    virtual ~NetworkServerIpcService(void);
    virtual int initialize(AccountManager* accountManager,
                           ProfileHandler* profileHandler,
                           ConnectionManager* connectionManager,
                           NetworkInterface* networkInterface,
                           NetworkServiceManager* networkService, Audio* audio,
                           ComponentScheduler* scheduler, Listener* listener,
                           FrontChannel* frontChannel,
                           NtpClient* ntpClient, SmtpClient* smtpClient, FtpClient* ftpClient,
                           DdnsClient* ddns_client,
                           OnvifRecordSettingManager* onvifRecordSettings,
                           QuickConnectManager* quickConnect);

protected:
    NetworkServerIpcService(void);

    virtual bool process_event(TaskEvent* event);
    virtual IpcMessage* handle_request(const IpcMessage* request);
    virtual void handle_request_with_bsdsock(IpcMessage* request, BSDSOCK* sock);

    int process_protocol(IpcMessage* response, const IpcMessage* request);
    virtual int process_user(IpcMessage* response, const IpcMessage* request);
    virtual int process_securityConfig(IpcMessage* response, const IpcMessage* request);
    int process_streamUri(IpcMessage* response, const IpcMessage* request);
    int process_streamUri20(IpcMessage* response, const IpcMessage* request);
    int process_snapshotUri(IpcMessage* response, const IpcMessage* request);
    int process_accessInfo(IpcMessage* response, const IpcMessage* request);
    int process_networkConfig(IpcMessage* response, const IpcMessage* request);
    int process_syncPoint(IpcMessage* response, const IpcMessage* request);

    /** ONVIF Replay Service */
    int process_replayURI(IpcMessage* response, const IpcMessage* request);

    int get_users(UserGetAllResponse* pRes);
    int get_user(UserGetResponse* pRes, UserGetRequest* pReq);
    int set_user(UserSetRequest* pReq, const char* peerAccount=NULL, const char* peerIp=NULL);
    int set_users(UsersSetRequest* pReq, const char* peerAccount=NULL, const char* peerIp=NULL);
    int add_user(UserAddRequest* pReq, const char* peerAccount=NULL, const char* peerIp=NULL);
    int remove_user(UserRemoveRequest* pReq, const char* peerAccount=NULL, const char* peerIp=NULL);
    int get_networkConfig(NetworkGetResponse* pRes);
    int set_networkConfig(NetworkSetResponse* pRes, NetworkSetRequest* pReq, const char* peerAccount=NULL, const char* peerIp=NULL);

    int set_accountSetting(Utility::SmartPointerUtils::shared_ptr<AccountSetting>& accountSetting, const char* peerAccount=NULL, const char* peerIp=NULL);
    int set_accountSetting(Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable>& accountSetting, const char* peerAccount=NULL, const char* peerIp=NULL);
    UserSetting* find_userSetting(AccountSetting* accountSetting, const char* userId);
    AccountManager::UserSettingSerializable* find_userSetting(AccountManager::AccountSettingSerializable* accountSetting, const char* userId);
    UserSetting* find_emptyUserSetting(AccountSetting* accountSetting);
    int find_profileId_by_token(int* profileId, const char* profileToken);

    void generate_uriPrefix(char* buf, int buflen, bool isHttp);
    int is_validStreamingMethod(bool isMulticastRequest, int protocol, int tunnel1, int tunnel2);

private:
    AccountManager* accountManager;
    ProfileHandler* profileHandler;
    ConnectionManager* connectionManager;
protected:
    NetworkInterface* networkInterface;
    NetworkServiceManager* networkService;
private:
    Audio* audio;
    ComponentScheduler* scheduler;
    Listener* listener;
    FrontChannel* frontChannel;
    NtpClient* ntpClient;
    SmtpClient* smtpClient;
    FtpClient* ftpClient;
    DdnsClient* ddns_client;
    OnvifRecordSettingManager* onvifRecordSettings;
    QuickConnectManager* quickConnect;
};

#endif


