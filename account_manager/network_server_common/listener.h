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
 * @file    listener.h
 * @brief   Listener class and its data type declaration
 */
#ifndef _LISTENER_H_
#define _LISTENER_H_
#include <vector>
#include "framework/component/iotask.h"
#include "setting/setting_common/setting_struct.h"
#include "utility/utility_common/shared_ptr.h"
#include "communication/application/network_service_manager/network_service_manager.h"

#define NETWORK_SERVER_SETTING_FILE	"/mnt/setting/network_server.cfg"

class ComponentScheduler;
class AccountManager;
class ProfileHandler;
class Audio;
class ConnectionManager;
class Sysinfo;
class NetworkInterface;
class SettingChecker;
class FrontChannel;
class OnvifRecordSettingManager;

/**
@class Listener
@brief listen server sockets, and create accepted socket handler class
*/
class Listener : public IoTask
{
public:
    static Listener* createnew(void);
    virtual ~Listener(void);
    bool initialize(ComponentScheduler* taskManager,
                    ConnectionManager* connectionManager,
                    AccountManager* accountManager, ProfileHandler* profileHandler,
                    Audio* audio, NetworkInterface* networkInterface, FrontChannel* frontChannel,
                    OnvifRecordSettingManager* onvif_record_setting,
                    NetworkServiceManager* networkService=NULL);

    virtual Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting> get_serverSetting(void);
    virtual void set_serverSetting(Utility::SmartPointerUtils::shared_ptr<NetworkServerSetting>& serverSetting, SettingChecker* checker);

    virtual void apply_settingChanges(void);
    virtual void discard_settingChanges(void);
    virtual void reset_setting(RESET_SETTING_MODE mode);
    virtual int write_setting(void);
    virtual void modify_configRestoreData(const char* restore_data_path);

    void change_runningstate(bool newRunning);
    virtual bool suspend();

protected:
    Listener(void);

    virtual bool process_event(TaskEvent* event);
    virtual void on_bsdsockAccept(BSDSOCK *listenSocket);

    virtual void initialize_setting(void);
    bool is_valid_port(int port, int defaultPort);

    virtual void init_listen(void);
    void delete_tcpsock(void);

    void create_vnp(BSDSOCK* sock);
    virtual void create_rtsp(BSDSOCK* sock);
    virtual void create_ipc(BSDSOCK* sock);

    Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting> serverSetting;
    Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting> serverSettingCandidate;

private:
    bool running;

protected:
    BSDSOCK* vnpListen;
    BSDSOCK* rtspListen;
    BSDSOCK* ipcListen;
    ComponentScheduler* taskManager;
    ConnectionManager* connectionManager;
    AccountManager* accountManager;
    ProfileHandler* profileHandler;
    Audio* audio;
    NetworkInterface* networkInterface;
    FrontChannel* frontChannel;
    OnvifRecordSettingManager* onvif_record_setting;
    NetworkServiceManager* networkService;
};

#endif

