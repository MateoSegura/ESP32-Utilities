/*
 * File Name: emmc_example.cpp
 * Hardware needed: A micro-sd card module that exposes all the signals, and an ESP32 dev. board
 *
 * u-SD Card Module: https://www.sparkfun.com/products/9419
 * ESP32 Dev. Board: https://www.sparkfun.com/products/15663
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

#include <Arduino.h>
#include <esp32_utilities.h>

//******************** READ ME
//
// Simple example of how to use the "EMMC_Memory" object.
//
// If you want to save data to an external SD card, this library make's it easier to implement
// a lot of the overhead code that comes when trying to manage initialization as well as error
// handling for the file system r/w operations
//
// You can use this module in eMMC 1-bit mode, eMMC 4-bit mode, as well as regular SPI. This
// example simply polls for the CD pin if no card is inserted at boot. A more elegant way of
// handling this event, is by using RTOS. Refer for "examples/rtos/04_emmc_rtos_example.cpp"
//
// If you want to read more about this topic. refer to the links below:
//
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdmmc_host.html
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdspi_host.html
//
// I personally recommend you connect the SD card in 1-bit mode, as it's the perfect trade off
// between r/w speeds, and number of pins required. You also don't have to care about the problem
// that occurs when you try to do an Over The Air Update while connected in 4-bit emmc mode.
//
// Mateo :)

//******************** R/W TESTS CONFIGURATION
#define BLOCKS_TO_WRITE 1000 // Blocks of 512 bytes
#define BLOCKS_TO_READ 1000  // Blocks of 512 bytes

//******************** SETTINGS

#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false      // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

#define EMMC_MODE true   // Set to false to use SPI
#define _4_BIT_MODE true // Set to use card in 4-bit mode

#define CD_PIN 13 // Card Detect pin
#define EN_PIN -1 // Enable pin

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;
EMMC_Memory emmc;

//******************** INTERRUPTS

//********************  METHODS
void handleCardDetectPinChange();
void runWriteTest();
void runReadTest(); // TODO: Implement read test

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                            eMMC Example                                                  *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    // 3. Init eMMC
    long initial_time = micros();
    ESP_ERROR initialize_emmc = emmc.begin(EN_PIN, CD_PIN, eMMC_MODE, MODE_1_BIT);

    if (initialize_emmc.on_error)
    {
        terminal.printMessage(TerminalMessage(initialize_emmc.debug_message, "MMC", ERROR, micros()));
    }
    else
    {
        terminal.printMessage(TerminalMessage("eMMC initialized", "MMC", INFO, micros(),
                                              micros() - initial_time)); // How long did SD card take to init
    }
}

//********************  LOOP
void loop()
{
    // 1. Handle CD pin events
    handleCardDetectPinChange();

    // 2. If initialized run the tests
    if (emmc.isInitialized())
    {
        // 2.1 Run a small test that saves 5Mb of data
        runWriteTest();
        runReadTest();
    }
}

//********************  METHOD DECLARATION
void runWriteTest()
{
    terminal.printMessage(TerminalMessage("Running write speed test ...", "MMC", INFO, micros()));

    // 1. Create fake data
    uint8_t data[512];

    for (int i = 0; i < 512; i++)
    {
        data[i] = random(0, 255);
    }

    // 2. Initial write to create file
    ESP_ERROR create_file = emmc.writeFile("/write_test.txt", data);
    if (create_file.on_error)
    {
        terminal.printMessage(TerminalMessage(create_file.debug_message, "MMC", ERROR, micros()));
        while (1)
            ;
    }

    // 3. Attempt to write
    long initial_time = micros();
    double write_times_sum = 0;

    for (int i = 0; i < (BLOCKS_TO_WRITE - 1); i++)
    {
        long initial_write_time = micros();
        emmc.appendFile("/write_test.txt", data);

        write_times_sum += micros() - initial_write_time;
    }

    // 4. Display stats to user
    terminal.printMessage(TerminalMessage("Writing test done: ", "MMC", INFO, micros(), micros() - initial_time));
    esp.uart0.println();

    double avg_write_time = write_times_sum / (BLOCKS_TO_WRITE * 1000);
    terminal.println("Average write time: " + String(avg_write_time, 2) + "mS");

    long bytes_written = 512 * BLOCKS_TO_WRITE;
    terminal.println("Number of bytes written: " + String(bytes_written) + " bytes");

    double avg_write_speed = (512 * BLOCKS_TO_WRITE) / avg_write_time;
    terminal.println("Average write speed: " + String((avg_write_speed / 1000000), 2) + " MB/s");

    esp.uart0.println();
}

void runReadTest()
{
}

void handleCardDetectPinChange()
{
    // 1. On insert
    if (digitalRead(CD_PIN) == LOW && !emmc.isInitialized())
    {
        long initial_time = micros();
        delay(50); // Give the SD some time to initialize
        ESP_ERROR initialize_emmc = emmc.begin(EN_PIN, CD_PIN, eMMC_MODE, MODE_1_BIT);

        if (initialize_emmc.on_error)
        {
            while (1)
            {
                terminal.printMessage(TerminalMessage(initialize_emmc.debug_message, "MMC", ERROR, micros()));
                delay(1000);
            }
        }
        else
        {
            terminal.printMessage(TerminalMessage("eMMC initialized", "MMC", INFO, micros(),
                                                  micros() - initial_time)); // How long did SD card take to init
        }
    }

    // 2. On remove
    if (digitalRead(CD_PIN) == HIGH && emmc.isInitialized())
    {
        emmc.setDetected(false);
        terminal.printMessage(TerminalMessage("eMMC removed", "MMC", WARNING, micros()));
    }
}

// End.