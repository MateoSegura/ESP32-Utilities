/*
 * File Name: rtc_example.cpp
 * Hardware needed: One of the following modules, or any dev. board with these ICs
 *
 * RV8803: https://www.sparkfun.com/products/16281
 * RV3027: https://www.sparkfun.com/products/15486
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

#include <Arduino.h>
#include <esp32_utilities.h>

//******************** READ ME
//
// Simple example of how to use the "RealTimeClock" object.
//
// If your project requires you to keep track of time while powered off, a real time clock
// is fantastic for doing just that.
//
// The chips chosen have very attractive features for a relative low-price. Where these chips
// really stand out is with their ultra small footprint, and their current consumption (40nA)
//
// They also have programmable interrupts & clock outputs.
//
// The driver library for each RTC chip is a port of Sparkfun's work, simply adapted to the
// syntax & functionality expected from the ESP32 Utilities Libraries.
//
// Both chips can be connected through an I2C bus, at 400 KHz max.
//
// Mateo :)

//******************** SETTINGS

#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED true       // Set to true if using a real time clock, and we are!

#define RTC_IC RealTimeClock::RV8803_IC
// #define RTC_IC RealTimeClock::RV3027_IC

#define I2C0_SDA_PIN 21
#define I2C0_SCL_PIN 22
#define I2C0_FREQUENCY 400000 // KHz

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;

RealTimeClock rtc;    // RTC object
DateTime system_time; // DateTime object

//******************** INTERRUPTS
// Period millisecond hardware timer to keep system time after initialized
static void IRAM_ATTR updateTime()
{
    rtc.updateMillisecondsCounter(system_time);
}

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                      Real Time Clock Example                                             *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);
    terminal.setTimeKeeper(system_time); // Takes in the DateTime object of your app if SYSTEM_TIME_ENABLED is true

    // 3. Init I2C bus
    esp.i2c0.begin(I2C0_SDA_PIN, I2C0_SCL_PIN, I2C0_FREQUENCY); // Refer to soc_example.cpp for information on this function

    // 4. Init real time clock
    long initial_process_time = micros();                   // For process time measuring
    ESP_ERROR initialize_rtc = rtc.begin(RTC_IC, esp.i2c0); // begin(IC_TYPE, I2C port)

    if (initialize_rtc.on_error) // Catch error
    {
        while (1)
        {
            esp.uart0.println("Could not initialize real time clock. Check connections!");
            delay(500);
        }
    }

    terminal.printMessage(TerminalMessage("Real time clock initialized", "RTC", INFO, micros(), micros() - initial_process_time));

    // 5. Set to compiler time
    rtc.setToCompilerTime();

    // 6. Get time from rtc
    system_time = rtc.getTime();

    terminal.printMessage(TerminalMessage("System Time: " + system_time.toString(), "RTC", INFO, micros()));

    // 7. Set a Hardware Timer with 1 ms period for accurate timing. Usual drift is about 1 second/5 hours
    //
    //    Refer to "soc_example.cpp" for information on how to utilize the hardware timers
    //

    esp.timer0.setup();
    esp.timer0.attachInterrupt(updateTime);
    esp.timer0.timerPeriodMilliseconds(1); // in milliseconds
    esp.timer0.enableInterrupt();
}

//********************  LOOP
void loop()
{
    terminal.printMessage(TerminalMessage("Current Date & Time: " + system_time.toString(true), "RTC", INFO, micros()));
    delay(100);
}

//* End