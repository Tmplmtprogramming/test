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
@file  account_manager.h
@brief AccountManager class declaration
*/
#ifndef _ACCOUNT_MANAGER_H_
#define _ACCOUNT_MANAGER_H_
#include "framework/component/event_target.h"
#include "setting/setting_common/setting_struct.h"
#include "setting/setting_common/setting_define.h"
#include "utility/utility_common/shared_ptr.h"
#include "setting/parser/serializable.h"

class TaskEvent;
class SettingChecker;
class AccountLock;

#define ACCOUNTMANAGER_SETTING_FILE "/mnt/setting/account.cfg"

#define USE_NEW 1

/**
@enum ADMIN_PASSWORD_STATUS
@brief return value of AccountManager::get_admin_password_status()
*/
typedef enum ADMIN_PASSWORD_STATUS
{
    VALID_ADMIN_PASSWORD = 0, /**< current password is ok */
    DEFAULT_ADMIN_PASSWORD = 1, /**< current password is default(4321) */
    WEAK_ADMIN_PASSWORD = 2, /**< current password is not default, but it's not strong enough */
} ADMIN_PASSWORD_STATUS;

//struct AccountSettingSerializable;
//struct UserSettingSerializable;

/**
@class AccountManager
@brief manage user account list and accountlock
*/
class AccountManager : public EventTarget, public Serializable
{
public:
    struct UserSettingSerializable : public Serializable
    {
        char	isEnable;
        char	id[MAX_USER_NAME_LEN];
        char	password[MAX_USER_PASSWORD_LEN];
        int		level;
        char	authAudioIn;
        char	authAudioOut;
        char	authRelay;
        char	authPTZ;
        char	authProfile;		// 0:Default, 1:All
        char    onvifUserLevel;     // 0: admin, 1: Operator, 2:MediaUser, 3:Anonymous

        UserSettingSerializable()
        {
            reset();
        }
        // get Serializable data from Structures
        virtual void read();
        // create Structures from Serializable data
        virtual void write();
        // Resetting Structures from Serializable data
        virtual void reset()
        {
            isEnable = 0;
            memset(id, 0x00, sizeof(id));
            memset(password, 0x00, sizeof(password));
            level = 0;
            authAudioIn = 0;
            authAudioOut = 0;
            authRelay = 0;
            authPTZ = 0;
            authProfile = 0;
            onvifUserLevel = 0;
        }
        void print()
        {
            std::cout << "authAudioIn : " << std::hex << static_cast<int>(this->authAudioIn) << std::endl;
            std::cout << "authAudioOut : " << std::hex << static_cast<int>(this->authAudioOut) << std::endl;
            std::cout << "authPTZ : " << std::hex << static_cast<int>(this->authPTZ) << std::endl;
            std::cout << "authProfile : " << std::hex << static_cast<int>(this->authProfile) << std::endl;
            std::cout << "authRelay : " << std::hex << static_cast<int>(this->authRelay) << std::endl;
            std::cout << "id : " << this->id << std::endl;
            std::cout << "isEnable : " << std::hex << static_cast<int>(this->isEnable) << std::endl;
            std::cout << "level : " << this->level << std::endl;
            std::cout << "onvifUserLevel : " << std::hex << static_cast<int>(this->onvifUserLevel) << std::endl;
            std::cout << "password : " << this->password << std::endl;
        }
        UserSettingSerializable& operator=(const UserSetting& rhs)
        {
            this->authAudioIn = rhs.authAudioIn;
            this->authAudioOut= rhs.authAudioOut;
            this->authPTZ = rhs.authPTZ;
            this->authProfile = rhs.authProfile;
            this->authRelay = rhs.authRelay;
            strlcpy(this->id, rhs.id, sizeof(rhs.id));
            this->isEnable = rhs.isEnable;
            this->level = rhs.level;
            this->onvifUserLevel = rhs.onvifUserLevel;
            strlcpy(this->password, rhs.password, sizeof(rhs.password));
            return *this;
        }
        void convert_toRawSetting(UserSetting& rhs)
        {
            rhs.authAudioIn = this->authAudioIn;
            rhs.authAudioOut= this->authAudioOut;
            rhs.authPTZ= this->authPTZ;
            rhs.authProfile = this->authProfile;
            rhs.authRelay = this->authRelay;
            rhs.isEnable = this->isEnable;
            rhs.level = this->level;
            rhs.onvifUserLevel = this->onvifUserLevel;
//            rhs.admin.id = this->admin.id;
//            rhs.admin.password = this->admin.password;
            strlcpy(rhs.id, this->id, sizeof(rhs.id));
            strlcpy(rhs.password, this->password, sizeof(rhs.password));
        }
        virtual void convert_toBitStream(void* structure_pointer)
        {
            UserSetting* raw_setting = static_cast<UserSetting*>(structure_pointer);
            this->convert_toRawSetting(*raw_setting);
        }
        virtual void convert_fromBitStream(const void* structure_pointer)
        {
            const UserSetting* raw_setting = static_cast<const UserSetting*>(structure_pointer);
            *this = *raw_setting;
        }
    };
    struct AccountSettingSerializable : public Serializable
    {
        AccountManager::UserSettingSerializable	admin;
        AccountManager::UserSettingSerializable	user[MAX_USER_COUNT];
        AccountManager::UserSettingSerializable	guest;

        // get Serializable data from Structures
        virtual void read();
        // create Structures from Serializable data
        virtual void write();
        // Resetting Structures from Serializable data
        virtual void reset()
        {
            admin.reset();
            for(int i = 0; i < MAX_USER_COUNT; i++)
            {
                user[i].reset();
            }
            guest.reset();
        }
        AccountSettingSerializable& operator=(const AccountSetting& rhs)
        {
            this->admin = rhs.admin;
            for(int i = 0; i < MAX_USER_COUNT; i++)
            {
                this->user[i] = rhs.user[i];
            }
            this->guest = rhs.guest;
            return *this;
        }
        void convert_toBitStream(AccountSetting& rhs)
        {
            this->admin.convert_toBitStream(&rhs.admin);
            this->guest.convert_toBitStream(&rhs.guest);
            for(int i = 0; i < MAX_USER_COUNT; i++)
            {
                this->user[i].convert_toBitStream(&rhs.user[i]);
            }
            return;
        }
        void convert_fromBitStream(const AccountSetting& rhs)
        {
            *this = rhs;
        }
        virtual void convert_toBitStream(void* structure_pointer)
        {
            AccountSetting* raw_setting = static_cast<AccountSetting*>(structure_pointer);
            this->convert_toBitStream(*raw_setting);
        }
        virtual void convert_fromBitStream(const void* structure_pointer)
        {
            const AccountSetting* raw_setting = static_cast<const AccountSetting*>(structure_pointer);
            this->convert_fromBitStream(*raw_setting);
        }
    };
//    struct OnvifAuthSerializable: public Serializable
//    {
//        char enableOnvifAuth;
//        virtual void read()
//        {
//            this->read_value(this->enableOnvifAuth, "enableOnvifAuth");
//        }
//        virtual void write()
//        {
//            this->set_write(this->enableOnvifAuth, "enableOnvifAuth");
//        }
//        virtual void reset()
//        {
//            this->enableOnvifAuth = '\0';
//        }
//        virtual void convert_toBitStream(void* structure_pointer)
//        {
//            char* enable = static_cast<char*>(structure_pointer);
//            *enable = this->enableOnvifAuth;
//        }
//        virtual void convert_fromBitStream(const void* structure_pointer)
//        {
//            const char* enable = static_cast<const char*>(structure_pointer);
//            this->enableOnvifAuth = *enable;
//        }
//    };
public:
    static AccountManager* createnew(void);
    virtual ~AccountManager(void);
    virtual bool initialize(void);

    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> get_accountSetting(void);
    void set_accountSetting(Utility::SmartPointerUtils::shared_ptr<AccountSetting>& account, SettingChecker* checker);
#if USE_NEW
    Utility::SmartPointerUtils::shared_ptr<const AccountSettingSerializable> get_accountSettingSerializable(void);
    void set_accountSettingSerializable(Utility::SmartPointerUtils::shared_ptr<AccountSettingSerializable>& account, SettingChecker* checker);
#endif
    Utility::SmartPointerUtils::shared_ptr<const char> get_enableOnvifAuth(void);
    void set_enableOnvifAuth(Utility::SmartPointerUtils::shared_ptr<char>& enable, SettingChecker* checker);

    virtual void apply_settingChanges(void);
    virtual void discard_settingChanges(void);
    virtual void reset_setting(RESET_SETTING_MODE mode);
    virtual int write_setting(void);
    virtual void modify_configRestoreData(const char* restore_data_path);

    int get_userSetting(UserSetting* usersetting, const char *id);
    virtual bool is_anonymous_allowed(bool isOnvifAccess);
    virtual ADMIN_PASSWORD_STATUS get_adminPasswordStatus(void);

    virtual void read();
    virtual void write();
    virtual void convert_toBitStream(void* structure_pointer){}
    virtual void convert_fromBitStream(const void* structure_pointer){}

protected:
    AccountManager(void);

    virtual bool process_event(TaskEvent* event);

    virtual void update_accountlock_list(void);
    void check_accountLockStatus(void);
    virtual void check_failStatus(AccountLock* locker);
    void check_blockStatus(AccountLock* locker);

    virtual void initialize_setting(void);

#if !USE_NEW
    void check_accountSetting(AccountSetting* new_setting, const AccountSetting* old_setting, SettingChecker* checker);
    void check_admin_account_setting_change(UserSetting *new_setting, const UserSetting *old_setting, SettingChecker* checker);
    void check_user_account_setting_change(UserSetting *new_setting, const UserSetting *old_setting, int userindex, SettingChecker* checker);
    void check_guest_account_setting_change(UserSetting *new_setting, const UserSetting *old_setting, SettingChecker* checker);
    void check_user_name_collison(AccountSetting *new_setting);
#else
    void check_accountSetting(AccountSettingSerializable* new_setting, const AccountSettingSerializable* old_setting, SettingChecker* checker);
    void check_admin_account_setting_change(UserSettingSerializable *new_setting, const UserSettingSerializable *old_setting, SettingChecker* checker);
    void check_user_account_setting_change(UserSettingSerializable *new_setting, const UserSettingSerializable *old_setting, int userindex, SettingChecker* checker);
    void check_guest_account_setting_change(UserSettingSerializable *new_setting, const UserSettingSerializable *old_setting, SettingChecker* checker);
    void check_user_name_collison(AccountSettingSerializable *new_setting);
#endif

    bool is_valid_user_id(const char *id, IPC_ERROR_CODE* errcode);
    virtual bool check_valid_password(const char *password);
    virtual bool is_valid_user_password(const char *password, IPC_ERROR_CODE* errcode);
    virtual int count_characterType(const char* str);
    virtual bool check_passwordComplexity(IPC_ERROR_CODE* errcode, int password_length, int type_count);
    bool is_weakSequence(const char* seq, int len, bool ignore_type);

protected:
    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> account;
    Utility::SmartPointerUtils::shared_ptr<const char> enableOnvifAuth;
    Utility::SmartPointerUtils::shared_ptr<const AccountSetting> accountCandidate;
    Utility::SmartPointerUtils::shared_ptr<const char> enableOnvifAuthCandidate;

    bool accountLockStatus;

//    AccountSettingSerializable account_setting_serializable;
#if USE_NEW
    Utility::SmartPointerUtils::shared_ptr<AccountSettingSerializable> account_serializable;
    Utility::SmartPointerUtils::shared_ptr<AccountSettingSerializable> accountCandidateSerializable;
    Utility::SmartPointerUtils::shared_ptr<const char> enableOnvifAuthSerializable;
    Utility::SmartPointerUtils::shared_ptr<const char> enableOnvifAuthSerializableCandidate;
#endif
};

#endif


