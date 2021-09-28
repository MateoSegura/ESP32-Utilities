#pragma once
/* 
* Company: ANZE Suspension
* File Name: ble.h
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer a "easier-to" implement version of the BLE library in the Arduino framework. It offers
// * higher level of abstraction for implementing the "Client" & "Server" functionality.
// *
// * Great sources to learn more about Bluetooth Low Energy & how it interacts with the ESP:
// * https://punchthrough.com/serial-over-ble/
// * https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLETests/Arduino/BLE_uart/BLE_uart.ino
// *
// *
// * * Please refer to the example included with the library for usage.
// *
// *
// * * Future Improvements:
// *
// * Written by: Mateo Segura

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
#define MAX_MTU_SIZE 185 // * THIS IS THE LARGET MTU SIZE ALLOWS IN IOS

#define DEVICE_NAME "ESP32 BLE Server"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"             // Service UUID
#define CHARACTERISTIC_UUID_UART "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // UART Characteristic UUID

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>
#include <utils.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//*****************************************************       DATA TYPES        *****************************************************/

//*****************************************************        BLE CLASS        *****************************************************/
class BluetoothLowEnergyServer
{
public:
    // * Being bluetooth Server
    void begin(
        BLEServerCallbacks *callback, // Call back for read/write operations
        BLECharacteristicCallbacks *characteristic_callback);

    // * Start advertising
    void startAdvertising();

    // * For implementation in main app's callback
    void onClientConnect();
    void onClientDisconnect();

    // * Maximum MTU size setter
    ESP_ERROR setMaxMTUsize(uint16_t mtu_size);

    // * MTU size getter
    uint16_t getMTUsize();

    // * Server advertising status getter
    bool getServerAdvertisingStatus();

    // * Returns true if connected
    bool getConnectionStatus();

    // * Send data to client
    ESP_ERROR sendDataToClient(char *data);

private:
    // * Objects from Arduino implementation
    BLEServer *pServer = NULL;
    BLECharacteristic *pUARTcharacteristic;

    // * Connection manager variables
    bool isAdvertising = false;
    bool deviceConnected = false;
    bool oldDeviceConnected = false;

    // * Objects from Arduino implementation
    uint8_t txValue = 0;
    uint8_t client_mtu_size = MAX_MTU_SIZE;

    // * This controls the connection status
    void clientConnectionManager();
};

// End.
