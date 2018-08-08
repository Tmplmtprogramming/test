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
 @file  account_manager.cpp
 @brief AccountManager class implementation
 */
#include "account_manager.h"

#include "utility/utility_common/utility_wrapper.h"
#include "data_access/file/file_reader.h"
#include "data_access/file/file_writer.h"
#include "framework/component/event_type.h"
#include "network_server/network_server_common/account_lock.h"
#include "setting/setting_common/setting_check_macro.h"
#include "setting/setting_common/setting_checker.h"
#include "data_container/event_data/smtp_event_buffer.h"

#define ACCOUNTLOCK_CHECK_INTERVAL	1000
#define ACCOUNTLOCK_CHECK_ONESHOT_ID EventSource::JOB_ID_0

/**
 @brief create new AccountManager
 @return new instance
 */
AccountManager* AccountManager::createnew(void)
{
    return new AccountManager();
}

/**
 @brief constructor
 */
AccountManager::AccountManager(void) :
        EventTarget("AccountManager", TT_ACCOUNT_MANAGER), Serializable()
{
    accountLockStatus = AccountLock::create(1, 5, 30000);	// block 30 seconds after 5 failed login attempts
    if (accountLockStatus)
    {
        set_eventmask(EV_USER_SETTING_CHANGED);
    }
    else
    {
        DP_ERR("AccountLock initialize failed");
    }
}

/**
 @brief destructor
 */
AccountManager::~AccountManager(void)
{
}

/**
 @brief set association, initialize accountlock
 */
bool AccountManager::initialize(void)
{
    initialize_setting();
    if (accountLockStatus)
    {
        update_accountlock_list();
        register_delayedjob(ACCOUNTLOCK_CHECK_INTERVAL, ACCOUNTLOCK_CHECK_ONESHOT_ID, EVENT_SENDING_COUNT_INFINITE);
    }
    EventSource::initialize();
    return 0;
}

/**
 @brief process task event
 - USER_SETTING_CHANGED : update account lock user info
 - DELAYED_JOB_ID_0 : check account lock state
 @param event - TaskEvent
 @return true
 */
bool AccountManager::process_event(TaskEvent* event)
{
    switch (event->get_eventtype())
    {
    case EV_USER_SETTING_CHANGED:
        update_accountlock_list();
        break;
    case DELAYEDJOB_EVENT(ACCOUNTLOCK_CHECK_ONESHOT_ID):
    {
        check_accountLockStatus();
#if !USE_NEW
        const_cast<AccountSettingSerializable*>(this->account_serializable.get())->admin.print();
#endif
        break;
    }
    default:
        DP_ERR("%d", event->get_eventtype());
        break;
    }
    return true;
}

/**
 @brief check account block status, and write log
 */
void AccountManager::check_accountLockStatus(void)
{
    AccountLock locker(ACCESS_NONE, "");
    check_failStatus(&locker);
    check_blockStatus(&locker);
}

/**
 *
 * @param locker
 */
void AccountManager::check_blockStatus(AccountLock* locker)
{
    BanInfoResult banInfoResult[USER_INFO_COUNT];
    int count = locker->get_banInfoResult(banInfoResult);

    const char* protocol_name[] =
    { "", "HTTP(S)", "SVNP", "RTSP", "IP Installer", "ONVIF" };
    int access_protocol = 0;
    for (int i = 0; i < count; i++)
    {
        switch (banInfoResult[i].access_protocol)
        {
        case ACCESS_FROM_HTTP: /* FALL THROUGH */
        case ACCESS_FROM_SVNP: /* FALL THROUGH */
        case ACCESS_FROM_RTSP: /* FALL THROUGH */
        case ACCESS_FROM_IPINSTALLER: /* FALL THROUGH */
        case ACCESS_FROM_ONVIF: /* FALL THROUGH */
        {
            access_protocol = banInfoResult[i].access_protocol;
            break;
        }
        default:
        {
            DP_ERR("protocol %d", banInfoResult[i].access_protocol);
            access_protocol = ACCESS_NONE;
            break;
        }
        }

        char logmsg[128];
        snprintf(logmsg, sizeof(logmsg), "%s has been blocked due to suspicious %s access from %s", banInfoResult[i].id,
                protocol_name[access_protocol], banInfoResult[i].ip);
        struct timeval tv;
        gettimeofday(&tv, NULL);

        if (!strncmp(banInfoResult[i].id, "admin", MAX_ID_LEN))
        {
            write_log(LogData::ACCESS_LOG, LogData::ACE_ADMINLOGIN, 1, tv.tv_sec, logmsg);
        }
        else if (!strncmp(banInfoResult[i].id, "guest", MAX_ID_LEN))
        {
            write_log(LogData::ACCESS_LOG, LogData::ACE_GUESTLOGIN, 1, tv.tv_sec, logmsg);
        }
        else
        {
            write_log(LogData::ACCESS_LOG, LogData::ACE_USERLOGIN, 1, tv.tv_sec, logmsg);
        }

        char event_info[40] = "Account blocked";
        char mail_msg[256];
        strncpy(mail_msg, logmsg, strlen(logmsg));

        SMTP_EVENT_BUF* smtp_request = new SMTP_EVENT_BUF;
        ::memset(smtp_request, 0x00, sizeof(SMTP_EVENT_BUF));
        memcpy(smtp_request->event_info, event_info, (strlen(event_info) > sizeof(smtp_request->event_info)-1) ? sizeof(smtp_request->event_info)-1:strlen(event_info));
        memcpy(smtp_request->mail_msg, mail_msg, (strlen(mail_msg) > sizeof(smtp_request->mail_msg)-1) ? sizeof(smtp_request->mail_msg)-1:strlen(mail_msg));
        gettimeofday(&smtp_request->etime, NULL);
        smtp_request->type = -1;    // no attachment

        throw_taskEventWithNewData(EV_SNAPSHOT_EMAIL_SEND, TT_SMTP_CLIENT, smtp_request);
    }
}

/**
 @brief check account login fail status, and write log
 */
void AccountManager::check_failStatus(AccountLock* locker)
{
    return; // if you want to use PeerAccessingLog(Login Fail), please remove this line

    FailInfoResult failInfoResult[USER_INFO_COUNT * FAIL_INFO_COUNT];
    int count = locker->get_failInfoResult(failInfoResult);

    const char* protocol_name[] =
    { "", "HTTP(S)", "SVNP", "RTSP", "IP Installer", "ONVIF" };
    int access_protocol = 0;
    for (int i = 0; i < count; i++)
    {
        switch (failInfoResult[i].access_protocol)
        {
        case ACCESS_FROM_HTTP: // FALL THROUGH
        case ACCESS_FROM_SVNP: /* FALL THROUGH */
        case ACCESS_FROM_RTSP: /* FALL THROUGH */
        case ACCESS_FROM_IPINSTALLER: /* FALL THROUGH */
        case ACCESS_FROM_ONVIF: /* FALL THROUGH */
        {
            access_protocol = failInfoResult[i].access_protocol;
            break;
        }
        default:
        {
            DP_ERR("protocol %d", failInfoResult[i].access_protocol);
            access_protocol = ACCESS_NONE;
            break;
        }
        }

        char logmsg[128];
        snprintf(logmsg, sizeof(logmsg), "%s %s log in fail : %s", protocol_name[access_protocol], failInfoResult[i].id,
                failInfoResult[i].ip);

        if (!strncmp(failInfoResult[i].id, "admin", MAX_ID_LEN))
        {
            write_log(LogData::ACCESS_LOG, LogData::ACE_ADMINLOGIN, 1, failInfoResult[i].time, logmsg);
        }
        else if (!strncmp(failInfoResult[i].id, "guest", MAX_ID_LEN))
        {
            write_log(LogData::ACCESS_LOG, LogData::ACE_GUESTLOGIN, 1, failInfoResult[i].time, logmsg);
        }
        else
        {
            write_log(LogData::ACCESS_LOG, LogData::ACE_USERLOGIN, 1, failInfoResult[i].time, logmsg);
        }
    }
}

/**
 @brief update accountlock user list
 */
void AccountManager::update_accountlock_list(void)
{
    const char *id[MAX_USER_COUNT + 2];
    char buf[MAX_USER_COUNT + 2][MAX_USER_NAME_LEN];
    int count = 0;

#if USE_NEW
    Utility::SmartPointerUtils::shared_ptr<const AccountSettingSerializable> setting = this->account_serializable;
#else
    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> setting = account;
#endif

    if (setting->admin.isEnable)
    {
        memcpy(buf[count], setting->admin.id, MAX_USER_NAME_LEN);
        id[count] = buf[count];
        count++;
    }

    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        if (setting->user[i].isEnable)
        {
            memcpy(buf[count], setting->user[i].id, MAX_USER_NAME_LEN);
            id[count] = buf[count];
            count++;
        }
    }

    if (setting->guest.isEnable)
    {
        memcpy(buf[count], setting->guest.id, MAX_USER_NAME_LEN);
        id[count] = buf[count];
        count++;
    }

    AccountLock locker(ACCESS_NONE, "");
    locker.update_userlist(id, count);
}

/**
 @brief get user setting
 @param[out] usersetting - UserSetting struct buffer
 @param id - user id to find
 @return 1 on success, 0 on fail
 */
int AccountManager::get_userSetting(UserSetting* usersetting, const char *id)
{
#if USE_NEW
    Utility::SmartPointerUtils::shared_ptr<const AccountSettingSerializable> setting = this->account_serializable;
#else
    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> setting = account;
#endif
    if (strcmp(id, "admin") == 0)
    {
        memcpy(usersetting, &setting->admin, sizeof(UserSetting));
        return 1;
    }
    else if (strcmp(id, "guest") == 0)
    {
        memcpy(usersetting, &setting->guest, sizeof(UserSetting));
        return 1;
    }
    else
    {
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            if (strcmp(id, setting->user[i].id) == 0)
            {
                memcpy(usersetting, &setting->user[i], sizeof(UserSetting));
                return 1;
            }
        }
    }
    return 0;
}

/**
 @brief check if anonymous user access is allowed or not
 @param isOnvifAccess - true if the connection is from ONVIF
 @return if anonymous is allowed return true, else return false
 */
bool AccountManager::is_anonymous_allowed(bool isOnvifAccess)
{
#if USE_NEW
    if (account_serializable->guest.authProfile)
#else
    if (account->guest.authProfile)
#endif
    {
        return true;
    }

    if (isOnvifAccess)
    {
        if (*enableOnvifAuth == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 @brief get current admin password status
 @return status
 @see enum ADMIN_PASSWORD_STATUS
 */
ADMIN_PASSWORD_STATUS AccountManager::get_adminPasswordStatus(void)
{
    IPC_ERROR_CODE errcode;
#if USE_NEW
    Utility::SmartPointerUtils::shared_ptr<const AccountSettingSerializable> setting = this->account_serializable;
#else
    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> setting = account;
#endif

    if (strcmp(setting->admin.password, "4321") == 0)
    {
        return DEFAULT_ADMIN_PASSWORD;
    }
    else if (is_valid_user_password(setting->admin.password, &errcode) == false)
    {
        return WEAK_ADMIN_PASSWORD;
    }
    return VALID_ADMIN_PASSWORD;
}

/**
 @brief initialize setting data
 */
void AccountManager::initialize_setting(void)
{
    this->account_serializable.reset(new AccountSettingSerializable);
    this->enableOnvifAuthSerializable.reset(new char);
    this->create_settingParser();
    SettingFileReader reader(ACCOUNTMANAGER_SETTING_FILE);
    if (reader)
    {
#if USE_NEW
        reader >> account >> enableOnvifAuth;
#else
        reader >> enableOnvifAuth;
#endif
    }
    else
    {
        DP_ERR("create account setting");
        reset_setting(HARD_DEFAULT_MODE);
        write_setting();
    }
#if USE_NEW
    try
    {
        cout << "HERE00" << endl;
        this->get_parser()->openIn(string("/mnt/setting"), string("account.cfg"));
    } catch (techwin::Exception& e)
    {
        cout << "Exception!! : " << e.what() << endl;
        // temporal
        UtilityWrapper::system_cmd("touch %s/%s", "/mnt/setting", "account.cfg.txt");
        this->get_parser()->openIn(string("/mnt/setting"), string("account.cfg"));
    }
    cout << "HERE01" << endl;
    try
    {
        this->get_parser()->read();
        cout << "HERE02" << endl;
    } catch (techwin::Exception& e)
    {
        cout << "HERE02.5" << e.what() << endl;
    }
    try
    {
        cout << "HERE03" << endl;
        this->get_parser()->load_dataToClass(*this, "account.cfg");
        cout << "HERE04" << endl;
        this->get_parser()->closeIn();
        cout << "HERE05" << endl;
    } catch (Exception& e)
    {
        cout << "HERE05.1" << e.what() << endl;
        this->get_parser()->closeIn();
        {
            cout << "HERE05.2" << endl;
            // Resetting....
#if 1
            account_serializable->reset();
            account_serializable->admin.isEnable = 1;
            strlcpy(account_serializable->admin.id, "admin", 16);
            strlcpy(account_serializable->admin.password, "4321", 16);
            account_serializable->admin.level = USER_LEVEL_ADMIN;
            account_serializable->admin.onvifUserLevel = ONVIFUSER_LEVEL_ADMIN;
            account_serializable->admin.authProfile = 1;
            account_serializable->admin.authAudioIn = 1;
            account_serializable->admin.authAudioOut = 1;
            account_serializable->admin.authRelay = 1;
            account_serializable->admin.authPTZ = 1;
            for (int i = 0; i < MAX_USER_COUNT; i++)
            {
                snprintf(account_serializable->user[i].id, 16, "user%d", i + 1);
                snprintf(account_serializable->user[i].password, 16, "user%d", i + 1);
                account_serializable->user[i].level = USER_LEVEL_USER;
                account_serializable->user[i].onvifUserLevel = ONVIFUSER_LEVEL_ANONYMOUS;	// MediaUser
            }
            strlcpy(account_serializable->guest.id, "guest", 16);
            strlcpy(account_serializable->guest.password, "guest", 16);
            account_serializable->guest.level = USER_LEVEL_GUEST;
            account_serializable->guest.onvifUserLevel = ONVIFUSER_LEVEL_ANONYMOUS;
        }
        cout << "HERE05.3" << endl;
        string buffer(ACCOUNTMANAGER_SETTING_FILE);
        string directory_string(buffer.substr(0, buffer.find_last_of("/\\")));
        string file_string(buffer.substr(buffer.find_last_of("/\\")+1));
#if 1
        this->get_parser()->openOut(directory_string, file_string);
        cout << "HERE05.4" << endl;
        this->get_parser()->store_classDataToParser(*this, "account.cfg");
        cout << "HERE05.5" << endl;
        this->get_parser()->write();
        cout << "HERE05.6" << endl;
        this->get_parser()->closeOut();
        cout << "HERE05.7" << endl;
#else
        this->get_parser()->serialize_out(this->account_serializable, directory_string, file_string);
#endif
#else
#endif
    }
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
#endif
}

/**
 @brief reset setting data to the factory default values
 @param mode - default mode
 */
void AccountManager::reset_setting(RESET_SETTING_MODE mode)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
#if !USE_NEW
    AccountSetting* accountSetting = new AccountSetting;
    memset(accountSetting, 0, sizeof(AccountSetting));
    accountSetting->admin.isEnable = 1;
    strlcpy(accountSetting->admin.id, "admin", 16);
    strlcpy(accountSetting->admin.password, "4321", 16);
    accountSetting->admin.level = USER_LEVEL_ADMIN;
    accountSetting->admin.onvifUserLevel = ONVIFUSER_LEVEL_ADMIN;
    accountSetting->admin.authProfile = 1;
    accountSetting->admin.authAudioIn = 1;
    accountSetting->admin.authAudioOut = 1;
    accountSetting->admin.authRelay = 1;
    accountSetting->admin.authPTZ = 1;
    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        snprintf(accountSetting->user[i].id, 16, "user%d", i + 1);
        snprintf(accountSetting->user[i].password, 16, "user%d", i + 1);
        accountSetting->user[i].level = USER_LEVEL_USER;
        accountSetting->user[i].onvifUserLevel = ONVIFUSER_LEVEL_ANONYMOUS;	// MediaUser
    }
    strlcpy(accountSetting->guest.id, "guest", 16);
    strlcpy(accountSetting->guest.password, "guest", 16);
    accountSetting->guest.level = USER_LEVEL_GUEST;
    accountSetting->guest.onvifUserLevel = ONVIFUSER_LEVEL_ANONYMOUS;
    account.reset(accountSetting);

    char* enable = new char(1);
    enableOnvifAuth.reset(enable);
#else
    this->account_serializable->reset();
    {
            account_serializable->admin.isEnable = 1;
            strlcpy(const_cast<AccountSettingSerializable*>(this->account_serializable.get())->admin.id, "admin", 16);
            strlcpy(const_cast<AccountSettingSerializable*>(this->account_serializable.get())->admin.password, "4321", 16);
            account_serializable->admin.level = USER_LEVEL_ADMIN;
            account_serializable->admin.onvifUserLevel = ONVIFUSER_LEVEL_ADMIN;
            account_serializable->admin.authProfile = 1;
            account_serializable->admin.authAudioIn = 1;
            account_serializable->admin.authAudioOut = 1;
            account_serializable->admin.authRelay = 1;
            account_serializable->admin.authPTZ = 1;
            for (int i = 0; i < MAX_USER_COUNT; i++)
            {
                snprintf(const_cast<AccountSettingSerializable*>(this->account_serializable.get())->user[i].id, 16, "user%d", i + 1);
                snprintf(const_cast<AccountSettingSerializable*>(this->account_serializable.get())->user[i].password, 16, "user%d", i + 1);
                const_cast<AccountSettingSerializable*>(this->account_serializable.get())->user[i].level = USER_LEVEL_USER;
                account_serializable->user[i].onvifUserLevel = ONVIFUSER_LEVEL_ANONYMOUS;	// MediaUser
            }
            strlcpy(const_cast<AccountSettingSerializable*>(account_serializable.get())->guest.id, "guest", 16);
            strlcpy(const_cast<AccountSettingSerializable*>(account_serializable.get())->guest.password, "guest", 16);
            account_serializable->guest.level = USER_LEVEL_GUEST;
            account_serializable->guest.onvifUserLevel = ONVIFUSER_LEVEL_ANONYMOUS;
    }
    this->account.reset(new AccountSetting);
    account_serializable->convert_toBitStream(*(const_cast<AccountSetting*>(account.get())));

    char* enable = new char(1);
    enableOnvifAuth.reset(enable);
    this->enableOnvifAuthSerializable.reset(new char);
    *(const_cast<char*>(enableOnvifAuthSerializable.get())) = '\0';
    //std::cout << "~~" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
#endif
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief write setting data to the file
 @return 1 on success, 0 on failure
 */
int AccountManager::write_setting(void)
{
#if !USE_NEW
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    SettingFileWriter writer(ACCOUNTMANAGER_SETTING_FILE);
    if (!writer)
    {
        DP_ERR("failed to write account setting");
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return 0;
    }

    writer << account << enableOnvifAuth;
#else
    SettingFileWriter writer(ACCOUNTMANAGER_SETTING_FILE);
    if (!writer)
    {
        DP_ERR("failed to write account setting");
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return 0;
    }

    writer << account << enableOnvifAuth;
    try
    {
#if 1
        this->get_parser()->openOut("/mnt/setting/", "account.cfg");
//        this->get_parser()->store_classDataToParser(*(const_cast<AccountSettingSerializable*>(this->account_serializable.get())), "account.cfg");
        this->get_parser()->store_classDataToParser(*this, "account.cfg");
        this->get_parser()->write();
        this->get_parser()->closeOut();
#else
        // dieing if un commanted.
        string d("/mnt/setting/");
        string f("account.cfg");
        this->get_parser()->serialize_out(this->account_serializable,/*"/mnt/setting/", "account.cfg"*/ d, f);
#endif
    }
    catch(techwin::Exception& e)
    {
        std::cout << e.what() << std::endl;
    }
#endif
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return 1;
}

/**
 @brief modify config restore data at \a restore_data_path.
 check consistency of audio authority of users and camera capability
 @param restore_data_path - path of restore config files
 */
void AccountManager::modify_configRestoreData(const char* restore_data_path)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    Utility::SmartPointerUtils::shared_ptr<AccountSetting> new_account;
    Utility::SmartPointerUtils::shared_ptr<char> new_enableOnvifAuth;

    char restore_file_name[128];
    snprintf(restore_file_name, 128, "%s%s", restore_data_path, ACCOUNTMANAGER_SETTING_FILE);

    SettingFileReader reader(restore_file_name);
    if (!reader)
    {
        DP_ERR("no account setting???");
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return;
    }

#if! USE_NEW
    reader >> new_account >> new_enableOnvifAuth;

    if (UtilityWrapper::get_modelInfo(MODELINFO_SUPPORT_AUDIO) == 0)
    {
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            new_account->user[i].authAudioIn = 0;
            new_account->user[i].authAudioOut = 0;
        }
        new_account->guest.authAudioIn = 0;
        new_account->guest.authAudioOut = 0;
    }

    SettingFileWriter writer(restore_file_name, false);
    if (!writer)
    {
        DP_ERR("write fail???");
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return;
    }
    writer << new_account << new_enableOnvifAuth;
#else
    reader >> new_enableOnvifAuth;
    try
    {
//        char restore_file_name[128];
//        snprintf(restore_file_name, 128, "%s%s", restore_data_path, "/mnt/setting");
        this->get_parser()->openIn(restore_file_name);
        this->get_parser()->read();
#if 0
        this->get_parser()->load_dataToClass(*(const_cast<AccountSettingSerializable*>(this->account_serializable.get())), "account.cfg");
#else
        this->get_parser()->load_dataToClass(*this, "account.cfg");
#endif
        this->get_parser()->closeIn();
    } catch (techwin::Exception& e)
    {
        cout << "Exception!! : " << e.what() << endl;
    }
    try
    {
        this->get_parser()->openOut("/mnt/setting/", "account.cfg");
#if 0
        this->get_parser()->store_classDataToParser(*(const_cast<AccountSettingSerializable*>(this->account_serializable.get())), "account.cfg");
#else
        this->get_parser()->store_classDataToParser(*this, "account.cfg");
#endif
        this->get_parser()->write();
        this->get_parser()->closeOut();
    }
    catch(techwin::Exception& e)
    {
        std::cout << e.what() << std::endl;
    }
#endif
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief get AccountSetting data in shared_ptr type
 @return AccountSetting
 */
Utility::SmartPointerUtils::shared_ptr<const AccountSetting> AccountManager::get_accountSetting(void)
{
//    std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
#if 0
    return account;
#else
    Utility::SmartPointerUtils::shared_ptr<AccountSetting> account_setting;
    AccountSetting* new_account_setting_ptr = new AccountSetting;
    this->account_serializable->convert_toBitStream(*new_account_setting_ptr);
    account_setting.reset(new_account_setting_ptr);
//    std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return account_setting;
#endif
}

/**
 @brief set AccountSetting. this function should be followed by apply_settingChanges() or discard_settingChanges()
 @param new_account - new setting data
 @param checker - SettingChecker
 */
void AccountManager::set_accountSetting(Utility::SmartPointerUtils::shared_ptr<AccountSetting>& new_account,
        SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
#if !USE_NEW
    bool prev_changed = checker->reset_changed();

    check_accountSetting(new_account.get(), account.get(), checker);

    if (checker->restore_changed(prev_changed))
    {
        accountCandidate = new_account;
    }
    // else: setting data is not changed, ignore new setting
#else
    Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable> new_account_serializable(new AccountManager::AccountSettingSerializable);
    *(new_account_serializable.get()) = *(new_account.get());
    this->set_accountSettingSerializable(new_account_serializable, checker);
#endif
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

#if USE_NEW
Utility::SmartPointerUtils::shared_ptr<const AccountManager::AccountSettingSerializable> AccountManager::get_accountSettingSerializable(void)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return this->account_serializable;
}

void AccountManager::set_accountSettingSerializable(Utility::SmartPointerUtils::shared_ptr<AccountManager::AccountSettingSerializable>& new_account, SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    bool prev_changed = checker->reset_changed();

    check_accountSetting(new_account.get(), account_serializable.get(), checker);

    if (checker->restore_changed(prev_changed))
    {
        accountCandidateSerializable = new_account;
    }
    // else: setting data is not changed, ignore new setting
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}
#endif


/**
 @brief check setting change
 @param[in,out] new_setting - new setting
 @param old_setting - old setting
 */
//void AccountManager::check_accountSetting(AccountSetting* new_setting, const AccountSetting* old_setting,
//        SettingChecker* checker)
//{
//    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//    check_admin_account_setting_change(&new_setting->admin, &old_setting->admin, checker);
//
//    bool prev_changed = checker->reset_changed();
//    for (int i = 0; i < MAX_USER_COUNT; i++)
//    {
//        check_user_account_setting_change(&new_setting->user[i], &old_setting->user[i], i + 1, checker);
//    }
//    if (checker->restore_changed(prev_changed))
//    {
//        check_user_name_collison(new_setting);
//    }
//
//    check_guest_account_setting_change(&new_setting->guest, &old_setting->guest, checker);
//    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//}
#if USE_NEW
/**
 @brief check setting change
 @param[in,out] new_setting - new setting
 @param old_setting - old setting
 */
void AccountManager::check_accountSetting(AccountSettingSerializable* new_setting, const AccountSettingSerializable* old_setting,
        SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    check_admin_account_setting_change(&new_setting->admin, &old_setting->admin, checker);

    bool prev_changed = checker->reset_changed();
    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        check_user_account_setting_change(&new_setting->user[i], &old_setting->user[i], i + 1, checker);
    }
    if (checker->restore_changed(prev_changed))
    {
        check_user_name_collison(new_setting);
    }

    check_guest_account_setting_change(&new_setting->guest, &old_setting->guest, checker);
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief check setting change
 @param[in,out] new_setting new setting
 @param[in] old_setting old setting
 */
void AccountManager::check_admin_account_setting_change(UserSettingSerializable *new_setting, const UserSettingSerializable *old_setting,
        SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    EventType event = EV_USER_SETTING_CHANGED;
    int data = 0;

    IPC_ERROR_CODE errcode = APP_ERR_FAILED;

    CHECK_VALUE_READONLY(isEnable);
    CHECK_STRING_READONLY(id, 16);

    if (strlen(new_setting->password) == 0)
    {
        memcpy(new_setting->password, old_setting->password, MAX_USER_PASSWORD_LEN);
    }
    bool prev_changed = checker->reset_changed();
    CHECK_PASSWORD_LOG(password, MAX_USER_PASSWORD_LEN, USER_SETTING_CHANGE_LOG, "Administrator Password");

    if (checker->restore_changed(prev_changed))	// admin password changed
    {
        if (strcmp(new_setting->id, new_setting->password) == 0)
        {
            checker->check_validity(false, errcode);
        }
        checker->check_validity(is_valid_user_password(new_setting->password, &errcode), errcode);
        if (strncmp(old_setting->password, "4321", 4) == 0)
        {
            checker->add_change(EV_ADMIN_PASSWORD_CHANGED, 0);
        }
    }

    CHECK_VALUE_READONLY(level);
    CHECK_VALUE_READONLY(authProfile);
    CHECK_VALUE_READONLY(authAudioIn);
    CHECK_VALUE_READONLY(authAudioOut);
    CHECK_VALUE_READONLY(authRelay);
    CHECK_VALUE_READONLY(authPTZ);
    CHECK_VALUE_READONLY(onvifUserLevel);
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief check setting change
 @param[in,out] new_setting new setting
 @param[in] old_setting old setting
 @param[in] userindex : user index (starts from 1)
 @param[in] support_audio : audio capability of the model
 */
void AccountManager::check_user_account_setting_change(UserSettingSerializable *new_setting, const UserSettingSerializable *old_setting, int userindex,
        SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    EventType event = EV_USER_SETTING_CHANGED;
    int data = 0;

    IPC_ERROR_CODE errcode;

    if (strlen(new_setting->password) == 0)
    {
        memcpy(new_setting->password, old_setting->password, MAX_USER_PASSWORD_LEN);
    }

    CHECK_BOOLEAN_LOG(isEnable, USER_SETTING_CHANGE_LOG, checker->logname("User %d Enable", userindex));

    if (new_setting->isEnable)
    {
        if (strcmp(new_setting->id, new_setting->password) == 0)
        {
            checker->check_validity(false, errcode);
        }
        CHECK_STRING_FUNCTION_LOG_ERRCODE(id, 16, is_valid_user_id, errcode, USER_SETTING_CHANGE_LOG,
                checker->logname("User %d Name", userindex), &errcode);
        CHECK_PASSWORD_FUNCTION_LOG_ERRCODE(password, MAX_USER_PASSWORD_LEN, is_valid_user_password, errcode,
                USER_SETTING_CHANGE_LOG, checker->logname("User %d Password", userindex), &errcode);
    }
    else
    {
        CHECK_STRING_LOG(id, 16, USER_SETTING_CHANGE_LOG, checker->logname("User %d Name", userindex));
        CHECK_PASSWORD_LOG(password, MAX_USER_PASSWORD_LEN, USER_SETTING_CHANGE_LOG,
                checker->logname("User %d Password", userindex));
    }

    CHECK_VALUE_READONLY(level);
    CHECK_BOOLEAN_LOG(authProfile, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Profile)", userindex));
    if (UtilityWrapper::get_modelInfo(MODELINFO_SUPPORT_AUDIO))
    {
        CHECK_BOOLEAN_LOG(authAudioIn, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Audio-In)", userindex));
        CHECK_BOOLEAN_LOG(authAudioOut, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Audio-Out)", userindex));
    }
    else
    {
        CHECK_VALUE_READONLY(authAudioIn);
        CHECK_VALUE_READONLY(authAudioOut);
    }
    CHECK_BOOLEAN_LOG(authRelay, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Alarm out)", userindex));
    CHECK_BOOLEAN_LOG(authPTZ, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (PTZ)", userindex));

    CHECK_VALUE_RANGE_NOEVENT(onvifUserLevel, 0, 3);
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief check setting change
 @param[in,out] new_setting new setting
 @param[in] old_setting old setting
 */
void AccountManager::check_guest_account_setting_change(UserSettingSerializable *new_setting, const UserSettingSerializable *old_setting,
        SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    EventType event = EV_USER_SETTING_CHANGED;
    int data = 0;

    CHECK_BOOLEAN_LOG(isEnable, USER_SETTING_CHANGE_LOG, "Guest Access");
    CHECK_STRING_READONLY(id, 16);
    CHECK_STRING_READONLY(password, MAX_USER_PASSWORD_LEN);
    CHECK_VALUE_RANGE_LOG(level, 3, 4, USER_SETTING_CHANGE_LOG, "Guest Authentication");
    CHECK_BOOLEAN_LOG(authProfile, USER_SETTING_CHANGE_LOG, "Guest Authority (Profile)");
    if (UtilityWrapper::get_modelInfo(MODELINFO_SUPPORT_AUDIO))
    {
        CHECK_BOOLEAN_LOG(authAudioIn, USER_SETTING_CHANGE_LOG, "Guest Authority (Audio-In)");
        CHECK_BOOLEAN_LOG(authAudioOut, USER_SETTING_CHANGE_LOG, "Guest Authority (Audio-Out)");
    }
    else
    {
        CHECK_VALUE_READONLY(authAudioIn);
        CHECK_VALUE_READONLY(authAudioOut);
    }
    CHECK_BOOLEAN_LOG(authRelay, USER_SETTING_CHANGE_LOG, "Guest Authority (Alarm out)");
    CHECK_BOOLEAN_LOG(authPTZ, USER_SETTING_CHANGE_LOG, "Guest Authority (PTZ)");
    CHECK_VALUE_READONLY(onvifUserLevel);
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief check if there is any duplicated user id
 generate APP_ERR_USERNAME_CRASH if a collison is detected
 @param[in] new_setting new setting to check
 */
void AccountManager::check_user_name_collison(AccountSettingSerializable *new_setting)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        if (new_setting->user[i].isEnable == 0)
        {
            continue;
        }
        for (int j = i + 1; j < MAX_USER_COUNT; j++)
        {
            if (new_setting->user[j].isEnable == 1 && strcmp(new_setting->user[i].id, new_setting->user[j].id) == 0)
            {
                throw InvalidSettingException(APP_ERR_USERNAME_CRASH);
            }
        }
    }
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

#endif

/**
 @brief check setting change
 @param[in,out] new_setting new setting
 @param[in] old_setting old setting
 */
//void AccountManager::check_admin_account_setting_change(UserSetting *new_setting, const UserSetting *old_setting,
//        SettingChecker* checker)
//{
//    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//    EventType event = EV_USER_SETTING_CHANGED;
//    int data = 0;
//
//    IPC_ERROR_CODE errcode = APP_ERR_FAILED;
//
//    CHECK_VALUE_READONLY(isEnable);
//    CHECK_STRING_READONLY(id, 16);
//
//    if (strlen(new_setting->password) == 0)
//    {
//        memcpy(new_setting->password, old_setting->password, MAX_USER_PASSWORD_LEN);
//    }
//    bool prev_changed = checker->reset_changed();
//    CHECK_PASSWORD_LOG(password, MAX_USER_PASSWORD_LEN, USER_SETTING_CHANGE_LOG, "Administrator Password");
//
//    if (checker->restore_changed(prev_changed))	// admin password changed
//    {
//        if (strcmp(new_setting->id, new_setting->password) == 0)
//        {
//            checker->check_validity(false, errcode);
//        }
//        checker->check_validity(is_valid_user_password(new_setting->password, &errcode), errcode);
//        if (strncmp(old_setting->password, "4321", 4) == 0)
//        {
//            checker->add_change(EV_ADMIN_PASSWORD_CHANGED, 0);
//        }
//    }
//
//    CHECK_VALUE_READONLY(level);
//    CHECK_VALUE_READONLY(authProfile);
//    CHECK_VALUE_READONLY(authAudioIn);
//    CHECK_VALUE_READONLY(authAudioOut);
//    CHECK_VALUE_READONLY(authRelay);
//    CHECK_VALUE_READONLY(authPTZ);
//    CHECK_VALUE_READONLY(onvifUserLevel);
//    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//}

/**
 @brief check setting change
 @param[in,out] new_setting new setting
 @param[in] old_setting old setting
 @param[in] userindex : user index (starts from 1)
 @param[in] support_audio : audio capability of the model
 */
//void AccountManager::check_user_account_setting_change(UserSetting *new_setting, const UserSetting *old_setting, int userindex,
//        SettingChecker* checker)
//{
//    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//    EventType event = EV_USER_SETTING_CHANGED;
//    int data = 0;
//
//    IPC_ERROR_CODE errcode;
//
//    if (strlen(new_setting->password) == 0)
//    {
//        memcpy(new_setting->password, old_setting->password, MAX_USER_PASSWORD_LEN);
//    }
//
//    CHECK_BOOLEAN_LOG(isEnable, USER_SETTING_CHANGE_LOG, checker->logname("User %d Enable", userindex));
//
//    if (new_setting->isEnable)
//    {
//        if (strcmp(new_setting->id, new_setting->password) == 0)
//        {
//            checker->check_validity(false, errcode);
//        }
//        CHECK_STRING_FUNCTION_LOG_ERRCODE(id, 16, is_valid_user_id, errcode, USER_SETTING_CHANGE_LOG,
//                checker->logname("User %d Name", userindex), &errcode);
//        CHECK_PASSWORD_FUNCTION_LOG_ERRCODE(password, MAX_USER_PASSWORD_LEN, is_valid_user_password, errcode,
//                USER_SETTING_CHANGE_LOG, checker->logname("User %d Password", userindex), &errcode);
//    }
//    else
//    {
//        CHECK_STRING_LOG(id, 16, USER_SETTING_CHANGE_LOG, checker->logname("User %d Name", userindex));
//        CHECK_PASSWORD_LOG(password, MAX_USER_PASSWORD_LEN, USER_SETTING_CHANGE_LOG,
//                checker->logname("User %d Password", userindex));
//    }
//
//    CHECK_VALUE_READONLY(level);
//    CHECK_BOOLEAN_LOG(authProfile, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Profile)", userindex));
//    if (UtilityWrapper::get_modelInfo(MODELINFO_SUPPORT_AUDIO))
//    {
//        CHECK_BOOLEAN_LOG(authAudioIn, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Audio-In)", userindex));
//        CHECK_BOOLEAN_LOG(authAudioOut, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Audio-Out)", userindex));
//    }
//    else
//    {
//        CHECK_VALUE_READONLY(authAudioIn);
//        CHECK_VALUE_READONLY(authAudioOut);
//    }
//    CHECK_BOOLEAN_LOG(authRelay, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (Alarm out)", userindex));
//    CHECK_BOOLEAN_LOG(authPTZ, USER_SETTING_CHANGE_LOG, checker->logname("User %d Authority (PTZ)", userindex));
//
//    CHECK_VALUE_RANGE_NOEVENT(onvifUserLevel, 0, 3);
//    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//}

/**
 @brief check setting change
 @param[in,out] new_setting new setting
 @param[in] old_setting old setting
 */
//void AccountManager::check_guest_account_setting_change(UserSetting *new_setting, const UserSetting *old_setting,
//        SettingChecker* checker)
//{
//    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//    EventType event = EV_USER_SETTING_CHANGED;
//    int data = 0;
//
//    CHECK_BOOLEAN_LOG(isEnable, USER_SETTING_CHANGE_LOG, "Guest Access");
//    CHECK_STRING_READONLY(id, 16);
//    CHECK_STRING_READONLY(password, MAX_USER_PASSWORD_LEN);
//    CHECK_VALUE_RANGE_LOG(level, 3, 4, USER_SETTING_CHANGE_LOG, "Guest Authentication");
//    CHECK_BOOLEAN_LOG(authProfile, USER_SETTING_CHANGE_LOG, "Guest Authority (Profile)");
//    if (UtilityWrapper::get_modelInfo(MODELINFO_SUPPORT_AUDIO))
//    {
//        CHECK_BOOLEAN_LOG(authAudioIn, USER_SETTING_CHANGE_LOG, "Guest Authority (Audio-In)");
//        CHECK_BOOLEAN_LOG(authAudioOut, USER_SETTING_CHANGE_LOG, "Guest Authority (Audio-Out)");
//    }
//    else
//    {
//        CHECK_VALUE_READONLY(authAudioIn);
//        CHECK_VALUE_READONLY(authAudioOut);
//    }
//    CHECK_BOOLEAN_LOG(authRelay, USER_SETTING_CHANGE_LOG, "Guest Authority (Alarm out)");
//    CHECK_BOOLEAN_LOG(authPTZ, USER_SETTING_CHANGE_LOG, "Guest Authority (PTZ)");
//    CHECK_VALUE_READONLY(onvifUserLevel);
//    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//}

/**
 @brief get enableOnvifAuth value in shared_ptr type
 @return enableOnvifAuth
 */
Utility::SmartPointerUtils::shared_ptr<const char> AccountManager::get_enableOnvifAuth(void)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return enableOnvifAuth;
}

/**
 @brief set enableOnvifAuth value. this function should be followed by apply_settingChanges() or discard_settingChanges()
 @param enable - new value
 @param checker - SettingChecker
 */
void AccountManager::set_enableOnvifAuth(Utility::SmartPointerUtils::shared_ptr<char>& enable, SettingChecker* checker)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    //CHECK_BOOLEAN
    checker->check_value_change(*enable, *enableOnvifAuth, true, 0, 1, EV_ONVIF_SETTING_CHANGED, 0, NO_LOG, "enableOnvifAuth",
            NULL, APP_ERR_FAILED, __func__);
    enableOnvifAuthCandidate = enable;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief apply setting candidates from set_setting functions
 */
void AccountManager::apply_settingChanges(void)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    bool need_write = false;
    if (accountCandidate)
    {
        account = accountCandidate;
        accountCandidate.reset();
        need_write = true;
    }
    if (enableOnvifAuthCandidate)
    {
        enableOnvifAuth = enableOnvifAuthCandidate;
        enableOnvifAuthCandidate.reset();
        need_write = true;
    }

    if (need_write)
    {
        write_setting();
    }
#if USE_NEW
    need_write = false;
    if (accountCandidateSerializable)
    {
        account_serializable = accountCandidateSerializable;
        accountCandidateSerializable.reset();
        need_write = true;
    }
    if (enableOnvifAuthCandidate)
    {
        enableOnvifAuth = enableOnvifAuthCandidate;
        enableOnvifAuthCandidate.reset();
        need_write = true;
    }
    if(need_write)
    {
        this->write_setting();
    }
#endif
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief discard setting candidates from set_setting functions
 */
void AccountManager::discard_settingChanges(void)
{
#if !USE_NEW
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    accountCandidate.reset();
    enableOnvifAuthCandidate.reset();
#else
    account_serializable.reset();
    accountCandidateSerializable.reset();
#endif
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
}

/**
 @brief check user password validity for non-S1 models
 @param[in] password : password to check
 @param[out] errcode : the reason of false return
 @return validity
 */
bool AccountManager::is_valid_user_password(const char* password, IPC_ERROR_CODE* errcode)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    int password_length = strlen(password);
    if(!check_valid_password(password))
    {
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return false;
    }
    int type_count = count_characterType(password);

    if (check_passwordComplexity(errcode, password_length, type_count) == false)
    {
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return false;
    }

    const int sequence_len = 4;
    for (int i = 0; i < password_length - sequence_len; i++)
    {
        if (is_weakSequence(&password[i], sequence_len, true))
        {
            *errcode = APP_ERR_PASSWORD_TOO_WEAK;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
            return false;
        }
    }
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return true;
}

/**
 @brief count how many types(lower/upper/number/special) of characters are included in the string
 @param str - target string
 @return count of character types
 */
int AccountManager::count_characterType(const char* str)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    int hasUpper = 0, hasLower = 0, hasNumber = 0, hasSpecial = 0;
    const char *c = str;

    while (*c != '\0')
    {
        if (islower(*c))
        {
            hasLower = 1;
        }
        else if (isupper(*c))
        {
            hasUpper = 1;
        }
        else if (isdigit(*c))
        {
            hasNumber = 1;
        }
        else
        {
            hasSpecial = 1;
        }
        c++;
    }
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return hasUpper + hasLower + hasNumber + hasSpecial;
}

/**
 @brief check if the password complexity is ok or not
 @param[out] errcode - when this function returns 0, \a errcode will contain the reason
 @param password_length - length of password
 @param type_count - count of character types in the password (return value of count_characterType())
 @return count of character types
 */
bool AccountManager::check_passwordComplexity(IPC_ERROR_CODE* errcode, int password_length, int type_count)
{
    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    if (password_length < 8)
    {
        *errcode = APP_ERR_PASSWORD_TOO_WEAK;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return false;
    }
    else if (password_length < 10)
    {
        if (type_count < 3)
        {
            *errcode = APP_ERR_PASSWORD_TOO_WEAK;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
            return false;
        }
    }
    else if (password_length <= 15)
    {
        if (type_count < 2)
        {
            *errcode = APP_ERR_PASSWORD_TOO_WEAK;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
            return false;
        }
    }
    else
    {
        *errcode = APP_ERR_PASSWORD_TOO_LONG;
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
        return false;
    }
    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
    return true;
}

/**
@brief check valid_password_type
@param password - string to check password
@return if the string is a valid password return true, else return false
*/
bool AccountManager::check_valid_password(const char* password)
{
    const char *c = password;

    while (*c != '\0')
    {
        if ((islower(*c)) || (isupper(*c)) || (isdigit(*c)) || *c == '~' || *c == '`' ||
        *c == '!' || *c == '@' || *c == '#' || *c == '$' || *c == '%' || *c == '^' || *c == '*' || *c == '(' || *c == ')' || *c == '_' || *c == '-' || 
        *c == '+' || *c == '=' || *c == '|' || *c == '{' || *c == '}' || *c == '[' || *c == ']' || *c == '.' || *c == '?' || *c == '/')
        {
            c++;
        }
        else
        {
            return false;
        }
    }
    return true;
}

/**
@brief check if the given string is a weak sequence, which means repeated or consecutive string sequence (aaa, abc, cba, etc.)
@param seq - string to check
@param len - length of \a seq
@param ignore_type - if false, check only alpha and numeric sequences. (ignore special characters) this option is for s1 models
@return if the string is a weak sequence return true, else return false
*/
bool AccountManager::is_weakSequence(const char* seq, int len, bool ignore_type)
{
    int diff = seq[1] - seq[0];
    if (diff != 0 && diff != -1 && diff != 1)
    {
        return false;
    }

    for (int i = 2; i < len; i++)
    {
        if (seq[i] - seq[i - 1] != diff)
        {
            return false;
        }
    }

    if (ignore_type == false)
    {
        for (int i = 0; i < len; i++)
        {
            if (isalnum(seq[i]) == 0)
            {
                return false;
            }
        }
    }

    return true;
}

/**
 @brief check if a user id is valid or not, and give error code if the id is not valid
 valid id : at most 15 alphanumeric characters
 @param[in] id : user id to check
 @param[out] errcode : the reason of false return
 @return id validity
 */
bool AccountManager::is_valid_user_id(const char* id, IPC_ERROR_CODE* errcode)
{
    int idlen = strlen(id);
    if (idlen == 0)
    {
        *errcode = APP_ERR_FAILED;
        return false;
    }
    else if (idlen > 15)
    {
        *errcode = APP_ERR_USERNAME_TOO_LONG;
        return false;
    }
    else if (strcmp(id, "admin") == 0 || strcmp(id, "guest") == 0)
    {
        *errcode = APP_ERR_USERNAME_CRASH;
        return false;
    }

    const char *c = id;
    while (*c != '\0')
    {
        if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9'))
        {
            c++;
        }
        else
        {
            return false;
        }
    }
    return true;
}

/**
 @brief check if there is any duplicated user id
 generate APP_ERR_USERNAME_CRASH if a collison is detected
 @param[in] new_setting new setting to check
 */
//void AccountManager::check_user_name_collison(AccountSetting *new_setting)
//{
//    //std::cout << "==>" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//    for (int i = 0; i < MAX_USER_COUNT; i++)
//    {
//        if (new_setting->user[i].isEnable == 0)
//        {
//            continue;
//        }
//        for (int j = i + 1; j < MAX_USER_COUNT; j++)
//        {
//            if (new_setting->user[j].isEnable == 1 && strcmp(new_setting->user[i].id, new_setting->user[j].id) == 0)
//            {
//                throw InvalidSettingException(APP_ERR_USERNAME_CRASH);
//            }
//        }
//    }
//    //std::cout << "<--" <<  __PRETTY_FUNCTION__ << "@" << __LINE__ << std::endl;
//}

void AccountManager::read()
{
    this->read_value(*(const_cast<AccountSettingSerializable*>(this->account_serializable.get())), "AccountSetting");
    this->read_value(*(const_cast<char*>(this->enableOnvifAuth.get())), "enableOnvifAuth");
}
void AccountManager::write()
{
    this->set_write(*(const_cast<AccountSettingSerializable*>(this->account_serializable.get())), "AccountSetting");
    this->set_write(*(const_cast<char*>(this->enableOnvifAuth.get())), "enableOnvifAuth");
}

void AccountManager::UserSettingSerializable::read()
{
    this->read_value(isEnable, "isEnable");
    this->read_value(id, sizeof(id), "id");
    this->read_value(password, sizeof(password), "password");
    this->read_value(level, "level");
    this->read_value(authAudioIn, "authAudioIn");
    this->read_value(authAudioOut, "authAudioOut");
    this->read_value(authRelay, "authRelay");
    this->read_value(authPTZ, "authPTZ");
    this->read_value(authProfile, "authProfile");
    this->read_value(onvifUserLevel, "onvifUserLevel");
}

void AccountManager::UserSettingSerializable::write()
{
    this->set_write(level, "level");
    this->set_write(authAudioIn, "authAudioIn");
    this->set_write(authAudioOut, "authAudioOut");
    this->set_write(authRelay, "authRelay");
    this->set_write(authPTZ, "authPTZ");
    this->set_write(authProfile, "authProfile");
    this->set_write(onvifUserLevel, "onvifUserLevel");
    this->set_write(isEnable, "isEnable");
    this->set_write(id, sizeof(id), "id");
    this->set_write(password, sizeof(password), "password");
}

void AccountManager::AccountSettingSerializable::read()
{
    this->read_value(this->admin, "admin");
    this->read_value(this->user, sizeof(user) / sizeof(UserSettingSerializable), "user");
    this->read_value(this->guest, "guest");
}

void AccountManager::AccountSettingSerializable::write()
{
    this->set_write(this->admin, "admin");
    this->set_write(this->user, sizeof(user) / sizeof(UserSettingSerializable), "user");
    this->set_write(this->guest, "guest");
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

    if (UtilityWrapper::get_modelInfo(MODELINFO_SUPPORT_AUDIO) == 0)
    {
        restore_data->account.admin.authAudioIn = 0;
        restore_data->account.admin.authAudioOut = 0;
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            restore_data->account.user[i].authAudioIn = 0;
            restore_data->account.user[i].authAudioOut = 0;
        }
    }
    else
    {
        restore_data->account.admin.authAudioIn = 1;
        restore_data->account.admin.authAudioOut = 1;
    }

    storage->write_settingfile(restore_filename, restore_data, setting_size);
    UtilityWrapper::free(restore_data);
#endif

