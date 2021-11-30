/*
 * File Name: soc_example.cpp
 * Hardware needed: No extra hardware needed, just a ESP32 dev. board
 *
 * ESP32 Dev. Board: https://www.sparkfun.com/products/15663
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//******************** A QUICK NOTE
//
// Investing time to learn an SoC is a big commitment to make. Therefore, take this example with a grain
// of salt. It is meant to show you how to use certain features of the SoC using the "SystemOnChip" object,
// but it requires you to have intermediate knowledged of these interfaces, and how to utilize them to
// connect peripherals to your microcontroller.
//
// I recommend you spend a significant amount of time reading about these interfaces. If possible, get yourself
// an SPI peripheral, & an I2C peripheral, and code away!
//
// Do NOT be afraid to try things out. You will only learn how this stuff really works by doing it.
//
// Do NOT give up. This isn't an over-night process. Enjoy the journey!

#include <Arduino.h>
#include <esp32_utilities.h>

//******************** READ ME
//
// Simple example of how to use the "SystemOnChip" object.
//
// Although I am a big fan of the Arduino framework, I can sometimes get VERY confused
// when trying to implement internal hardware drivers for certain SoC. Let me explain...
//
// If I want to use the I2C0 hardware driver, I want to have an easy and straight away way of
// using such peripheral. In the Arduino world, someone decided to call it "TwoWire", and something
// inside of me just feels childish when using such wording in my code.
//
// Therefore, a simple class "SystemOnChip" was created. This library not only renames such Arduino
// objects to more "professional" names, BUT MOST IMPORTANTLY, it gives the programmer a list of the
// the available hardware in the specific SoC he/she istrying to program.
//
// This specific examples implements the following:
//
//  - UART
//  - I2C
//  - SPI
//  - Hardware timers
//  - ESPClass (from Arduino ESP-IDF)
//
// This library is mainly used for "housekeeping". Using this object also eases the process of
// porting application code to a different microcontroller, as the platform specific code for
// things like timers has been wrapped into simple functions, that are platform agnostic
//
// Mateo :)

//******************** SETTINGS

//* UART0
#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false      // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

//* I2C0
#define I2C0_SDA_PIN 21
#define I2C0_SCL_PIN 22
#define I2C0_FREQUENCY 400000 // KHz

//* VSPI
#define VSPI_MOSI_PIN 23
#define VSPI_MISO_PIN 19
#define VSPI_CLK_PIN 18
#define VSPI_CLK_FREQ_MHz 80

//* HSPI
#define HSPI_MOSI_PIN 25
#define HSPI_MISO_PIN 17
#define HSPI_CLK_PIN 16
#define HSPI_CLK_FREQ_MHz 40

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;

//******************** INTERRUPTS
static void IRAM_ATTR interruptExample()
{
    terminal.printMessage(TerminalMessage("Hello there from interrupt", "SOC", INFO, micros()));
}

//********************  SETUP
void setup()
{
    //* 1. How to init UART port
    //    ESP32 has 3 UART ports. The default port for programming & debbugging output from Espressif Systems is
    //    UART0, with TX in GPIO1, and RX in GPIO3. Max baud rate ir 3,000,000 (3Mb/s)
    //
    //    To learn more about UART refer to the links below:
    //    https://learn.sparkfun.com/tutorials/serial-communication/uarts
    //    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html
    //
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                       System on Chip Example                                             *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    //* 2. How to init I2C port
    //    ESP32 has 1 I2C driver, but since the bus has a very low clock frequency, you can implement a
    //    software emulated I2C driver.
    //
    //    This interface is mainly used for simple sensors with low data throughput.
    //
    //    To learn more about I2C refer to the links below:
    //    https://learn.sparkfun.com/tutorials/i2c/all
    //    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html
    //
    esp.i2c0.begin(I2C0_SDA_PIN, I2C0_SCL_PIN, I2C0_FREQUENCY);

    //* 3. How to init SPI port.
    //    ESP32 has 2 SPI ports available for use. They're refered to as HSPI & VSPI. These buses can be clocked up
    //    to 80MHz if using native pins, and 40 MHz if using the GPIO matrix to map these signals to other GPIOs.
    //
    //    Most classes will require you to pass a SPI port as well as a chip select pin, and sometimes and interrupt pin.
    //
    //    This interface is mainly used for high data throughput peripherals.
    //
    //    To learn more about SPI refer to the links below:
    //    https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi/all
    //    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html
    //
    esp.hspi.begin(HSPI_CLK_PIN, HSPI_MISO_PIN, HSPI_MOSI_PIN);
    esp.hspi.setFrequency(HSPI_CLK_FREQ_MHz * 1000000);

    esp.vspi.begin(VSPI_CLK_PIN, VSPI_MISO_PIN, VSPI_MOSI_PIN);
    esp.vspi.setFrequency(VSPI_CLK_FREQ_MHz * 1000000);

    //* 4. How to init a Hardware Timer.
    //    The ESP32 has 4 64-bit hardware timers.
    //
    //    The functions below simply wrap the ESP IDF function calls to setup the hardware timers
    //
    //    To learn more about the ESP32 hardware timers refer to the link below:
    //    https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
    //
    //
    esp.timer0.setup();
    esp.timer0.timerPeriodMilliseconds(1000, true); // (milliseconds, auto reload)
    esp.timer0.attachInterrupt(interruptExample);   // (function to execute when timer expires)
    esp.timer0.enableInterrupt();

    //* 5. SoC Utilities
    //     Espressif created a class named "ESPClass". It is mainly used to get information
    //     about the SoC, restart the SoC, etc.
    //
    //     This object can be used as "esp.utilities"
    //
    //     To fully understand the functionality of this object refer to the link below
    //     https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/Esp.h
    //
    uint8_t soc_cores = esp.utilities.getChipCores();
    terminal.printMessage(TerminalMessage("Number of cores in SoC: " + String(soc_cores), "SOC", INFO, micros()));
}

//********************  LOOP
void loop() {}

//* End