#pragma once
/*
 * Company: ANZE Suspension
 * File Name: rtc.h
 * Project: ESP32 Utilities
 * Version: 1.0
 * Compartible Hardware:
 * Date Created: September 8, 2021
 * Last Modified: September 9, 2021
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer some abstraction for RTC ICs.

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
// * None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <utils.h>
#include <Arduino.h>
#include "Wire.h"
#include "chips/rv8803/rv8803.h"
#include "chips/rv3027/rv3028.h"

//*****************************************************       DATA TYPES        *****************************************************/

//************************************************      REAL TIME CLOCK CLASS        ************************************************/
class RealTimeClock
{
public:
    // * RTC chips
    enum RTC_TYPE
    {
        RV8803_IC,
        RV3028_IC,
    };

    // * Initialize real time clock
    ESP_ERROR begin(RTC_TYPE rtc_ic, TwoWire &port);

    // * Set RTC chip to compiler time
    ESP_ERROR setToCompilerTime();

    // * Update time
    ESP_ERROR updateTime(DateTime new_time);

    // * Milliseconds counter
    void updateMillisecondsCounter(DateTime &system_time);

    // * Return time string
    String getTimeString();

    // * Return time
    DateTime getTime()
    {
        return rtc_class_time;
    }

private:
    // * Implemented chips
    RV8803 rv8803;
    RV3028 rv3028;

    // * Local Variables
    RTC_TYPE rtc_class_ic;
    DateTime rtc_class_time;
};

// End.