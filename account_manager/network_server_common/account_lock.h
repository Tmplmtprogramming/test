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
#ifndef __ACCOUNT_LOCK_H__
#define __ACCOUNT_LOCK_H__

#ifndef _TODO_ADD_C_COMPILE
extern "C" {
#include "ipc/ipcutil.h"
}
#else
#include "ipc/ipcutil.h"
#endif

#define MAX_ID_LEN		25
#define USER_INFO_COUNT	25
#define FAIL_INFO_COUNT	10
#define IP_STR_LEN		40
#define PW_SEM_PATH		"ACCOUNTLOCKSEM"
#define PW_KEY_PATH		"/work/daemon/cfg/lighttpd.conf"	// 키값 생성을 위한 파일경로를 지정
#define PW_KEY_PROJ_ID	6200		// 키값 생성을 위한 아이디

#define ACCESS_NONE				0
#define ACCESS_FROM_HTTP		1
#define ACCESS_FROM_SVNP		2
#define ACCESS_FROM_RTSP		3
#define ACCESS_FROM_IPINSTALLER	4
#define ACCESS_FROM_ONVIF		5

//----------------------------------------------------------------------------
// 사용자별 비밀번호 틀린회수를 저장하기 위한 구조체 정의

/**
@struct
@brief ban info
*/
typedef struct
{
    int access_protocol;
    char ip[IP_STR_LEN];
} BanInfo;

/**
@struct
@brief fail info
*/
typedef struct
{
    int access_protocol;
    char ip[IP_STR_LEN];
    time_t time;
} FailInfo;

/**
@struct : LockInfo
@brief :
*/
typedef struct
{
    char id[MAX_ID_LEN];				// 아이디
    int failure_count;				// 누적 틀린 회수
    long long last_timestamp;	// 최종 변경시간
    BanInfo ban_info;
    FailInfo fail_info[FAIL_INFO_COUNT];
} LockInfo;

/**
@struct BanInfoResult
*/
typedef struct
{
    char id[MAX_ID_LEN];
    int access_protocol;
    char ip[IP_STR_LEN];
} BanInfoResult;
/**
@struct FailInfoResult
*/
typedef struct
{
    char id[MAX_ID_LEN];
    int access_protocol;
    char ip[IP_STR_LEN];
    time_t time;
} FailInfoResult;
/**
@struct : AccountPWLockInfo
@brief :
*/
typedef struct
{
    int enable;
    int max_failure_count;
    int lock_time_ms;
    LockInfo user[USER_INFO_COUNT];
} AccountPWLockInfo;
/**
@enum : AUTH_RESULT_E
@brief :
*/
typedef enum
{
    AUTH_RESULT_SUCCESS,
    AUTH_RESULT_FAIL,
    AUTH_RESULT_BLOCKED
} AUTH_RESULT_E;
/**
@class : accountlock
@brief :
@see
*
*/
class AccountLock
{
public:
    static bool create(int enable = 1, int max_failure_count = 5, int lock_time_ms = 30000);
    static bool destroy(void);

    AccountLock(int protocol, const char *ip);
    virtual ~AccountLock(void);

    void update_policy(int enable, int max_failure_count, int lock_time_ms);
    void update_userlist(const char *id[], int count);
    AUTH_RESULT_E check_auth_result(const char *id, bool granted, int *lock_time_sec);

    int get_banInfoResult(BanInfoResult info[USER_INFO_COUNT]);
    int get_failInfoResult(FailInfoResult info[USER_INFO_COUNT*FAIL_INFO_COUNT]);

private:
    bool connect(void);
    void disconnect(void);
    void reset_failure_count(LockInfo *lock);
    void increase_failure_count(LockInfo *lock);
    int check_lock(LockInfo *lock);
    LockInfo* find(const char *id);

    IPC_SEM sem;
    IPC_SHM shm;
    AccountPWLockInfo *lockInfo;

    int protocol;
    char cli_ip[IP_STR_LEN];
};

#endif
