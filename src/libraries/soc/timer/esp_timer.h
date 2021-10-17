#pragma once
/* 
* Company: ANZE Suspension
* File Name: esp_timer.h
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer some abstraction for ESP32 system on chip.

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
// * None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>

//*****************************************************       DATA TYPES        *****************************************************/

//**************************************************      ESP32 TIMER CLASS        **************************************************/
class ESPtimer
{
public:
    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;

    ESPtimer(uint8_t hardware_timer)
    {
        timer_number = hardware_timer;
    }

    void setup()
    {
        timer = timerBegin(timer_number, timer_prescaler, count_up);
    }

    void attachInterrupt(void (*function)(), bool on_rising_edge = true)
    {
        timerAttachInterrupt(timer, function, on_rising_edge);
    }

    void dettachInterrupt()
    {
        timerDetachInterrupt(timer);
    }

    void timerPeriod(double milliseconds, bool auto_relaod = true)
    {
        milliseconds = milliseconds * 100;
        timerAlarmWrite(timer, milliseconds, auto_relaod);
    }

    void enableInterrupt()
    {
        timerAlarmEnable(timer);
    }

    bool disableInterrupt()
    {
        bool err = false;

        if (timerAlarmEnabled(timer))
        {
            timerAlarmDisable(timer);
        }
        else
        {
            err = true;
        }
        return !err;
    }

private:
    hw_timer_t *timer = NULL;

    uint8_t timer_number;
    uint32_t timer_prescaler = 800; // 80000000 / 80000 = 1000 ticks per second
    bool count_up = true;           // Count up by default
};

// End.