/*
 * File Name: ble_uart_example.cpp
 * Hardware needed: Any ESP32 dev. board
 *
 * ESP32 Dev. Board: https://www.sparkfun.com/products/15663
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

#include <Arduino.h>
#include <esp32_utilities.h>

//******************** READ ME
//
// Simple example of how to use the "BluetoothLowEnergyServer" object.
//
// This class simply wraps up the Arduino BLE library made by Neil Kolban
// into simple to use & repetable code.
//
// From experience, it's been easier to implement a simple API, and communicate
// using a "UART" interface over bluetooth, than to try to add all of these
// fancy services & characteristics.
//
// I'm sure there are applications where this isn't the best solution, but for
// our applications, the app & the device are designed to be paired &
// transfer all sort of information such as files, commands, & settings.
//
// The max throughput is around ~11KB/s (limited by the iOS MTU size
// of 185 bytes). If using Android, you can send up 512 bytes if the MTU size
// is negotiated, and increase it to ~30.7 KB/s
//
// This example has also been implemented using RTOS, and you can find it under
// "examples/rtos/ble_server_rtos_example".
//
// If you want to find out more about bluetooth low energy, follow the links below:
//
// https://www.novelbits.io/basics-bluetooth-low-energy/
// https://github.com/nkolban/esp32-snippets/tree/master/Documentation
//
// Mateo :)

//******************** SETTINGS

#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false      // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

// BLE Service
#define BLE_NAME "Bluetooth Server Example"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;
BluetoothLowEnergyServer bleServer;

class MyServerCallbacks : public BLEServerCallbacks
{
    // -- On client connect
    void onConnect(BLEServer *pServer)
    {
        bleServer.onClientConnect();
        terminal.printMessage(TerminalMessage("Client connected", "BLE", INFO, micros()));
    };

    // -- On client disconnect
    void onDisconnect(BLEServer *pServer)
    {
        bleServer.onClientDisconnect();
        terminal.printMessage(TerminalMessage("Client disconnected", "BLE", INFO, micros()));

        delay(200); // Allow some time for the server to stat advertising again
        bleServer.startAdvertising();
        terminal.printMessage(TerminalMessage("Bluetooth server is advertising", "BLE", INFO, micros()));
    }
} ServerCallbacks;

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        String incoming_bluetooth_message = "Incoming data (size: ";
        incoming_bluetooth_message += rxValue.length();
        incoming_bluetooth_message += ") -> ";

        // Store incoming message in string
        if (rxValue.length() > 0)
        {
            for (int i = 0; i < rxValue.length(); i++)
            {
                incoming_bluetooth_message += rxValue[i];
                // TODO: Handle your incoming data as you wish here
            }
            terminal.printMessage(TerminalMessage(incoming_bluetooth_message, "BLE", INFO, micros()));
        }
    }

} CharacteristicCallbacks;

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                      Bluetooth Server Example                                            *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    // 3. Init bluetooth server
    long initial_time = micros();
    bleServer.begin(BLE_NAME,
                    SERVICE_UUID,
                    CHARACTERISTIC_UUID,
                    &ServerCallbacks,
                    &CharacteristicCallbacks);

    bleServer.startAdvertising();

    terminal.printMessage(TerminalMessage("Bluetooth server is advertising", "BLE", INFO, micros(), micros() - initial_time));

    // 4. Set MTU size. Max iOS size is 185 bytes (11KB/s)
    ESP_ERROR mtu_set = bleServer.setMaxMTUsize(185); // -- Try to negotiate Max size MTU (iOS max. MTU is 185 bytes)

    if (mtu_set.on_error) // Catch error
    {
        terminal.printMessage(TerminalMessage(mtu_set.debug_message, "MMC", ERROR, micros()));
    }
    else
    {
        terminal.printMessage(TerminalMessage("MTU size set to 185 bytes. Max throughput is ~ 11KB/s", "MMC", INFO, micros()));
    }
}

//********************  LOOP
void loop()
{
    delay(1000);
    bleServer.sendDataToClient("Hello there from esp!");
}

// End.