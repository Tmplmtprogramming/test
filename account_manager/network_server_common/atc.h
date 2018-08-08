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
@file atc.h
@brief Atc class declaration
*/
#ifndef _ATC_H_
#define _ATC_H_
#include "framework/component/event_target.h"
#include "data_container/video_data/video_codec_information.h"
#include "network_server/network_server_common/connection_manager.h"

#define DEBUG 0
#include "data_container/event_data/profile_running_info.h"
#include "data_container/event_data/network_streaming_info.h"

/**
@struct AtcState
@brief ATC running information of each profile
       this information is maintained as a class member variable
*/
typedef struct AtcState
{
    int profileNum;
    int direction;
    int lastCheckTime;
    int up2downTransitionCount;
    int adjustPercentSum;	// for averaging
    int adjustPercentCount;

    void add_adjustPercent(int adjustPercent);
    int get_averageAdjustPercent(void);
    void reset_adjustPercent(void);
} AtcState;

#define TAEHO 0

/**
@struct AtcInputInfo
@brief profile running information used by atc.
*/
typedef struct AtcInputInfo
{
    // from profileHandler
    int profileNum;
#if! TAEHO
    int currentBitrate; /**< bitrate in bps */
#else
    float currentBitrate;
#endif
    int atcModeSetting;
    int atcLimit;
    int atcSensitivity;
    int atcRate;
    // calculate from input/output
    int adjustPercent;
} AtcInputInfo;

/**
@struct AtcOutputInfo
@brief stream client connection information used by atc
*/
typedef struct AtcOutputInfo
{
    // from conmanager
    int occupied;
    int usedSimoCh[MAX_STREAM_PER_CONNECTION];
#if! TAEHO
    int outputBitrate;
#else
    float outputBitrate;
#endif
    int notSending;
    int atcPriority;
    // calculate from input/output
#if! TAEHO
    int inputBitrate;	/**< total bitrate of all used profiles */
    int transferRate;	/**< outputBitrate / (inputBitrate * margin) */
#else
    float inputBitrate;	/**< total bitrate of all used profiles */
    float transferRate;	/**< outputBitrate / (inputBitrate * margin) */
#endif
} AtcOutputInfo;

/**
@class Atc
@brief ATC(Auto Transmission Control) manager
*/
class Atc : public EventTarget
{
public:
    Atc(void);
    virtual ~Atc(void);
    virtual bool initialize();
    virtual bool process_event(TaskEvent* event);

private:
    void check_bandwidth(void);
    void gather_inOutInfo(AtcInputInfo in[MAX_SUB_CH_NUMBER], AtcOutputInfo out[MAX_STREAM_COUNT]);
    void calculate_outputTransferRate(AtcOutputInfo out[MAX_STREAM_COUNT]);
    int calculate_adjustPercent(const AtcOutputInfo out[MAX_STREAM_COUNT], int inputIndex);
    int is_streaming_input(const AtcOutputInfo* out, int inputIndex);
    int calculate_newAtcRate(AtcState* runningState, const AtcInputInfo* in, int adjustPercent);
    int calculate_newAtcRate_normal(AtcState* runningState, const AtcInputInfo* in, int adjustPercent);
    int calculate_newAtcRate_goingDown(AtcState* runningState, const AtcInputInfo* in, int adjustPercent);
    int calculate_newAtcRate_goingUp(AtcState* runningState, const AtcInputInfo* in, int adjustPercent);

    void set_eventState(bool state);
    int request_inoutStatus();

private:
    AtcState runningInfo[MAX_SUB_CH_NUMBER];
    bool eventState;

    static const int upSlope[11];	/**< atc rate will be increased by this slope(array index is current atc rate) */


    /**
     *
     * @enum
     * @brief
     */
    enum ATC_STATUS
    {
        ATC_IDLE,
        ATC_WAITING_BITRATE_INFO,
        ATC_RECEIVED_INPUT_INFO,
        ATC_RECEIVED_OUTPUT_INFO,
        ATC_RECEIVED_ALL_INFO,
        ATC_CALCULATING,
        ATC_SENDING_FEEDBACK
    };
    ATC_STATUS status;
    ProfileRunningInfo profile_running_info;
    NetworkStreamingInfo network_streaming_info;
};
#endif

