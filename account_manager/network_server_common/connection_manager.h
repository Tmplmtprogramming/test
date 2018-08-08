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
@file  connection_manager.h
@brief ConnectionManager class declaration
*/
#ifndef _CONNECTION_MANAGER_H_
#define _CONNECTION_MANAGER_H_
#include <list>
#include <pthread.h>
#include "streamer.h"
#include "framework/component/event_target.h"
#include "utility/utility_common/utility_wrapper.h"
#include "data_container/event_data/network_streaming_info.h"

class Sysinfo;

/**
@class ConnectionManager
@brief manage streamer list
*/
class ConnectionManager : public EventTarget
{
public:
    static ConnectionManager* createnew(void);
    virtual ~ConnectionManager(void);
    virtual bool initialize();
    virtual bool suspend();

    void get_networkStreamingInfo(NetworkStreamingInfo* info);

    int add_streamer(Streamer* st);
    void remove_streamer(Streamer* st);
    void remove_all_streamer(void);
    Streamer* find_streamer(STREAMER_TYPE type, void* arg);

    int increase_connectioncount(void);
    void decrease_connectioncount(void);
    int increase_streamcount(void);
    void decrease_streamcount(void);
    int increase_multicastconnection(void);
    void decrease_multicastconnection(void);

    int get_localport(int count);
    int get_authorization(STREAMER_AUTH_TYPE authtype, Streamer *st);
    int check_authorization(STREAMER_AUTH_TYPE authtype, Streamer *st);
    void release_authorization(STREAMER_AUTH_TYPE authtype, Streamer *st);

    void fwupdatenoti(int type, unsigned int message = 0);
protected:
    ConnectionManager(void);

    virtual bool process_event(TaskEvent* event);

    void update_accountlock_list(void);
    int increase_count(int& count, int limit, const char *name);
    void decrease_count(int& count, const char* name);

private:
    int maxStreamerCount;
    int maxStreamCount;
    int maxStreamCountWhileRecording;
    int maxConnectionCount;
    int maxMulticastConnectionCount;

    Streamer** streamerList;
    Streamer* authStreamer[STREAMER_AUTH_TYPE_COUNT];

    pthread_mutex_t lock;
    int nextLocalPort;
    int connectionCount;
    int multicastConnectionCount;
    int streamCount;
    bool isRecordingOn;
};


#endif

