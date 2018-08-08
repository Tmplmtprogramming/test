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
@file  connection_manager.cpp
@brief ConnectionManager class implementation
*/

#include "connection_manager.h"
#include "utility/utility_common/utility_wrapper.h"
#include "network_server/network_server_common/account_lock.h"

#define MIN_LOCAL_UDP_PORT 40000
#define MAX_LOCAL_UDP_PORT 60000
#define GET_TYPE(data)		((data) >> 8)
#define GET_MESSAGE(data)	((data) & 0xff)
/**
@brief create an instance
@return new instance
*/
ConnectionManager* ConnectionManager::createnew(void)
{
    return new ConnectionManager();
}

/**
@brief constructor
*/
ConnectionManager::ConnectionManager(void) : EventTarget("conmanager", TT_MANAGER),
    maxStreamerCount(0), maxStreamCount(0), maxStreamCountWhileRecording(0),
    maxConnectionCount(0), maxMulticastConnectionCount(0),
    streamerList(NULL),
    nextLocalPort(MIN_LOCAL_UDP_PORT), connectionCount(0),
    multicastConnectionCount(0), streamCount(0), isRecordingOn(false)
{
    pthread_mutex_init(&lock, NULL);

    memset(authStreamer, 0, sizeof(authStreamer));

    set_eventmask(EV_RECORD_ADD_PROFILE);
    set_eventmask(EV_RECORD_DEL_PROFILE);
    set_eventmask(EV_SERVICE_SUSPENDED);
    set_eventmask(EV_FIRMWARE_UPDATE_NOTI_REQUESTED);
    set_eventmask(EV_ATC_REQUEST_OUTPUT_BITRATE);
}

/**
@brief destructor
*/
ConnectionManager::~ConnectionManager(void)
{
    pthread_mutex_destroy(&lock);
    UtilityWrapper::free(streamerList);
}

/**
@brief initialize association, and system-specific attributes
*/
bool ConnectionManager::initialize()
{
    maxStreamerCount = 60;

    maxStreamCount = UtilityWrapper::get_modelInfo(MODELINFO_MAX_STREAM_COUNT);
    maxStreamCountWhileRecording = UtilityWrapper::get_modelInfo(MODELINFO_MAX_REC_STREAM_COUNT);
    maxConnectionCount = UtilityWrapper::get_modelInfo(MODELINFO_MAX_CONNECTION_COUNT);
    maxMulticastConnectionCount = 512;

    streamerList = (Streamer**) UtilityWrapper::malloc(Utility::MemoryUtils::ETC_REGION, sizeof(Streamer*) * maxStreamerCount);
    memset(streamerList, 0, sizeof(Streamer*) * maxStreamerCount);
    EventSource::initialize();
    return true;
}

/**
@brief send firmware update notification through update_auth streamer
@param type - noti type
@param message - noti message
*/
void ConnectionManager::fwupdatenoti(int type, unsigned int message)
{
    DP_LOG("CONMANAGER1::fwupdatenoti--> type:%d, message:%d \n",type, message);
    pthread_mutex_lock(&lock);
    if (authStreamer[STREAMER_FWUPDATE_AUTH] && authStreamer[STREAMER_FWUPDATE_AUTH]->get_state() != SUSPENDED)
    {
        DP_LOG("CONMANAGER2::fwupdatenoti--> type:%d, message:%d\n",type, message);
        authStreamer[STREAMER_FWUPDATE_AUTH]->drain_control_send_buff();
        authStreamer[STREAMER_FWUPDATE_AUTH]->fwupdatenoti(type, message);
        authStreamer[STREAMER_FWUPDATE_AUTH]->drain_control_send_buff();
    }
    pthread_mutex_unlock(&lock);
}

/**
@brief handle task event
	EV_RECORD_ADD_PROFILE, EV_RECORD_DEL_PROFILE - change recording status, which can affect streaming capability
@param event - TaskEvent
@return true
*/
bool ConnectionManager::process_event(TaskEvent* event)
{
    switch(event->get_eventtype())
    {
        case EV_FIRMWARE_UPDATE_NOTI_REQUESTED:
        {
            if(event->get_data() > 0xFF)
            {
                fwupdatenoti(GET_TYPE(event->get_data()), GET_MESSAGE(event->get_data()));
            }
            else
            {
                fwupdatenoti(event->get_data());
            }
            break;
        }
        case EV_RECORD_ADD_PROFILE:
        {
            isRecordingOn = true;
            break;
        }
        case EV_RECORD_DEL_PROFILE:
        {
            isRecordingOn = false;
            break;
        }
        case EV_SERVICE_SUSPENDED:
        {
            suspend();
            break;
        }
        case EV_ATC_REQUEST_OUTPUT_BITRATE:
        {
            try
            {
                NetworkStreamingInfo* info = new NetworkStreamingInfo;
                ::memset(info, 0x00, sizeof(NetworkStreamingInfo));
                this->get_networkStreamingInfo(info);
                this->throw_taskEventWithNewData(EV_ATC_RESPONSE_OUTPUT_BITRATE, TT_ATC, info);
            }
            catch(std::exception& e)
            {
                DP_ERR("%s Exception occures!!", __PRETTY_FUNCTION__);
                throw e;
            }
            break;
        }
        default:
            DP_ERR("%d", event->get_eventtype());
            break;
    }
    return true;
}


/**
 *
 * @return
 */
bool ConnectionManager::suspend()
{
    remove_all_streamer();
    EventTarget::suspend();
    return true;
}
/**
@brief add a streamer to the streamer list
@param st - streamer instance to add
@return streamer id on success, -1 on fail(streamer full)
*/
int ConnectionManager::add_streamer(Streamer* st)
{
    int ret = -1;

    pthread_mutex_lock(&lock);
    for (int i = 0; i < maxStreamerCount; i++)
    {
        if (streamerList[i] == NULL)
        {
            streamerList[i] = st;
            ret = i;
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    if (ret == -1)
    {
        DP_ERR("streamer full");
    }

    return ret;
}

/**
@brief remove a streamer from the streamer list
@param st - streamer instance to remove
*/
void ConnectionManager::remove_streamer(Streamer* st)
{
    release_authorization(STREAMER_AUTH_TYPE_ALL, st);

    pthread_mutex_lock(&lock);
    for (int i = 0; i < maxStreamerCount; i++)
    {
        if (streamerList[i] == st)
        {
            streamerList[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
}



/**
 *
 */
void ConnectionManager::remove_all_streamer()
{
    int i;
    pthread_mutex_lock(&lock);
//	switch (reason) {
//		case DISCONNECT_REASON_FW_UPDATE:
    for (i = 0; i < maxStreamerCount; i++)
    {
        if (streamerList[i] && streamerList[i] != authStreamer[STREAMER_FWUPDATE_AUTH])
        {
            streamerList[i]->on_disconnect();
        }
    }
//			break;
//		default:
//			for (i = 0; i < maxStreamerCount; i++) {
//				if (streamerList[i])
//					streamerList[i]->on_disconnect();
//			}
//			break;
//	}
    pthread_mutex_unlock(&lock);
}

/**
@brief find a streamer that meets the condition
@param type - streamer type
@param arg - type specific parameter
@return found streamer. NULL if failed to find
*/
Streamer* ConnectionManager::find_streamer(STREAMER_TYPE type, void* arg)
{
    Streamer *ret = NULL;

    pthread_mutex_lock(&lock);
    for (int i = 0; i < maxStreamerCount; i++)
    {
        if (streamerList[i] && streamerList[i]->check_type(type, arg))
        {
            ret = streamerList[i];
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    return ret;
}

/**
@brief get local udp port
@param count - necessary port count
@return first of \a count continuous ports. return value will be an even number
*/
int ConnectionManager::get_localport(int count)
{
    pthread_mutex_lock(&lock);
    int ret = (nextLocalPort + 1) & (~1);	// make even
    nextLocalPort = ret + count;
    if (nextLocalPort >= MAX_LOCAL_UDP_PORT)
    {
        nextLocalPort = MIN_LOCAL_UDP_PORT;
    }
    pthread_mutex_unlock(&lock);

    return ret;
}

/**
@brief increase stream or connection count
@param[in,out] count - target count
@param limit - max limit
@param name - count name (for debug purpose)
@return changed count on success, 0 on fail
*/
int ConnectionManager::increase_count(int& count, int limit, const char *name)
{
    int ret;
    pthread_mutex_lock(&lock);
    if (count < limit)
    {
        ret = ++count;
    }
    else
    {
        ret = 0;
    }
    pthread_mutex_unlock(&lock);

    if (ret == 0)
    {
        DP_ERR("%s full", name);
    }
    else
    {
        DP_LIVE("add %s: %d", name, ret);
    }

    return ret;
}

/**
@brief decrease stream or connection count
@param[in,out] count - target count
@param name - count name (for debug purpose)
*/
void ConnectionManager::decrease_count(int& count, const char* name)
{
    pthread_mutex_lock(&lock);
    count--;
    pthread_mutex_unlock(&lock);
    DP_LIVE("del %s: %d", name, count);
}

/**
@brief increase connection count, and check max connection
@return total connection count on success, 0 on fail
*/
int ConnectionManager::increase_connectioncount(void)
{
    // Multiconnection is not for under total connection limitation(Currently connected Count is not included in maxConnectionCount)
    return increase_count(connectionCount, maxConnectionCount+multicastConnectionCount, "connection"); 
}

/**
@brief decrease connection count
*/
void ConnectionManager::decrease_connectioncount(void)
{
    decrease_count(connectionCount, "connection");
}

/**
@brief increase stream count, and check max streaming capability
@return total stream count on success, 0 on fail
*/
int ConnectionManager::increase_streamcount(void)
{
    int limit = isRecordingOn? maxStreamCountWhileRecording : maxStreamCount;

    return increase_count(streamCount, limit, "stream");
}

/**
@brief decrease stream count
*/
void ConnectionManager::decrease_streamcount(void)
{
    decrease_count(streamCount, "stream");
}

/**
@brief increase multicast connection count, and check max count
	this count is not about multicast stream, but about rtsp connection
@return total multicast connection count on success, 0 on fail
*/
int ConnectionManager::increase_multicastconnection(void)
{
    return increase_count(multicastConnectionCount, maxMulticastConnectionCount, "multicast");
}

/**
@brief decrease multicast connection count
	this count is not about multicast stream, but about rtsp connection
*/
void ConnectionManager::decrease_multicastconnection(void)
{
    decrease_count(multicastConnectionCount, "multicast");
}

/**
@brief get authorization
@param authtype - auth type
@param st - a streamer which wants the auth
@return 1 on success, 0 on fail
*/
int ConnectionManager::get_authorization(STREAMER_AUTH_TYPE authtype, Streamer *st)
{
    int ret = 0;

    pthread_mutex_lock(&lock);
    if (authStreamer[authtype] == NULL)
    {
        authStreamer[authtype] = st;
        ret = 1;
    }
    else if (authStreamer[authtype] == st)
    {
        ret = 1;
    }
    pthread_mutex_unlock(&lock);

    return ret;
}

/**
@brief check streamer authorization
@param authtype - auth type
@param st - streamer
@return if the streamer is authorized return 1, else return 0
*/
int ConnectionManager::check_authorization(STREAMER_AUTH_TYPE authtype, Streamer *st)
{
    return (authStreamer[authtype] == st);
}

/**
@brief release authorization of the streamer
@param authtype - auth type
@param st - streamer
*/
void ConnectionManager::release_authorization(STREAMER_AUTH_TYPE authtype, Streamer *st)
{
    int i;
    pthread_mutex_lock(&lock);
    if (authtype == STREAMER_AUTH_TYPE_ALL)
    {
        for (i = 0; i < STREAMER_AUTH_TYPE_COUNT; i++)
        {
            if (authStreamer[i] == st)
            {
                authStreamer[i] = NULL;
            }
        }
    }
    else
    {
        if (authStreamer[authtype] == st)
        {
            authStreamer[authtype] = NULL;
        }
    }
    pthread_mutex_unlock(&lock);
}

/**
@brief get connection status of all streamers
@param[out] info - NetworkStreamingInfo
*/
void ConnectionManager::get_networkStreamingInfo(NetworkStreamingInfo* info)
{
    memset(info, 0, sizeof(NetworkStreamingInfo));

    int connid = 0;

    pthread_mutex_lock(&lock);
    for (int i = 0; i < maxStreamerCount; i++)
    {
        if (streamerList[i] && streamerList[i]->get_streamingInfo(&info->connection[connid]))
        {
            if (++connid >= MAX_STREAM_COUNT)
            {
                break;
            }
        }
    }
    pthread_mutex_unlock(&lock);
}


