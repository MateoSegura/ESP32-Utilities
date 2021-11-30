/*
 * File Name: ota_update_example.cpp
 * Hardware needed: No extra hardware needed, just a ESP32 dev. board
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//******************** READ ME
//
// Simple example of how to use an object named "Arduino OTA feature". Read on...
//
// The first time the firmware is flashed to the device has to be through the UART0 port,
// using your USB cable & a USB-UART bridge. Most development boards already have this
//
// If your network details are correct, then proceed to adding the following lines to your
// "platform.ini" file
//
//      upload_protocol = espota
//      upload_port = 10.0.0.xxx ; Your IP address
//
// Change the period to see how the LED period changes between updates
//
// If you want to learn more about over the air updates, the links below are a great
// place to start
//
// https://www.youtube.com/watch?v=1pwqS_NUG7Q
// https://lastminuteengineers.com/esp32-ota-updates-arduino-ide/
//
// There's an RTOS version of this example under "examples/rtos/terminal_example_rtos.cpp"
//
// Mateo :)

#include <Arduino.h>
#include <esp32_utilities.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

//******************** SETTINGS
#define LED_BUILT_IN 2
#define DELAY_PERIOD_mS 1000 // TODO: Change this vaLUE between updates to see new sketch uploaded

// Terminal
#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false      // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

// WiFi
#define WIFI_SSID "my_network_name"          // TODO: Add network name
#define WIFI_PASSWORD "my_network_password"  // TODO: Add network password
#define WIFI_CONNECTION_ATTEMPT_TIMEOUT_S 10 // WiFi will stop trying to connect after this time

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;

//********************  METHODS
bool connectToWiFi();
void setupOTA();

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                              Terminal Example                                            *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    // 3. Init WiFi
    bool connect_wifi = connectToWiFi();

    if (!connect_wifi) // Catch error
    {
        while (1)
        {
            terminal.printMessage(TerminalMessage("Check your SSID & Password", "WIF", WARNING, micros()));
            delay(500);
        }
    }

    // 4. If connected succesfully, setup Over the Air Updates
    setupOTA();

    // 5. Setup LED PIN as output
    pinMode(LED_BUILT_IN, OUTPUT);
}

//********************  LOOP
bool led_state;

void loop()
{
    ArduinoOTA.handle();

    // Update LED
    led_state = !led_state;
    digitalWrite(LED_BUILT_IN, led_state);
    delay(DELAY_PERIOD_mS);
}

bool connectToWiFi()
{
    long initial_time = micros();

    // 1. Begin WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    terminal.printMessage(TerminalMessage("Attempting WiFi connection ..., this might take some time", "WIF", INFO, micros()));

    // 2. Attempt connection
    while (WiFi.status() != WL_CONNECTED && micros() - initial_time < WIFI_CONNECTION_ATTEMPT_TIMEOUT_S * 1000000)
    {
        delay(30 / portTICK_PERIOD_MS);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        terminal.printMessage(TerminalMessage("Wifi Connection Timeout", "WIF", ERROR, micros()));
        return false;
    }

    // 3. Set WiFi station
    WiFi.mode(WIFI_STA);

    // 4. Display local IP to terminal & return
    terminal.printMessage(TerminalMessage("WiFi connection successful. IP address: " + WiFi.localIP().toString(), "WIF", INFO, micros(), micros() - initial_time));

    return true;
}

void setupOTA()
{
    ArduinoOTA
        .onStart([]()
                 {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      terminal.printMessage(TerminalMessage("Start updating " + String(type),"WIF",INFO,micros())); })
        .onEnd([]()
               { terminal.printMessage(TerminalMessage("End.", "WIF", INFO, micros())); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { terminal.printMessage(TerminalMessage("Updating", "WIF", INFO, micros())); })
        .onError([](ota_error_t error)
                 {
      terminal.printMessage(TerminalMessage("Error: ", "WIF", ERROR, micros()));
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

    ArduinoOTA.begin();
}
// End.