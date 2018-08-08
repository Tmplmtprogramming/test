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
@file  streamer.cpp
@brief Streamer class implementation
*/
#include "streamer.h"

#include "utility/utility_common/utility_wrapper.h"
#include "connection_manager.h"

/**
@brief constructor
@param taskname TASK name
@param task_id TASK ID
*/
Streamer::Streamer(const char* taskname, CLASS_ID task_id) :
    IoTask(taskname, task_id), conmanager(NULL), myUserId(-1),
    streamCount(0)
{
}

/**
@brief destructor
*/
Streamer::~Streamer(void)
{
    if (myUserId >= 0)
    {
        conmanager->remove_streamer(this);
    }
    while (streamCount > 0)
    {
        streamCount--;
        conmanager->decrease_streamcount();
    }
}

/**
@brief initialize association and register Streamer to ConnectionManager
@param conmanager - ConnectionManager
@return true on success, false on fail
*/
bool Streamer::initialize(ConnectionManager* conmanager)
{
    this->conmanager = conmanager;
    myUserId = conmanager->add_streamer(this);

    return (myUserId >= 0);
}

/**
@brief check streamer type and type argument
@param type - streamer type
@param arg - type specific parameter
@return if this meets the condition, return true. else return false.
*/
bool Streamer::check_type(STREAMER_TYPE type, void* arg)
{
    return false;
}

/**
@brief check stream count while adding a stream
@param type - STREAM_CONNECTION_TYPE
@param profileId - streaming profile id
@return 1 on success, 0 on fail
*/
int Streamer::add_stream(STREAM_CONNECTION_TYPE type, int profileId)
{

//#define RAW_IMAGE_PROFILE_NUM		15
//#define RAW_AUDIO_PROFILE_NUM		16
    if(profileId == EXTRA_SETTING_PROFILE_NUM || profileId == DPTZ_SETTING_PROFILE_NUM || profileId == RAW_IMAGE_PROFILE_NUM
            || profileId == RAW_AUDIO_PROFILE_NUM || profileId == QUAD_VIEW_PROFILE_NUM_FOR_MULTIDIRECTION || profileId == DIS_SETTING_PROFILE_NUM)
    {
        return 1;
    }

    if (conmanager == NULL)
    {
        return 0;
    }
    if(conmanager->increase_streamcount() == 0)
    {
        return 0;
    }

    streamCount++;
    return 1;
}

/**
@brief decrease stream count
@param profileId - streaming profile id
*/
void Streamer::delete_stream(int profileId)
{
    if (streamCount == 0)
    {
        DP_ERR("check delete profile id %d", profileId);
        return;
    }
    streamCount--;
    conmanager->decrease_streamcount();
}

/**
@brief get authorization
@param authtype - auth type
@return 1 on success, 0 on fail
*/
int Streamer::get_authorization(STREAMER_AUTH_TYPE type)
{
    return conmanager->get_authorization(type, this);
}

/**
@brief check authorization
@param authtype - auth type
@return if this streamer has the authorization return 1, else return 0
*/
int Streamer::check_authorization(STREAMER_AUTH_TYPE type)
{
    return conmanager->check_authorization(type, this);
}
/**
@brief release authorization of the streamer
@param authtype - auth type
*/
void Streamer::release_authorization(STREAMER_AUTH_TYPE type)
{
    conmanager->release_authorization(type, this);
}
/**
@brief on disconnect
*/
void Streamer::on_disconnect(void)
{
    //set_state(TS_DELETE);
    suicide();
    IoTask::suspend();
}

/**
@brief Streamer virtual function to get current streaming information
@param[out] info - StreamingInfo
@return if the streamer has streaming information return 1, else return 0
*/
int Streamer::get_streamingInfo(StreamingInfo* info)
{
    return 0;
}

