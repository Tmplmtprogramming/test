/******************************************************************************
* Copyright(c) 2012 by Samsung Techwin, Inc.
*
* This software is copyrighted by, and is the sole property of Samsung Techwin.
*
* Samsung Techwin, Inc.
* KOREA
* http://www.samsungtechwin.co.kr
*****************************************************************************/
/**
* @file : accountlock.h
* @brief : This file will provide the template for C program coding
*        standard.
*
* @author : yj8843.cho <.@samsung.com>
* @date : 2014.11.07
* @version : 1.0
*/
#include <errno.h>
#include <sys/time.h>
#include "network_server/network_server_common/account_lock.h"

#include "utility/utility_common/utility_wrapper.h"
/**
@fn accountlock accountlock::create
@brief :
@param  :
@return  :
@see
*
*/
bool
AccountLock::create(int enable, int max_failure_count, int lock_time_ms)
{
    bool result = true;
    if (sema_create(PW_KEY_PATH, PW_KEY_PROJ_ID) == RET_ERROR)
    {
        result = false;
    }

    if (shm_create(PW_KEY_PATH, PW_KEY_PROJ_ID, sizeof(AccountPWLockInfo)) == RET_ERROR)
    {
        result = false;
    }

    if (result)
    {
        IPC_SHM shm;
        shm_init(&shm);
        if (shm_connect(&shm, PW_KEY_PATH, PW_KEY_PROJ_ID, sizeof(AccountPWLockInfo)))
        {
            AccountPWLockInfo *lockInfo = (AccountPWLockInfo *)shm.shmaddr;
            memset(lockInfo, 0x00, sizeof(AccountPWLockInfo));
            lockInfo->enable = enable;
            lockInfo->max_failure_count = max_failure_count;
            lockInfo->lock_time_ms = lock_time_ms;
            shm_disconnect(&shm);
        }
    }
    return result;
}
/**
@fn accountlock accountlock::destroy
@brief :
@param  :
@return  :
@see
*
*/
bool
AccountLock::destroy()
{
    return sema_remove(PW_KEY_PATH, PW_KEY_PROJ_ID);
}
/**
@fn accountlock accountlock::ACCOUNTLOCK
@brief :
@param  :
@return  :
@see
*
*/
AccountLock::AccountLock(int access_protocol, const char *access_ip)
    : lockInfo(NULL), protocol(access_protocol)
{
    memset(&sem, 0x00, sizeof(IPC_SEM));
    memset(&shm, 0x00, sizeof(IPC_SHM));
    sema_init(&sem);
    shm_init(&shm);
    strlcpy(cli_ip, access_ip, IP_STR_LEN);
}
/**
@fn accountlock accountlock::~ACCOUNTLOCK
@brief :
@param  :
@return  :
@see
*
*/
AccountLock::~AccountLock(void)
{
}
/**
@fn accountlock accountlock::update_policy
@brief :
@param  :
@return  :
@see
*
*/
void
AccountLock::update_policy(int enable, int max_failure_count, int lock_time_ms)
{
    if (connect())
    {
        lockInfo->enable = enable;
        lockInfo->max_failure_count = max_failure_count;
        lockInfo->lock_time_ms = lock_time_ms;
        disconnect();
    }
    else
    {
        DP_ERR("accountlock policy update failed");
    }
}
/**
@fn accountlock accountlock::update_userlist
@brief :
@param  :
@return  :
@see
*
*/
void
AccountLock::update_userlist(const char *id[], int count)
{
    int i, j;
    int existing_id[count];

    memset(existing_id, 0, sizeof(existing_id));

    if (connect())
    {
        for (i = 0; i < USER_INFO_COUNT; i++)
        {
            if (lockInfo->user[i].id[0])
            {
                for (j = 0; j < count; j++)
                {
                    if (existing_id[j] == 0 && strcmp(lockInfo->user[i].id, id[j]) == 0)
                    {
                        existing_id[j] = 1;
                        break;
                    }
                }
                if (j == count)
                {
                    DP_LOG("remove %s from accountlock user list", lockInfo->user[i].id);
                    memset(&lockInfo->user[i], 0, sizeof(LockInfo));
                }
            }
        }
        for (i = 0; i < count; i++)
        {
            if (existing_id[i] == 0)
            {
                for (j = 0; j < USER_INFO_COUNT; j++)
                {
                    if (lockInfo->user[j].id[0] == 0)
                    {
                        break;
                    }
                }
                if (j == USER_INFO_COUNT)
                {
                    DP_ERR("add user to accountlock user failed");
                    for (j = 0; j < USER_INFO_COUNT; j++)
                    {
                        DP_ERR("%s:%d:%lld", lockInfo->user[j].id, lockInfo->user[j].failure_count, lockInfo->user[j].last_timestamp);
                    }
                }
                else
                {
                    DP_LOG("add %s to accountlock user list", id[i]);
                    memset(&lockInfo->user[j], 0, sizeof(LockInfo));
                    strlcpy(lockInfo->user[j].id, id[i], MAX_ID_LEN);
                }
            }
        }
        disconnect();
    }
    else
    {
        DP_ERR("accountlock user update failed");
    }
}
/**
@fn accountlock accountlock::check_auth_result
@brief :
@param  :
@return  :
@see
*
*/
AUTH_RESULT_E
AccountLock::check_auth_result(const char *id, bool granted, int *lock_time_sec)
{
    std::cout << __FUNCTION__ << ": " << id << " " << (int)granted << " " << *lock_time_sec << std::endl;
    AUTH_RESULT_E ret = granted? AUTH_RESULT_SUCCESS : AUTH_RESULT_FAIL;
    LockInfo *info;

    std::cout << "THERE00" << std::endl;
    if (id[0] == '\0')
    {
    std::cout << "THERE01" << std::endl;
        return ret;
    }

    std::cout << "THERE02" << std::endl;
    if (connect())
    {
    std::cout << "THERE03" << std::endl;
        if (lockInfo->enable)
        {
    std::cout << "THERE04" << std::endl;
            info = find(id);
            if ((*lock_time_sec = check_lock(info)) != 0)
            {
    std::cout << "THERE05" << std::endl;
                ret = AUTH_RESULT_BLOCKED;
            }
            else
            {
    std::cout << "THERE06" << std::endl;
                if (granted)
                {
    std::cout << "THERE07" << std::endl;
                    reset_failure_count(info);
                    ret = AUTH_RESULT_SUCCESS;
                }
                else
                {
    std::cout << "THERE08" << std::endl;
                    increase_failure_count(info);
                    if ((*lock_time_sec = check_lock(info)) != 0)
                    {
    std::cout << "THERE09" << std::endl;
                        ret = AUTH_RESULT_BLOCKED;
                    }
                    else
                    {
    std::cout << "THERE10" << std::endl;
                        ret = AUTH_RESULT_FAIL;
                    }
    std::cout << "THERE11" << std::endl;
                }
    std::cout << "THERE12" << std::endl;
            }
    std::cout << "THERE13" << std::endl;
        }
    std::cout << "THERE14" << std::endl;
        disconnect();
    std::cout << "THERE15" << std::endl;
    }
    std::cout << "THERE16" << std::endl;
    if (ret == AUTH_RESULT_BLOCKED)
    {
        DP_ERR("%s blocked %d", id, *lock_time_sec);
    }
    std::cout << "THERE17" << std::endl;
    return ret;
}
/**
@fn accountlock accountlock::connect
@brief :
@param  :
@return  :
@see
*
*/
bool
AccountLock::connect(void)
{
    if (sema_connect(&sem, PW_KEY_PATH, PW_KEY_PROJ_ID) != RET_SUCCESS ||
            shm_connect(&shm, PW_KEY_PATH, PW_KEY_PROJ_ID, sizeof(AccountPWLockInfo)) != RET_SUCCESS)
    {
        DP_ERR("AccountLock CONNECT ERROR %d(%s)", errno, strerror(errno));
        return false;
    }

    if (sema_lock(&sem) != RET_SUCCESS)
    {
        shm_disconnect(&shm);
        DP_ERR("AccountLock LOCK ERROR %d(%s)", errno, strerror(errno));
        return false;
    }

    lockInfo = (AccountPWLockInfo *)shm.shmaddr;
    return true;
}
/**
@fn accountlock accountlock::disconnect
@brief :
@param  :
@return  :
@see
*
*/
void
AccountLock::disconnect(void)
{
    shm_disconnect(&shm);
    sema_unlock(&sem);
}
/**
@fn accountlock accountlock::reset_failure_count
@brief :
@param  :
@return  :
@see
*
*/
void
AccountLock::reset_failure_count(LockInfo *lock)
{
    if (lock)
    {
        lock->failure_count = 0;
        lock->last_timestamp = get_current_time_millis();
    }
}
/**
@fn accountlock accountlock::increase_failure_count
@brief :
@param  :
@return  :
@see
*
*/
void
AccountLock::increase_failure_count(LockInfo *lock)
{
    if (lock)
    {
        lock->last_timestamp = get_current_time_millis();
        lock->failure_count++;

        if(lock->failure_count < lockInfo->max_failure_count)
        {
            int index = lock->failure_count - 1; // fail_info is "0" base.
            lock->fail_info[index].access_protocol = protocol;
            memcpy(lock->fail_info[index].ip, cli_ip, IP_STR_LEN);
            lock->fail_info[index].time = time(NULL);
        }
        else if (lock->failure_count == lockInfo->max_failure_count)
        {
            lock->ban_info.access_protocol = protocol;
            memcpy(lock->ban_info.ip, cli_ip, IP_STR_LEN);
        }
    }
}
/**
@fn accountlock accountlock::check_lock
@brief :
@param  :
@return  :
@see
*
*/
int
AccountLock::check_lock(LockInfo *lock)
{
    int remaining_ms;
    if (lock)
    {
        if (lock->failure_count >= lockInfo->max_failure_count)
        {
            remaining_ms = lockInfo->lock_time_ms - (get_current_time_millis() - lock->last_timestamp);
            if (remaining_ms > 0)
            {
                DP_ERR("locktime %d, current %lld, last %lld, remain %d",
                       lockInfo->lock_time_ms, get_current_time_millis(), lock->last_timestamp, remaining_ms);
                return (remaining_ms + 999) / 1000;
            }
            else
            {
                lock->failure_count = 0;
                lock->last_timestamp = get_current_time_millis();
            }
        }
    }
    return 0;
}
/**
@fn accountlock accountlock::find
@brief :
@param  :
@return  :
@see
*
*/
LockInfo*
AccountLock::find(const char *id)
{
    int i;
    for (i = 0; i < USER_INFO_COUNT; i++)
    {
        if (strncmp(lockInfo->user[i].id, id, MAX_ID_LEN) == 0)
        {
            DP_LOG("%s found, %d:%lld", lockInfo->user[i].id, lockInfo->user[i].failure_count, lockInfo->user[i].last_timestamp);
            return &lockInfo->user[i];
        }
    }
    DP_ERR("AccountLock FIND %s ERROR!!!!", id);
    return NULL;
}

/**
@brief get information of newly banned users
@param[out] info - output array
@return count of newly banned users
*/
int AccountLock::get_banInfoResult(BanInfoResult info[USER_INFO_COUNT])
{
    int count = 0;
    if (connect())
    {
        for (int i = 0; i < USER_INFO_COUNT; i++)
        {
            if (lockInfo->user[i].ban_info.access_protocol != ACCESS_NONE)
            {
                memcpy(info[count].id, lockInfo->user[i].id, MAX_ID_LEN);
                info[count].access_protocol = lockInfo->user[i].ban_info.access_protocol;
                memcpy(info[count].ip, lockInfo->user[i].ban_info.ip, IP_STR_LEN);
                count++;

                lockInfo->user[i].ban_info.access_protocol = ACCESS_NONE;
                lockInfo->user[i].ban_info.ip[0] = '\0';
            }
        }
        disconnect();
    }
    return count;
}

/**
@brief get information of newly failed users
@param[out] info - output array
@return count of newly failed users
*/
int AccountLock::get_failInfoResult(FailInfoResult info[USER_INFO_COUNT*FAIL_INFO_COUNT])
{
    int count = 0;
    if (connect())
    {
        for (int i = 0; i < USER_INFO_COUNT; i++)
        {
            for(int j = 0; j < lockInfo->user[i].failure_count ; j++)
            {
                if(lockInfo->user[i].fail_info[j].access_protocol != ACCESS_NONE)
                {
                    memcpy(info[count].id, lockInfo->user[i].id, MAX_ID_LEN);
                    memcpy(info[count].ip, lockInfo->user[i].fail_info[j].ip, IP_STR_LEN);
                    info[count].access_protocol = lockInfo->user[i].fail_info[j].access_protocol;
                    info[count].time = lockInfo->user[i].fail_info[j].time;
                    count++;

                    lockInfo->user[i].fail_info[j].access_protocol = ACCESS_NONE;
                    lockInfo->user[i].fail_info[j].ip[0] = '\0';
                    lockInfo->user[i].fail_info[j].time = 0;
                }
            }
        }
        disconnect();
    }
    return count;
}
