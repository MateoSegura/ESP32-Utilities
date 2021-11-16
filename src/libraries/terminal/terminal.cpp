/*
 * Company: ANZE Suspension
 * File Name: Terminal.cpp
 * Project: ESP32 Utilities
 * Version: 1.0
 * Compartible Hardware:
 * Date Created: September 8, 2021
 * Last Modified: September 9, 2021
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//*****************************************************       HEADER FILE       *****************************************************/
#include "terminal.h"

//*****************************************       TERMINAL CLASS FUNCTIONS DEFINTION       ******************************************/
void Terminal::begin(HardwareSerial &app_uart_port, bool time_stamp_micros_enabled, bool enable_system_time)
{
    terminal_enabled = true;
    system_time_enabled = enable_system_time;
    terminal_uart_port = app_uart_port;
    time_stamp_micros = time_stamp_micros_enabled;
}

void Terminal::end()
{
    Terminal::printMessage(TerminalMessage("Terminal ended", "TER", INFO, micros()));
    terminal_enabled = false;
}

void Terminal::printMessage(TerminalMessage debug_message)
{
    if (terminal_enabled)
    {
        String temp_message;
        String temp_message_type;

        String temp_message_system = debug_message.system.substring(0, 4);

        switch (debug_message.type)
        {
        case INFO:
            temp_message_type = "INF";
            break;
        case WARNING:
            temp_message_type = "WAR";
            break;
        case ERROR:
            temp_message_type = "ERR";
            break;
        }

        temp_message += createTimeStamp(debug_message.time);

        temp_message += createProcessTimeStamp(debug_message.process_time);

        temp_message += " [";
        temp_message += debug_message.core;
        temp_message += "] [";
        temp_message += temp_message_type;
        temp_message += "] [";
        temp_message += temp_message_system;
        temp_message += "] - ";
        temp_message += debug_message.body;

        terminal_uart_port.println(temp_message);
    }
}

void Terminal::print(String message)
{
    if (terminal_enabled)
    {
        terminal_uart_port.print(message);
    }
}

void Terminal::println(String message)
{
    if (terminal_enabled)
    {
        terminal_uart_port.println(message);
    }
}

//*********************************************       PRIVATE FUNCTIONS DEFINTION       ********************************************/

String Terminal::createTimeStamp(unsigned long event_time)
{
    String temp_string;
    String temp_system_time;

    if (system_time_enabled)
    {
        // // -- Create time string
        // sprintf(system_time_buffer,
        //         "%04d-%02d-%02d - %02d:%02d:%02d.%3d", // 25 character String
        //         system_time->year,
        //         system_time->month,
        //         system_time->day,
        //         system_time->hours,
        //         system_time->minutes,
        //         system_time->seconds,
        //         system_time->milliseconds);

        // -- Create time string
        if (system_time != nullptr)
        {
            sprintf(system_time_buffer,
                    "%02d:%02d:%02d.%3d", // 25 character String
                    system_time->hours,
                    system_time->minutes,
                    system_time->seconds,
                    system_time->milliseconds);
        }
        else
        {
            sprintf(system_time_buffer,
                    "%02d:%02d:%02d.%3d", // 25 character String
                    0,
                    0,
                    0,
                    0);
        }

        temp_string += "[";
        temp_string += this->system_time_buffer;
        temp_string += "]";
    }

    // -- Time since system boot
    if (time_stamp_micros)
    {
        sprintf(this->time_stamp_buffer, "%14ld", event_time);
        temp_string += "[";
        temp_string += this->time_stamp_buffer;
        temp_string += " uS]";
    }
    else
    {
        sprintf(this->time_stamp_buffer, "%10ld", event_time / 1000);
        temp_string += "[";
        temp_string += this->time_stamp_buffer;
        temp_string += " mS]";
    }
    return temp_string;
}

String Terminal::createProcessTimeStamp(unsigned long process_time)
{
    String temp_message;

    if (process_time != 0)
    {
        if (time_stamp_micros)
        {
            sprintf(this->process_time_buffer, "%5ld", process_time);
            temp_message += "[+";
            temp_message += this->process_time_buffer;
            temp_message += " uS]";
        }
        else
        {
            sprintf(this->process_time_buffer, "%5ld", process_time / 1000);
            temp_message += "[+";
            temp_message += this->process_time_buffer;
            temp_message += " mS]";
        }
    }
    else
    {
        sprintf(this->process_time_buffer, "%5s", "");
        temp_message += "[ ";
        temp_message += this->process_time_buffer;
        temp_message += "   ]";
    }
    return temp_message;
}

// End.