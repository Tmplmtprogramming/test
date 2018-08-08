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
@file  streamer.h
@brief Streamer class declaration
*/
#ifndef _STREAMER_H_
#define _STREAMER_H_
#include <netinet/in.h>
#include <arpa/inet.h>
#include "framework/component/iotask.h"

class ConnectionManager;


/**
 *
 * @enum
 * @brief
 */
typedef enum STREAMER_TYPE
{
    STREAMER_TYPE_VNP,
    STREAMER_TYPE_RTP_UNICAST,
    STREAMER_TYPE_RTP_MULTICAST,
    STREAMER_TYPE_CGI,
} STREAMER_TYPE;


/**
 *
 * @enum
 * @brief
 */
typedef enum STREAMER_AUTH_TYPE
{
    STREAMER_TALK_AUTH,
    STREAMER_FWUPDATE_AUTH,
    STREAMER_PLAYBACK_AUTH,
    STREAMER_PTZ_AUTH,
    STREAMER_AUTH_TYPE_ALL,
} STREAMER_AUTH_TYPE;
#define STREAMER_AUTH_TYPE_COUNT STREAMER_AUTH_TYPE_ALL


/**
 *
 * @enum
 * @brief
 */
typedef enum STREAM_CONNECTION_TYPE
{
    UNICAST_CONNECTION = 1,
    MULTICAST_CONNECTION
} STREAM_CONNECTION_TYPE;

/**
@union SocketAddress
@brief workaround to avoid struct-aliasing problem while socket address handling
*/
typedef union SocketAddress
{
    struct sockaddr_storage ss;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
    struct sockaddr sa;
} SocketAddress;

#define MAX_STREAM_PER_CONNECTION	10
/**
@struct StreamingInfo
@brief streaming status info for atc
*/
typedef struct StreamingInfo
{
    int occupied;
    char peerIp[40];
    int usedProfile[MAX_STREAM_PER_CONNECTION];
    int usedSimoCh[MAX_STREAM_PER_CONNECTION];
    int currentBitrate;
    int atcPriority;
    int networkStatus;
    int lastUpdateTime;
} StreamingInfo;

/**
@class Streamer
@brief base class for media streaming
*/
class Streamer : public IoTask
{
public:
    virtual ~Streamer(void);
    bool initialize(ConnectionManager* conmanager);

    virtual bool check_type(STREAMER_TYPE type, void* arg);
    virtual void on_disconnect(void);

    /**
     *
     * @param type
     * @param message
     * @param 0
     */
    virtual void fwupdatenoti(int type, unsigned int message = 0)
    {
        return;
    }

    /**
     *
     */
    virtual void drain_control_send_buff(void)
    {
        return;
    }
    virtual int get_streamingInfo(StreamingInfo* info);
protected:
    Streamer(const char* taskname, CLASS_ID task_id);
    int add_stream(STREAM_CONNECTION_TYPE type, int profileId);
    void delete_stream(int profileId);
    int get_authorization(STREAMER_AUTH_TYPE type);
    int check_authorization(STREAMER_AUTH_TYPE type);
    void release_authorization(STREAMER_AUTH_TYPE type);

protected:
    ConnectionManager* conmanager;

    int myUserId;
    int streamCount;
};


#endif

