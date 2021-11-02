/*
 * File Name: terminal_example.cpp
 * Hardware needed: No extra hardware needed, just a good ol' ESP32 dev. board
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

#include <Arduino.h>
#include <esp32_utilities.h>
#include <ACAN2517FD.h>

//******************** READ ME
//
// Simple example of how to use an object named "Arduino OTA feature". Read on...
//
// https://github.com/collin80/due_can/tree/master/examples
// https://github.com/collin80/due_can/blob/master/src/due_can.cpp // TODO: Auto Baud rate
//
// There's an RTOS version of this example under "examples/rtos/terminal_example_rtos.cpp"
//
// Mateo :)

//******************** SETTINGS
#define LED_BUILT_IN 2
#define DELAY_PERIOD_mS 1000 // TODO: Change this vaLUE between updates to see new sketch uploaded

// Terminal
#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false      // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

// CAN Controller pins
#define MCP2517_SCK 16 // 16
#define MCP2517_SDI 25 // 25
#define MCP2517_SDO 17 // 17
#define MCP2517_CS 27  // 30
#define MCP2517_INT 36 // 36

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;

ACAN2517FD can(MCP2517_CS, esp.hspi, MCP2517_INT);

//********************  METHODS
void testSendBandWidth();
void testReceiveBandwith();

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                               CAN FD Example                                             *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    // 3. Init HSPI bus
    esp.hspi.begin(MCP2517_SCK, MCP2517_SDO, MCP2517_SDI);

    // 4. Init CAN bus
    long initial_time = micros();
    ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, 1000 * 1000, DataBitRateFactor::x1);

    settings.mDriverReceiveFIFOSize = 200;

    const uint32_t error_code = can.begin(settings, []
                                          { can.isr(); });

    if (error_code != 0)
    {
        while (1)
        {
            terminal.printMessage(TerminalMessage("Could not initialize CAN controller. Check connections",
                                                  "WIF", ERROR, micros()));
            delay(500);
        }
    }

    terminal.printMessage(TerminalMessage("CAN controller initialized", "WIF", ERROR, micros(),
                                          micros() - initial_time));
}

void loop()
{
    testSendBandWidth();
    testReceiveBandwith();
    delay(100);
}

// Send definition:
void testSendBandWidth()
{
    long initial_time = millis();

    int messages_per_second = 0;
    int missed_messages = 0;
    double bandwith = 0;

    while (millis() < initial_time + 1000)
    {
        CANFDMessage frame;
        frame.id = 100;
        frame.len = 32;
        for (uint8_t i = 0; i < frame.len; i++)
        {
            frame.data[i] = i * 2;
        }
        bandwith = frame.len * messages_per_second;
        const bool ok = can.tryToSend(frame);
        if (ok)
        {
            terminal.printMessage(TerminalMessage("CAN Send Successful. Count: " + String(messages_per_second), "APP", INFO, micros()));
            messages_per_second++;
        }
        else
        {
            terminal.printMessage(TerminalMessage("Buffer Full", "APP", INFO, micros()));
            missed_messages++;
        }
    }

    terminal.printMessage(TerminalMessage("Messages per second:  " + String(messages_per_second),
                                          "CAN", INFO, micros(), micros() - initial_time));

    terminal.printMessage(TerminalMessage("Missed in 1s: " + String(missed_messages),
                                          "CAN", INFO, micros(), micros() - initial_time));

    terminal.printMessage(TerminalMessage("Bandwith in MB/s: " + String(bandwith / 1000),
                                          "CAN", INFO, micros(), micros() - initial_time));
}

// Receive definition
void testReceiveBandwith()
{
    long initial_time = millis();

    int messages_per_second = 0;
    double receive_bandwith = 0;

    while (millis() < initial_time + 1000)
    {
        CANFDMessage frame;
        TerminalMessage id_debug_message;
        TerminalMessage ext_id_debug_message;
        TerminalMessage data_debug_message;
        while (can.receive(frame))
        {
            // ID
            messages_per_second++;
            id_debug_message.body = "[ID:0x ";
            id_debug_message.body += String(frame.id);
            id_debug_message.body += " ";
            id_debug_message.body += String(HEX);
            id_debug_message.body += "] ";
            id_debug_message.type = INFO;
            id_debug_message.system = "CAN";
            id_debug_message.time = micros();
            terminal.printMessage(id_debug_message);

            // EXT ID
            if (frame.ext)
            {
                ext_id_debug_message.body = "[EXT:";
                ext_id_debug_message.body += "YES] ";
                ext_id_debug_message.type = INFO;
                ext_id_debug_message.system = "CAN";
                ext_id_debug_message.time = micros();
                terminal.printMessage(ext_id_debug_message);
            }
            else
            {
                ext_id_debug_message.body = "[EXT:";
                ext_id_debug_message.body += "NO] ";
                ext_id_debug_message.type = INFO;
                ext_id_debug_message.system = "CAN";
                ext_id_debug_message.time = micros();
                terminal.printMessage(ext_id_debug_message);
            }

            // DATA
            data_debug_message.body = " DATA = ";
            for (uint8_t i = 0; i < frame.len; i++)
            {
                data_debug_message.body += String(frame.data[i]);
                data_debug_message.body += " ";
            }
            data_debug_message.body += " ";
            data_debug_message.system = "CAN";
            data_debug_message.type = INFO;
            data_debug_message.time = micros();
            terminal.printMessage(data_debug_message);
        }
        receive_bandwith = frame.len * messages_per_second;
    }
    terminal.printMessage(TerminalMessage("Messages per second:  " + String(messages_per_second),
                                          "CAN", INFO, micros(), micros() - initial_time));

    terminal.printMessage(TerminalMessage("Bandwith in MB/s: " + String(receive_bandwith / 1000),
                                          "CAN", INFO, micros(), micros() - initial_time));
}