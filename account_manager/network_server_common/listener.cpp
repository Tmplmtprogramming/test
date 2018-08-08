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
 * @file  listener.cpp
 * @brief Listener class implementation
 */
#include <stdio.h>
#include <stdlib.h>
#include "listener.h"

#include "utility/utility_common/utility_wrapper.h"
#include "socket/bsdsock.h"
#include "communication/network/network_interface/network_interface.h"
#include "data_access/file/file_reader.h"
#include "data_access/file/file_writer.h"
#include "setting/setting_common/setting_check_macro.h"
#include "setting/setting_common/setting_checker.h"
#include "network_server/vnp/vnp_checker.h"
#include "network_server/rtp/rtsp.h"
#include "network_server/cgi_ipc/ipc_handler.h"
#include "framework/scheduler/component_scheduler.h"
#include "record/onvif_record_setting_manager.h"

/**
@brief create new instance
@return new instance
*/
Listener* Listener::createnew(void)
{
    return new Listener();
}

/**
@brief constructor
*/
Listener::Listener(void) :
    IoTask("LISTENER", TT_LISTEN), running(true), vnpListen(NULL), rtspListen(NULL), ipcListen(NULL),
    taskManager(NULL), connectionManager(NULL), accountManager(NULL), profileHandler(NULL),
    audio(NULL), networkInterface(NULL), frontChannel(NULL), onvif_record_setting(NULL),networkService(NULL)
{
//	set_eventsource(false);
//	set_eventtarget(true);
    set_eventmask(EV_IP_CHANGED);
    set_eventmask(EV_VNP_SETTING_CHANGED);
    set_eventmask(EV_RTP_SETTING_CHANGED);
    set_eventmask(EV_SERVICE_SUSPENDED);
}

/**
@brief destructor
*/
Listener::~Listener(void)
{
    if (vnpListen)
    {
        delete_bsdsock(vnpListen);
    }
    if (rtspListen)
    {
        delete_bsdsock(rtspListen);
    }
    if (ipcListen)
    {
        delete_bsdsock(ipcListen);
    }
}

/**
@brief initialize associations, and start to listen
@param taskManager - ComponentScheduler
@param connectionManager - ConnectionManager
@param accountManager - AccountManager
@param profileHandler - ProfileHandler
@param audio - Audio
@param networkInterface - NetworkInterface
@return true on success, false on fail
@note most of the parameters are subcomponents' association. this should be improved later
*/
bool Listener::initialize(ComponentScheduler* taskManager,
                          ConnectionManager* connectionManager, AccountManager* accountManager,
                          ProfileHandler* profileHandler, Audio* audio,
                          NetworkInterface* networkInterface, FrontChannel* frontChannel,
                          OnvifRecordSettingManager* onvif_record_setting,
                          NetworkServiceManager* networkService)
{
    this->taskManager = taskManager;
    this->connectionManager = connectionManager;
    this->accountManager = accountManager;
    this->profileHandler = profileHandler;
    this->audio = audio;
    this->networkInterface = networkInterface;
    this->frontChannel = frontChannel;
    this->onvif_record_setting = onvif_record_setting;
    this->networkService = networkService;

    initialize_setting();
    init_listen();
    EventSource::initialize();
//	set_state(TS_RUNNING);

    return true;
}

/**
@brief handle task event
	re-create all tcp sockets on EV_IP_CHANGED(ipv6 might be changed)
		or EV_STREAM_PORT_SETTING_CHANGED (to avoid bind error)
@param event - event
@return true
*/
bool Listener::process_event(TaskEvent* event)
{
    switch(event->get_eventtype())
    {
        case EV_IP_CHANGED:
        case EV_VNP_SETTING_CHANGED:
        case EV_RTP_SETTING_CHANGED:
            delete_tcpsock();
            init_listen();
            break;
        case EV_SERVICE_SUSPENDED:
            suspend();
            break;
        default:
            DP_ERR("Unacceptable event %d", event->get_eventtype());
            break;
    }
    return true;
}

/**
@brief suspend
@return true
*/
bool Listener::suspend()
{
    change_runningstate(0);
    IoTask::suspend();
    return true;
}

/**
@brief change listener state
@param newRunning - new running state
*/
void Listener::change_runningstate(bool newRunning)
{
    running = newRunning;
}

/**
@brief initialize listen sockets
*/
void Listener::init_listen(void)
{
    char ipv6_enable = networkInterface->get_networkInterfaceSetting()->v6.isEnable;
    int ipType = ipv6_enable? TYPE_IPV6 : TYPE_IPV4;

    Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting> setting = serverSetting;

    if (setting->vnp.isEnable && vnpListen == NULL)
    {
        vnpListen = initialize_listensock(ipType, setting->vnp.port);
    }
    if (setting->rtsp.isEnable && rtspListen == NULL)
    {
        rtspListen = initialize_listensock(ipType, setting->rtsp.port);
    }
    if (ipcListen == NULL)
    {
        ipcListen = initialize_listensock(CGI_IPC_PATH);
    }
}

/**
@brief delete all tcp sockets before re-create sockets
*/
void Listener::delete_tcpsock(void)
{
    if (vnpListen)
    {
        delete_bsdsock(vnpListen);
        vnpListen = NULL;
    }
    if (rtspListen)
    {
        delete_bsdsock(rtspListen);
        rtspListen = NULL;
    }
}

/**
@brief accept socket and create socket handler task
@param listenSocket - listen socket to which the new connection is coming
*/
void Listener::on_bsdsockAccept(BSDSOCK *listenSocket)
{
    BSDSOCK* newSocket = listenSocket->get_acceptedsocket();
    if (newSocket == NULL)
    {
        DP_ERR("%p accept error", listenSocket);
        return;
    }

    if (listenSocket == vnpListen)
    {
        create_vnp(newSocket);
    }
    else if (listenSocket == rtspListen)
    {
        create_rtsp(newSocket);
    }
    else if (listenSocket == ipcListen)
    {
        create_ipc(newSocket);
    }
    else
    {
        DP_ERR("crazy unknown bsdsock %p accept %p", listenSocket, newSocket);
        delete newSocket;
    }
}

/**
@brief create a ProtocolDetector which will handle a newly accepted bsdsock
@param sock - accepted socket
*/
void Listener::create_vnp(BSDSOCK* sock)
{
    VnpChecker *checker = new VnpChecker();
    if (checker->initialize(sock, taskManager, connectionManager, accountManager, profileHandler, audio, networkInterface))
    {
        taskManager->add_task(checker);
    }
    else
    {
        delete checker;
    }
}

/**
@brief create a Rtsp which will handle a newly accepted bsdsock
@param sock - accepted socket
*/
void Listener::create_rtsp(BSDSOCK* sock)
{
    Rtsp* rtsp = Rtsp::createnew(serverSetting->ignoreRtspTimeout);
    if (rtsp->initialize(sock, taskManager, connectionManager, accountManager, profileHandler, audio, networkInterface, frontChannel, onvif_record_setting))
    {
        taskManager->add_task(rtsp);
    }
    else
    {
        delete rtsp;
    }
}

/**
@brief create a IpcHandler which will handle a newly accepted bsdsock
@param sock - accepted socket
*/
void Listener::create_ipc(BSDSOCK* sock)
{
    IpcHandler *handler = IpcHandler::createnew();
    if (handler->initialize(sock) == true)
    {
        taskManager->add_task(handler);
    }
    else
    {
        delete handler;
    }
}

/**
@brief initialize setting data
*/
void Listener::initialize_setting(void)
{
    SettingFileReader reader(NETWORK_SERVER_SETTING_FILE);
    if (reader)
    {
        reader >> serverSetting;
    }
    else
    {
        DP_ERR("create network server setting");
        reset_setting(HARD_DEFAULT_MODE);
        write_setting();
    }
}

/**
@brief reset setting data to the factory default values
       no change for soft reset mode
@param mode - default mode
*/
void Listener::reset_setting(RESET_SETTING_MODE mode)
{
    if (mode == HARD_DEFAULT_MODE)
    {
        NetworkServerSetting* newSetting = new NetworkServerSetting;

        memset(newSetting, 0, sizeof(NetworkServerSetting));
        newSetting->rtsp.protocolType = PROTOCOL_TYPE_RTSP;
        newSetting->rtsp.isEnable = 1;
        newSetting->rtsp.port = 554;
        newSetting->vnp.protocolType = PROTOCOL_TYPE_VNP;
        newSetting->vnp.isEnable = 1;
        newSetting->vnp.port = 4520;

        serverSetting.reset(newSetting);
    }
}

/**
@brief write setting data to the file
*/
int Listener::write_setting(void)
{
    SettingFileWriter writer(NETWORK_SERVER_SETTING_FILE);
    if (!writer)
    {
        DP_ERR("failed to write network server setting");
        return 0;
    }

    writer << serverSetting;
    return 1;
}

/**
@brief modify config restore data at \a restore_data_path.
       because we need to keep network related setting data during config restore,
       overwrite restore setting data of streaming server with current setting file.
@param restore_data_path - path of restore config files
*/
void Listener::modify_configRestoreData(const char* restore_data_path)
{
    UtilityWrapper::system_cmd("cp %s %s%s -f",
                               NETWORK_SERVER_SETTING_FILE, restore_data_path, NETWORK_SERVER_SETTING_FILE);
}

/**
@brief get NetworkServerSetting data in shared_ptr type
@return NetworkServerSetting
*/
Utility::SmartPointerUtils::shared_ptr<const NetworkServerSetting>
Listener::get_serverSetting(void)
{
    return serverSetting;
}

/**
@brief set NetworkServerSetting. this function should be followed by apply_settingChanges() or discard_settingChanges()
@param new_setting - new setting data
@param checker - SettingChecker
*/
void Listener::set_serverSetting(Utility::SmartPointerUtils::shared_ptr<NetworkServerSetting>& new_setting, SettingChecker* checker)
{
    const NetworkServerSetting* old_setting = serverSetting.get();

    bool prev_changed = checker->reset_changed();
    EventType event;
    int data = 0;

    event = EV_VNP_SETTING_CHANGED;
    CHECK_VALUE_READONLY(vnp.protocolType);
    CHECK_VALUE_READONLY(vnp.isEnable);
    CHECK_VALUE_FUNCTION_LOG(vnp.port, is_valid_port, PORT_SETTING_CHANGE_LOG, "Device Port", 4520);

    event = EV_RTP_SETTING_CHANGED;
    CHECK_VALUE_READONLY(rtsp.protocolType);
    CHECK_BOOLEAN(rtsp.isEnable);
    CHECK_VALUE_FUNCTION_LOG(rtsp.port, is_valid_port, PORT_SETTING_CHANGE_LOG, "RTSP Port", 554);

    CHECK_BOOLEAN_LOG(ignoreRtspTimeout, PORT_SETTING_CHANGE_LOG, "RTSP timeout ignore");

    if (checker->restore_changed(prev_changed))
    {
        serverSettingCandidate = new_setting;
    }
    // else: setting data is not changed, ignore new setting
}

/**
@brief check if a network port is valid or not
@param port - port number to check
@param defaultPort - default value
@return if the port is valid return true, else return false
*/
bool Listener::is_valid_port(int port, int defaultPort)
{
    if (port == defaultPort)
    {
        return true;
    }
    if (port < 1024 || port > 65535 || port == WS_DISCOVERY_PORT || port == UPNP_DISCOVERY_PORT)
    {
        return false;
    }
    return true;
}

/**
@brief apply setting candidates from set_setting functions
*/
void Listener::apply_settingChanges(void)
{
    if (serverSettingCandidate)
    {
        serverSetting = serverSettingCandidate;
        serverSettingCandidate.reset();
        write_setting();
    }
}

/**
@brief discard setting candidates from set_setting functions
*/
void Listener::discard_settingChanges(void)
{
    serverSettingCandidate.reset();
}

#if 0
/**
@brief replace port settings with original one
       (config restore doesn't modify network related settings)
       adjust audio authorization of each user using sysinfo
*/
void NETWORK_SERVER_SETTING::modify_restore_data(void)
{
    char restore_filename[64];
    snprintf(restore_filename, sizeof(restore_filename), "%s/%s", CONFIG_RESTORE_TMP_DIR, filename);
    NetworkServerSetting *restore_data = (NetworkServerSetting*) UtilityWrapper::malloc(Utility::MemoryUtils::ETC_REGION, setting_size);
    storage->read_settingfile(restore_filename, restore_data, setting_size);

    memcpy(&(restore_data->rtsp), &(setting->rtsp), sizeof(SimpleProtocolSetting));
    memcpy(&(restore_data->vnp), &(setting->vnp), sizeof(SimpleProtocolSetting));
    restore_data->ignoreRtspTimeout = setting->ignoreRtspTimeout;

    storage->write_settingfile(restore_filename, restore_data, setting_size);
    UtilityWrapper::free(restore_data);
}
#endif

