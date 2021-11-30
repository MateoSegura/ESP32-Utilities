/*
 * File Name: adc_example.cpp
 * Hardware needed: No extra hardware needed, just a good ol' ESP32 dev. board
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//******************** READ ME
//
// Simple example of how to use an object named "Terminal". Read on...
//
// There's an RTOS version of this example under "examples/rtos/terminal_example_rtos.cpp"
//
// Mateo :)

#include <Arduino.h>
#include <esp32_utilities.h>
#include <Adafruit_MCP4725.h>

//******************** SETTINGS
#define NUMBER_OF_CHANNELS 8

#define UART0_BAUD_RATE 2000000       // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED true // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false     // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

#define ADC_CS_PIN 32
#define VSPI_SDI_PIN 23
#define VSPI_SDO_PIN 19
#define VSPI_CLK_PIN 18
#define VSPI_CLK_FREQUENCY 25000000

QueueHandle_t debug_message_queue = NULL;
SemaphoreHandle_t debug_message_queue_mutex = NULL;
uint16_t debug_message_queue_length = 500;

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;
AD7689 adc;
Adafruit_MCP4725 dac;

struct AnalogReadings
{
    uint16_t adc_reading[NUMBER_OF_CHANNELS];
    uint16_t temp_reading;
};

void sampleADC(void *parameters);
void setupTerminal(void *parameters);

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                               AD7689 Example                                             *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    debug_message_queue = xQueueCreate(debug_message_queue_length, sizeof(AnalogReadings));
    debug_message_queue_mutex = xSemaphoreCreateMutex();

    if (debug_message_queue == NULL || debug_message_queue_mutex == NULL)
    {
        terminal.println("Could not create Terminal messages queue objects.");
    }

    // 3. Begin DAC for v_ref
    esp.i2c0.begin(22, 14);
    dac.begin(0x60, &esp.i2c0);
    // dac.setVoltage((4096 * 65535 / 5000), true);
    dac.setVoltage(3265, true);

    // 3. Begin SPI bus
    esp.vspi.begin(VSPI_CLK_PIN, VSPI_SDO_PIN, VSPI_SDI_PIN);
    esp.vspi.setFrequency(VSPI_CLK_FREQUENCY);

    // 4. Begin ADC
    long initial_time = micros();
    adc.begin(ADC_CS_PIN, esp.vspi, VSPI_CLK_FREQUENCY);

    if (adc.selftest() == false)
    {
        terminal.printMessage(TerminalMessage("Error initializing ADC", "ADC", ERROR, micros()));
        while (1)
            ;
    }

    disableCore0WDT();

    xTaskCreatePinnedToCore(sampleADC, "adc", 100000, nullptr, 1, NULL, 1);
    // xTaskCreatePinnedToCore(setupTerminal, "ter", 10000, nullptr, 1, NULL, 0);
}

void loop() {}

//********************  LOOP
void sampleADC(void *parameters)
{
    // long start_time = millis();
    // uint16_t analog_readings[NUMBER_OF_CHANNELS];
    // uint16_t temp_reading;

    // uint32_t samples = 0;

    AnalogReadings adc_readings;
    int i = 0;

    while (1)
    {
        uint32_t samples = 0;
        long initial_time = millis();
        while (millis() - initial_time <= 1000)
        {
            adc.readChannels(NUMBER_OF_CHANNELS, UNIPOLAR_MODE, adc_readings.adc_reading, &adc_readings.temp_reading);
            esp.uart0.println(String(adc_readings.adc_reading[0]));

            samples++;
            i++;
            if (i == 14999)
                i = 0;
        }
        // terminal.printMessage(TerminalMessage("\n\n" + String(samples) + "\n\n", "ADC", INFO, micros()));
    }
}

void setupTerminal(void *parameters)
{
    // Local Variables
    TerminalMessage debug_message;
    AnalogReadings adc_readings;

    while (1)
    {
        // * Print message to console
        if (xQueueReceive(debug_message_queue, (void *)&adc_readings, 0) == pdTRUE)
        {
            // for (int i = 0; i < NUMBER_OF_CHANNELS; i++)
            // {
            esp.uart0.println(String(adc_readings.adc_reading[0]));

            // esp.uart0.println();
        }
    }
}

// End.