/* 
* Company: ANZE Suspension
* File Name: rtc.cpp
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*****************************************************       HEADER FILE       *****************************************************/
#include "rtc.h"

//***************************************      REAL TIME CLOCK CLASS FUNCTIONS DEFINTION       **************************************/
ESP_ERROR RealTimeClock::begin(RTC_TYPE rtc_ic, TwoWire *port)
{
    ESP_ERROR err;
    err.on_error = false;

    rtc_class_ic = rtc_ic;

    switch (rtc_class_ic)
    {
    case RV8803_IC:
        if (rv8803.begin(*port) == false)
        {
            err.on_error = true;
            err.debug_message = "Error Initializing real time clock IC RV8803";
        }
        else
        {
            if (rv8803.updateTime() == false) //Updates the time variables from RTC
            {
                err.on_error = true;
                err.debug_message = "Real time clock failed to update time";
            }
            else
            {
                rtc_class_time.year = rv8803.getYear();
                rtc_class_time.month = rv8803.getMonth();
                rtc_class_time.day = rv8803.getDate();
                rtc_class_time.hours = rv8803.getHours();
                rtc_class_time.minutes = rv8803.getMinutes();
                rtc_class_time.seconds = rv8803.getSeconds();
            }
        }
        break;

    default:
        err.on_error = true;
        err.debug_message = "Only RV8803 has been implemented";
        break;
    }

    return err;
}

ESP_ERROR RealTimeClock::setToCompilerTime()
{
    ESP_ERROR err;
    err.on_error = false;

    switch (rtc_class_ic)
    {
    case RV8803_IC:
        if (rv8803.setToCompilerTime() == false)
        {
            err.on_error = true;
            err.debug_message = "Could not set to compiler time";
        }

        break;

    default:
        err.on_error = true;
        err.debug_message = "Only RV8803 has been implemented";
        break;
    }

    return err;
}

ESP_ERROR RealTimeClock::updateTime(DateTime new_time)
{
    ESP_ERROR err;
    err.on_error = false;

    switch (rtc_class_ic)
    {
    case RV8803_IC:
        if (rv8803.setTime(new_time.seconds,
                           new_time.minutes,
                           new_time.hours,
                           2, //weekday
                           new_time.day,
                           new_time.month,
                           new_time.year) == false)
        {
            err.on_error = true;
            err.debug_message = "Could not set new time";
        }
        break;

    default:
        err.on_error = true;
        err.debug_message = "Only RV8803 has been implemented";
        break;
    }
    return err;
}

void RealTimeClock::updateMillisecondsCounter(DateTime *system_time)
{
    //Millis
    if (system_time->milliseconds == 999)
    {
        system_time->milliseconds = 0;
        system_time->seconds++;

        //seconds
        if (system_time->seconds == 59)
        {
            system_time->seconds = 0;
            system_time->minutes++;

            if (system_time->minutes == 59)
            {
                system_time->minutes = 0;
                system_time->hours++;

                return;
            }

            return;
        }

        return;
    }

    system_time->milliseconds++;
}

String RealTimeClock::getTimeString()
{
    String temp_message;
    temp_message += rtc_class_time.hours;
    temp_message += ":";
    temp_message += rtc_class_time.minutes;
    temp_message += ":";
    temp_message += rtc_class_time.seconds;
    temp_message += ".";
    temp_message += rtc_class_time.milliseconds;

    return temp_message;
}
