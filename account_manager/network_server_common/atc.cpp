/**********************************************************************
* Copyright(c) 2016 by Hanwha Techwin, Inc.
*
* This software is copyrighted by, and is the sole property
* of Hanwha Techwin.
*
* Hanwha Techwin, Co., Ltd.
* http://www.hanwhatechwin.co.kr
*********************************************************************/
/**
@file atc.cpp
@brief Atc class declaration
*/
#include "atc.h"
#include "connection_manager.h"
#include "audio_video/audio_video_common/profile_handler.h"
#include "socket/bsdsock.h"
#include "utility/utility_common/utility_wrapper.h"

#if (PROFILING_INTERVAL != NETWORK_PROFILING_INTERVAL)
#error in/out profiling intervals must be same
#endif

#define STEP_UP_THRESHOLD 85
#define STEP_DOWN_THRESHOLD 90

#define STEP_DOWN_THRESHOLD_T (PROFILING_INTERVAL*3) /*30000*/
#define STEP_UP_THRESHOLD_T (PROFILING_INTERVAL*3) /*30000*/

#define INITIAL_UP_TO_DOWN_TRANSITION_COUNT	2

#define ATC_EVENT_HOLD_TIME	15000	// same with webpage MD notification time(15sec)


/**
 *
 * @enum
 * @brief
 */
enum ATC_DIRECTION
{
    GOING_DOWN,
    GOING_UP,
    GOOD_WORKING
};

#define ATC_TIMER_ID_CHECK_BW          EventSource::JOB_ID_0
#define ATC_TIMER_ID_STOP_EVENTMODE    EventSource::JOB_ID_1

#define ADJUST_LIMIT 10

const int Atc::upSlope[11] = {3,3,3,4,5,7,7,6,3,3,3};

/**
@brief constructor
*/
Atc::Atc(void) : EventTarget("ATC", TT_ATC), eventState(false) , status(ATC_IDLE)
{
    memset(runningInfo, 0, sizeof(runningInfo));
    memset(&this->profile_running_info, 0x00, sizeof(ProfileRunningInfo));
    memset(&this->network_streaming_info, 0x00, sizeof(network_streaming_info));
    set_eventmask(EV_MD_CHANGED);
    set_eventmask(EV_ATC_RESPONSE_INPUT_BITRATE);
    set_eventmask(EV_ATC_RESPONSE_OUTPUT_BITRATE);
}

/**
@brief destructor
*/
Atc::~Atc(void)
{
}

/**
@brief initialize association
*/
bool Atc::initialize()
{
    register_delayedjob(PROFILING_INTERVAL, ATC_TIMER_ID_CHECK_BW, EVENT_SENDING_COUNT_INFINITE);
    return EventSource::initialize();
}

/**
@brief process task event
    EV_MD_CHANGED -> if on, set full-frame mode. if off, set a timer for i-frame mode
    DELAYED_JOB_ID_0 (ATC_TIMER_ID_CHECK_BW) -> calculate rate for compression/framerate control mode atc
    DELAYED_JOB_ID_1 (ATC_TIMER_ID_STOP_EVENTMODE) -> set i-frame mode
@param event - TaskEvent
*/
bool Atc::process_event(TaskEvent* event)
{
    switch(event->get_eventtype())
    {
        case DELAYEDJOB_EVENT(ATC_TIMER_ID_CHECK_BW):
        {
            this->request_inoutStatus();
            break;
        }
        case DELAYEDJOB_EVENT(ATC_TIMER_ID_STOP_EVENTMODE):
            set_eventState(false);
            break;
        case EV_MD_CHANGED:
        {
            if (event->get_data())
            {
                unregister_delayedjob(ATC_TIMER_ID_STOP_EVENTMODE);
                set_eventState(true);
            }
            else
            {
                register_delayedjob(ATC_EVENT_HOLD_TIME, ATC_TIMER_ID_STOP_EVENTMODE);
            }
            break;
        }
        case EV_ATC_RESPONSE_INPUT_BITRATE:
        {
            ::memset(&this->profile_running_info, 0x00, sizeof(ProfileRunningInfo));
            ::memcpy(&this->profile_running_info, event->get_sharedData<ProfileRunningInfo>(), sizeof(ProfileRunningInfo));
            if(this->status == ATC_WAITING_BITRATE_INFO)
            {
                this->status = ATC_RECEIVED_INPUT_INFO;
            }
            else if(this->status == ATC_RECEIVED_OUTPUT_INFO)
            {
                this->status = ATC_RECEIVED_ALL_INFO;
                this->check_bandwidth();
            }
            break;
        }
        case EV_ATC_RESPONSE_OUTPUT_BITRATE:
        {
            ::memset(&this->network_streaming_info, 0x00, sizeof(network_streaming_info));
            ::memcpy(&this->network_streaming_info, event->get_sharedData<NetworkStreamingInfo>(), sizeof(NetworkStreamingInfo));
            if(this->status == ATC_WAITING_BITRATE_INFO)
            {
                this->status = ATC_RECEIVED_OUTPUT_INFO;
            }
            else if(this->status == ATC_RECEIVED_INPUT_INFO)
            {
                this->status = ATC_RECEIVED_ALL_INFO;
                this->check_bandwidth();
            }
            break;
        }
        default:
        {
            return false;
        }
    }
    return true;
}

/**
@brief main function to calculate atc rate from in/out bandwidth, and send the result to video manager
*/
void Atc::check_bandwidth(void)
{
    AtcInputInfo in[MAX_SUB_CH_NUMBER] = {};	// index: simoch
    AtcOutputInfo out[MAX_STREAM_COUNT] = {};

    gather_inOutInfo(in, out);
    this->status = ATC_CALCULATING;
    calculate_outputTransferRate(out);

    for (int i = 0; i < MAX_SUB_CH_NUMBER; i++)
    {
        if (in[i].profileNum >= 0)
        {
            int adjust_percent = calculate_adjustPercent(out, i);
#if DEBUG
            printf("adjust_percent = %d\n", adjust_percent);
#endif
            int atc_rate = calculate_newAtcRate(&runningInfo[i], &in[i], adjust_percent);
#if DEBUG
            printf("atc_rate = %d, in[%d].atcRate = %d\n", atc_rate, i, in[i].atcRate);
#endif
            if (atc_rate != in[i].atcRate)
            {
                DP_LOG("Throwing ATC Event %d(%d, %d)", atc_rate | (i << 16), atc_rate, i);
                throw_taskevent(EV_ATC_NOTIFICATION, TT_PROFILE_HANDLER, atc_rate | (i << 16));
            }
        }
    }
    this->status = ATC_IDLE;
}

/**
@brief get input/output bandwidth information from video manager and connection manager
@param[out] in - array of AtcInputInfo (array index is simo channel)
@param[out] out - array of AtcOutputInfo
*/
void Atc::gather_inOutInfo(AtcInputInfo in[MAX_SUB_CH_NUMBER], AtcOutputInfo out[MAX_STREAM_COUNT])
{
    ProfileRunningInfo input;
    NetworkStreamingInfo output;

    static const int atc_sensitivity[5] = {2, 5, 10, 20, 30};

    if(this->status == ATC_RECEIVED_ALL_INFO)
    {
        ::memset(&input, 0x00, sizeof(ProfileRunningInfo));
        ::memset(&output, 0x00, sizeof(NetworkStreamingInfo));
        ::memcpy(&input, &(this->profile_running_info), sizeof(ProfileRunningInfo));
        ::memcpy(&output, &(this->network_streaming_info), sizeof(NetworkStreamingInfo));
    }
    else
    {
        return;
    }

#if DEBUG
    std::cout << "ATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATC" << std::endl;
#endif
    for (int i = 0; i < MAX_SUB_CH_NUMBER; i++)
    {
        in[i].profileNum = input.profile[i].profileNum;
        in[i].currentBitrate =  input.profile[i].currentBitrate;
        in[i].atcModeSetting = input.profile[i].atcModeSetting;
        in[i].atcLimit = input.profile[i].atcLimit;
        in[i].atcSensitivity = atc_sensitivity[input.profile[i].atcSensitivity];
        in[i].atcRate = input.profile[i].atcRate;

        if (runningInfo[i].profileNum != in[i].profileNum)
        {
            memset(&runningInfo[i], 0, sizeof(AtcState));
            runningInfo[i].profileNum = in[i].profileNum;
            runningInfo[i].direction = GOOD_WORKING;
            runningInfo[i].lastCheckTime = UtilityWrapper::get_milisecond();
        }
#if DEBUG
        std::cout << "in[" << i << "].currentBitrate = " << (long)in[i].currentBitrate << std::endl;
#endif
    }
#if DEBUG
    std::cout << "ATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATC" << std::endl << std::endl;
#endif
#if DEBUG
    std::cout << "ATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATC" << std::endl;
#endif
    for (int i = 0; i < MAX_STREAM_COUNT; i++)
    {
        if (output.connection[i].occupied)
        {
            out[i].occupied = 1;
            memcpy(out[i].usedSimoCh, output.connection[i].usedSimoCh, sizeof(int) * MAX_STREAM_PER_CONNECTION);
            out[i].outputBitrate = output.connection[i].currentBitrate;
            out[i].atcPriority = output.connection[i].atcPriority;

            for (int j = 0; j < MAX_STREAM_PER_CONNECTION; j++)
            {
                int profileId = output.connection[i].usedSimoCh[j];
                if (profileId >= 0 && profileId < MAX_SUB_CH_NUMBER && in[profileId].currentBitrate > 0)
                {
                    out[i].inputBitrate += in[profileId].currentBitrate;
                }
            }

            if (UtilityWrapper::get_milisecond() - output.connection[i].lastUpdateTime > PROFILING_INTERVAL * 3)
            {
                out[i].outputBitrate = 0;
                out[i].notSending = 1;
            }
        }
#if DEBUG
        std::cout << "out[" << i << "].inputBitrate = " << (long)out[i].inputBitrate << std::endl;
        std::cout << "out[" << i << "].outputBitrate = " << (long)out[i].outputBitrate << std::endl;
#endif
    }
#if DEBUG
    std::cout << "ATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATC" << std::endl;
#endif
}

/**
@brief calculate network transfer rate(compared to all input data) of each connection
@param[in,out] out - array of AtcOutputInfo
*/
void Atc::calculate_outputTransferRate(AtcOutputInfo out[MAX_STREAM_COUNT])
{
#if DEBUG
    std::cout << "ATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATC" << std::endl;
#endif
    for (int i = 0; i < MAX_STREAM_COUNT; i++)
    {
        if (out[i].occupied)
        {
            if (out[i].notSending == 1)
            {
                out[i].transferRate = ADJUST_LIMIT;
                continue;
            }

            int bitrate_margin;
            if (out[i].inputBitrate > 800 * 1024)
            {
                bitrate_margin = 95;
            }
            else if (out[i].inputBitrate > 500 * 1024)
            {
                bitrate_margin = 85;
            }
            else
            {
                bitrate_margin = 75;
            }

#if! DEBUG
            int marginal_bitrate = out[i].inputBitrate * bitrate_margin / 100;
#else
            float marginal_bitrate = out[i].inputBitrate * bitrate_margin / 100;
#endif
            out[i].transferRate = out[i].outputBitrate * 100 / marginal_bitrate;
            if (out[i].transferRate > 100)
            {
                out[i].transferRate = 100;
            }
            else if (out[i].transferRate < ADJUST_LIMIT)
            {
                out[i].transferRate = ADJUST_LIMIT;
            }
#if DEBUG
            printf("marginal_bitrate = %f bitrate_margin = %d\n", marginal_bitrate, bitrate_margin);
#if TAEHO
            printf("out[%d].inputBitrate = %f\n", i, out[i].inputBitrate);
            printf("out[%d].outputBitrate = %f\n", i, out[i].outputBitrate);
            printf("out[%d].transferRate = %f\n", i, out[i].transferRate);
#else
            printf("out[%d].inputBitrate = %d\n", i, out[i].inputBitrate);
            printf("out[%d].outputBitrate = %d\n", i, out[i].outputBitrate);
            printf("out[%d].transferRate = %d\n", i, out[i].transferRate);
#endif
#endif
        }
    }
#if DEBUG
    std::cout << "ATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATCATC" << std::endl;
#endif
}

/**
@brief calculate adjust rate of a profile.
       current implementation adapts input bitrate to the output bitrate of the client with highest priority.
       if more than 1 clients have same priority, choose the worst one
@param out - all AtcOutputInfo
@param inputIndex - simo channel of target profile
@return calculated adjust percent
*/
int Atc::calculate_adjustPercent(const AtcOutputInfo out[MAX_STREAM_COUNT], int inputIndex)
{
    int max_priority = 0;	// minimum valid priority is 1
#if! DEBUG
    int min_rate = 100;
#else
    int min_rate = 0;
#endif

    for (int i = 0; i < MAX_STREAM_COUNT; i++)
    {
        if (is_streaming_input(&out[i], inputIndex))
        {
#if DEBUG
            printf("HERE00, i = %d, max_priority = %d, out[%d].atcPriority = %d\n",i, max_priority, i, out[i].atcPriority);
#endif
            if (max_priority < out[i].atcPriority)
            {
#if DEBUG
                printf("HERE01, i = %d\n", i);
#endif
                max_priority = out[i].atcPriority;
                min_rate = out[i].transferRate;
#if DEBUG
                printf("min_rate = %d\n", out[i].transferRate);
#endif
            }
            else if (max_priority == out[i].atcPriority && min_rate > out[i].transferRate)
            {
#if DEBUG
                printf("HERE03, i = %d\n", i);
#endif
                min_rate = out[i].transferRate;
#if DEBUG
                printf("min_rate = %d\n", out[i].transferRate);
#endif
            }
#if DEBUG
            printf("HERE05, i = %d\n", i);
#endif
        }
    }
#if DEBUG
    printf("min_rate = %d @ %s\n", min_rate, __PRETTY_FUNCTION__);
#endif
    return min_rate;
}

/**
@brief check if the output connection is streaming the given input profile
@param out - AtcOutputInfo
@param inputIndex - simo channel
@return if the output stream contains input channel return 1, else return 0
*/
int Atc::is_streaming_input(const AtcOutputInfo* out, int inputIndex)
{
    if (out->occupied == 0)
    {
        return 0;
    }
    for (int i = 0; i < MAX_STREAM_PER_CONNECTION; i++)
    {
        if (out->usedSimoCh[i] == inputIndex)
        {
            return 1;
        }
    }
    return 0;
}

/**
@brief calculate new ATC rate when the previous running state was "GOOD_WORKING"
@param[in,out] runningState - AtcState of the target profile
@param in - AtcInputInfo of target profile
@param adjustPercent - result of calculate_adjustPercent()
@return new atc rate
*/
int Atc::calculate_newAtcRate_normal(AtcState* runningState, const AtcInputInfo* in, int adjustPercent)
{
    int now = UtilityWrapper::get_milisecond();
    int newAtcRate = in->atcRate;

    if (adjustPercent >= STEP_DOWN_THRESHOLD)
    {
        runningState->lastCheckTime = now;
    }
    else
    {
        if (now - runningState->lastCheckTime > STEP_UP_THRESHOLD_T)
        {
            int averagePercent = runningState->get_averageAdjustPercent();
            if (averagePercent <= STEP_DOWN_THRESHOLD)
            {
                runningState->direction = GOING_DOWN;
                newAtcRate = newAtcRate * averagePercent / 100;
            }
            runningState->reset_adjustPercent();
            runningState->lastCheckTime = now;
        }
        else
        {
            runningState->add_adjustPercent(adjustPercent);
        }
    }
    return newAtcRate;
}

/**
@brief calculate new ATC rate when the previous running state was "GOING_DOWN"
@param[in,out] runningState - AtcState of the target profile
@param in - AtcInputInfo of target profile
@param adjustPercent - result of calculate_adjustPercent()
@return new atc rate
*/
int Atc::calculate_newAtcRate_goingDown(AtcState* runningState, const AtcInputInfo* in, int adjustPercent)
{
    int now = UtilityWrapper::get_milisecond();
    int newAtcRate = in->atcRate;

    if (adjustPercent < STEP_DOWN_THRESHOLD)
    {
        if (runningState->up2downTransitionCount-- < 0)
        {
            newAtcRate = newAtcRate * adjustPercent / 100;
            runningState->lastCheckTime = now;
            runningState->reset_adjustPercent();
        }
    }
    else
    {
        runningState->up2downTransitionCount = 0;
        if (now - runningState->lastCheckTime > STEP_UP_THRESHOLD_T)
        {
            runningState->lastCheckTime = now;
            int averagePercent = runningState->get_averageAdjustPercent();
            if (averagePercent >= STEP_DOWN_THRESHOLD)
            {
                runningState->direction = GOING_UP;
                newAtcRate += upSlope[in->atcRate / 10];
            }
            runningState->reset_adjustPercent();
        }
        else
        {
            runningState->add_adjustPercent(adjustPercent);
        }
    }
    return newAtcRate;
}

/**
@brief calculate new ATC rate when the previous running state was "GOING_UP"
@param[in,out] runningState - AtcState of the target profile
@param in - AtcInputInfo of target profile
@param adjustPercent - result of calculate_adjustPercent()
@return new atc rate
*/
int Atc::calculate_newAtcRate_goingUp(AtcState* runningState, const AtcInputInfo* in, int adjustPercent)
{
    int now = UtilityWrapper::get_milisecond();
    int newAtcRate = in->atcRate;

    if (adjustPercent >= STEP_UP_THRESHOLD)
    {
        newAtcRate += upSlope[in->atcRate / 10];
        runningState->lastCheckTime = now;
        runningState->reset_adjustPercent();
    }
    else
    {
        if (now - runningState->lastCheckTime > STEP_UP_THRESHOLD_T)
        {
            runningState->lastCheckTime = now;
            int average = runningState->get_averageAdjustPercent();
            if (average <= STEP_DOWN_THRESHOLD)
            {
                runningState->up2downTransitionCount = INITIAL_UP_TO_DOWN_TRANSITION_COUNT;
                runningState->direction = GOING_DOWN;
                newAtcRate = newAtcRate * average / 100;
            }
            runningState->reset_adjustPercent();
        }
        else
        {
            runningState->add_adjustPercent(adjustPercent);
        }
    }
    return newAtcRate;
}

/**
@brief calculate new ATC rate of a profile
@param[in,out] runningState - AtcState of the target profile
@param in - AtcInputInfo of target profile
@param adjustPercent - result of calculate_adjustPercent()
@return new atc rate
*/
int Atc::calculate_newAtcRate(AtcState* runningState, const AtcInputInfo* in, int adjustPercent)
{
    int newAtcRate;

#if DEBUG
    printf("runningState->direction = %d\n", runningState->direction);
#endif
    switch (runningState->direction)
    {
        case GOOD_WORKING:
        {
#if DEBUG
            printf("ATC good working, adjustPercent = %d\n", adjustPercent);
#endif
            newAtcRate = calculate_newAtcRate_normal(runningState, in, adjustPercent);
            break;
        }
        case GOING_DOWN:
        {
#if DEBUG
            printf("ATC going down, adjustPercent = %d\n", adjustPercent);
#endif
            newAtcRate = calculate_newAtcRate_goingDown(runningState, in, adjustPercent);
            break;
        }
        case GOING_UP:
        {
#if DEBUG
            printf("ATC going up, adjustPercent = %d\n", adjustPercent);
#endif
            newAtcRate = calculate_newAtcRate_goingUp(runningState, in, adjustPercent);
            break;
        }
        default:
        {
            DP_ERR("invalid atc direction %d", runningState->direction);
            newAtcRate = in->atcRate;
            break;
        }
    }

    if (newAtcRate < in->atcRate - in->atcSensitivity)
    {
        newAtcRate = in->atcRate - in->atcSensitivity;
    }
    if (newAtcRate < in->atcLimit)
    {
        newAtcRate = in->atcLimit;
    }
    if (newAtcRate >= 100)
    {
        newAtcRate = 100;
        runningState->direction = GOOD_WORKING;
    }

    return newAtcRate;
}

/**
@brief change event mode atc state
@param state - new state
*/
void Atc::set_eventState(bool state)
{
    if (eventState != state)
    {
        if (state)
        {
            eventState = true;
            throw_taskevent(EV_ATC_EVENT_STREAM_ONCE, TT_PROFILE_HANDLER);
        }
        else
        {
            eventState = false;
            throw_taskevent(EV_ATC_EVENT_STREAM_MUTE, TT_PROFILE_HANDLER);
        }
    }
}


/**
@brief add an adjust percent entry for average
@param adjustPercent - adjust percent
*/
void AtcState::add_adjustPercent(int adjustPercent)
{
    adjustPercentCount++;
    adjustPercentSum += adjustPercent;
}

/**
@brief calculate average of all adjust percent values provided by add_adjustPercent()
@return average value
*/
int AtcState::get_averageAdjustPercent(void)
{
    if (adjustPercentCount == 0)
    {
        DP_ERR("try to divide by zero");
        adjustPercentSum = 0;
        return 0;
    }
    return adjustPercentSum / adjustPercentCount;
}

/**
@brief clear stored adjust percents for average
*/
void AtcState::reset_adjustPercent(void)
{
    adjustPercentSum = 0;
    adjustPercentCount = 0;
}

/**
 @brief requests bitrate infomation from other components.
 @return always 0
 */
int Atc::request_inoutStatus()
{
    this->throw_taskevent(EV_ATC_REQUEST_OUTPUT_BITRATE, TT_MANAGER);
    this->throw_taskevent(EV_ATC_REQUEST_INPUT_BITRATE, TT_PROFILE_HANDLER);

    this->status = ATC_WAITING_BITRATE_INFO;
    return 0;
}
