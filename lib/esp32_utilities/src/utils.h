#pragma once

/* 
* Company: ANZE Suspension
* File Name: utils.h
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
    ESP_ERROR() {}

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
    DateTime() {}
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
};
