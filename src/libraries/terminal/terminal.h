#pragma once

/*
 * File Name: Terminal.h
 * Project: ESP32 Utilities Libraries
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer the following features to the programmer without adding too much processing overhead
// *
// * 1. Time stamping
// * 2. Process time (how long a given function took to excecute)
// * 3. Process core (useful in dual core CPU like ESP)
// * 4. Message type (error, warning or information)
// * 5. System outputting debug message
// * 5. The actual debug message
// *
// * -- Example terminal output WITHOUT real time clock chip
// *
// *       * Time Stamp *         * Process Time *   * Core *  * Message Type *   * System *              * Debug Message *
// *
// *    [            51 mS]---------[+   11 mS]--------[1]----------[INF]------------[TER]------------- Terminal initialized
// *    [            59 mS]---------[+    0 mS]--------[1]----------[INF]------------[LED]------------- Debugging LED initialized
// *    [           117 mS]---------[+   58 mS]--------[1]----------[INF]------------[MEM]------------- eMMC Storage initialized
// *    ....
// *    ... So on and so forth
// *
// * -- Example terminal output WITH real time clock chip
// *
// *                                * Time Stamp *        * Process Time *  * Core *   * Message Type *   * System *              * Debug Message *
// *
// *    [ET       00:04:35.078] [            51 mS]---------[+   11 mS]--------[1]----------[INF]------------[TER]------------- Terminal initialized
// *    [ET       00:04:35.078] [            59 mS]---------[+    0 mS]--------[1]----------[INF]------------[LED]------------- Debugging LED initialized
// *    [ET       00:04:35.078] [           117 mS]---------[+   58 mS]--------[1]----------[INF]------------[MEM]------------- eMMC Storage initialized
// *    ....
// *    ... So on and so forth
// *
// * -- Please refer to the example included with the library for usage.
// *
// *
// * -- Future Improvements
// *
// *    1. Allow system independant debugging
// *    2. Set debug levels (error only, warning only, etc)
// *
// * Written by: Mateo Segura

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
// * None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <utils.h>
#include <Arduino.h>
#include <HardwareSerial.h>

//*****************************************************       DATA TYPES        *****************************************************/

// * Message types
enum TERMINAL_MESSAGE_TYPE
{
    INFO,
    WARNING,
    ERROR,
};

//***********************************************      TERMINAL MESSAGE CLASS        ************************************************/
class TerminalMessage
{
public:
    TerminalMessage() {}

    TerminalMessage(String message_body, String message_system, TERMINAL_MESSAGE_TYPE message_type, long time_stamp = 0, long process_time_taken = 0)
    {
        body = message_body;
        system = message_system;
        type = message_type;
        time = time_stamp;
        core = xPortGetCoreID();
        process_time = process_time_taken;
    }

    String body;
    String system;
    TERMINAL_MESSAGE_TYPE type;
    long time;
    uint32_t core;
    long process_time;
};

//****************************************************      TERMINAL CLASS        ***************************************************/
class Terminal
{
public:
    // * Initialize terminal
    void begin(HardwareSerial &app_uart_port, // Set to UART 0 by default
               bool time_stamp_micros_enabled = false,
               bool enable_system_time = false); // Set to milliseconds by default

    // * End
    void end();

    void setTimeKeeper(DateTime *time)
    {
        system_time = time;
    }

    // * Print serial message with the format described above
    void printMessage(TerminalMessage debug_message);

    // * Normal print & println
    void print(String message);
    void println(String message);

private:
    // * Status
    bool terminal_enabled;

    // * Terminal port
    HardwareSerial terminal_uart_port = Serial; // Set to UART0 by default

    // * Date Time
    DateTime *system_time;
    bool system_time_enabled;

    // * Initialization Variables
    bool time_stamp_micros;

    // * Buffers for time stamp stringss
    char time_stamp_buffer[14];
    char process_time_buffer[6];
    char system_time_buffer[25];

    // * Create nice timestamp.
    String createTimeStamp(unsigned long event_time);

    // * Create nice process timestamp.
    String createProcessTimeStamp(unsigned long event_time);
};

// End.