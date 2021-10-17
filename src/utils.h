#pragma once

/* 
* File Name: utils.h2
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>

//*****************************************************       DATA TYPES        *****************************************************/

// -- Global function return for ESP32 Utilities libraries error checking
class ESP_ERROR
{
public:
    ESP_ERROR()
    {
        on_error = false;
        debug_message = "";
    }

    ESP_ERROR(bool err, String error_message)
    {
        on_error = err;
        debug_message = error_message;
    }
    bool on_error;
    String debug_message;
};

class DateTime
{
public:
    DateTime()
    {
        year = 0;
        month = 0;
        day = 0;
        hours = 0;
        minutes = 0;
        seconds = 0;
        milliseconds = 0;
    }

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;

    String toString(bool date_enabled = false)
    {
        String temp_string;

        if (date_enabled)
        {
            temp_string += year;
            temp_string += month;
            temp_string += day;
        }

        temp_string += hours;
        temp_string += minutes;
        temp_string += seconds;
        temp_string += milliseconds;

        return temp_string;
    }

    void updateMilliseconds()
    {
        milliseconds++;

        if (milliseconds == 999)
        {
            milliseconds = 0;
            seconds++;

            if (seconds == 59)
            {
                seconds = 0;
                minutes++;

                if (minutes == 59)
                {
                    minutes = 0;
                    hours++;

                    if (hours == 24)
                    {
                        hours = 0;
                        day++;
                    }
                }
            }
        }
    }
};
