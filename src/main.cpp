/*
* File Name: main.cpp
* Project: ESP32 System on Module
* Version: 1.0
* Compartible Hardware: REV1.0
*/

#include <Arduino.h>
#include <esp32_utilities.h>
#include <MPU9250.h>

#define AD7689_CS_PIN 5

#define VSPI_MOSI_PIN 23
#define VSPI_MISO_PIN 19
#define VSPI_SCK_PIN 18
#define VSPI_FREQUENCY 200000 // 10 MHz.

SystemOnChip esp;
AD7689 ad7689(AD7689_CS_PIN, &esp.vspi, 8);

void setup()
{
  //* Terminal
  //* 1. initialize terminal
  esp.uart0.begin(115200);

  //* Begin SPI port
  esp.vspi.begin(VSPI_SCK_PIN, VSPI_MISO_PIN, VSPI_MOSI_PIN);

  // //* Begin ADC
  // ESP_ERROR initialize_adc = ad7689.begin(AD7689_CS_PIN, esp.vspi, VSPI_FREQUENCY);

  // if (initialize_adc.on_error) // Catch error
  // {
  //   esp.uart0.println(initialize_adc.debug_message);
  //   while (1)
  //     ; // Endless loop if not initialized correctly
  // }

  ad7689.acquireChannel(0);
  bool adc_self_test = ad7689.selftest();

  if (adc_self_test == false)
  {
    esp.uart0.println("Could not init ADC");
  }

  esp.uart0.println("ADC has been initialized");
}

void loop()
{
  Serial.println(ad7689.acquireChannel(0));
}