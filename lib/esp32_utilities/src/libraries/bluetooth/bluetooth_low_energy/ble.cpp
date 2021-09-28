/* 
* Company: ANZE Suspension
* File Name: ble.cpp
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*****************************************************       HEADER FILE       *****************************************************/

#include "ble.h"

//*****************************************       BLE SERVER CLASS FUNCTIONS DEFINTION       ****************************************/

// * Begin
void BluetoothLowEnergyServer::begin(BLEServerCallbacks *callback, BLECharacteristicCallbacks *characteristic_callback)
{
    // Create the BLE Device
    BLEDevice::init(DEVICE_NAME);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(callback);

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pUARTcharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_UART,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);

    pUARTcharacteristic->addDescriptor(new BLE2902());

    pUARTcharacteristic->setCallbacks(characteristic_callback);

    // Start the service
    pService->start();
}

// * Start Advertising
void BluetoothLowEnergyServer::startAdvertising()
{
    pServer->getAdvertising()->start();
    isAdvertising = true;
}

// * On connect
void BluetoothLowEnergyServer::onClientConnect()
{
    deviceConnected = true;
    clientConnectionManager();
}

// * On disconnect
void BluetoothLowEnergyServer::onClientDisconnect()
{
    deviceConnected = false;
    clientConnectionManager();
}

// * Maximum MTU size setter
ESP_ERROR BluetoothLowEnergyServer::setMaxMTUsize(uint16_t mtu_size)
{
    ESP_ERROR err;
    err.on_error = false;

    if (mtu_size > client_mtu_size)
    {
        err.on_error = true;
        err.debug_message = "Biggest MTU size you can use is 185 bytes. About 11kb/s";
    }
    else
    {
        BLEDevice::setMTU(mtu_size);

        if (BLEDevice::getMTU() != mtu_size)
        {
            err.on_error = true;
            err.debug_message = "Could not set MTU size";
        }
    }

    return err;
}

// * MTU size getter
uint16_t BluetoothLowEnergyServer::getMTUsize()
{
    return BLEDevice::getMTU();
}

// * Server advertising status getter
bool BluetoothLowEnergyServer::getServerAdvertisingStatus()
{
    return isAdvertising;
}

// * Connection status getter
bool BluetoothLowEnergyServer::getConnectionStatus()
{
    if (deviceConnected)
    {
        return true;
    }
    return false;
}

// * Send data to client
ESP_ERROR BluetoothLowEnergyServer::sendDataToClient(char *data)
{
    ESP_ERROR err;
    err.on_error = false;

    if (deviceConnected)
    {
        if (sizeof(data) > client_mtu_size)
        {
            err.on_error = true;
            err.debug_message = "Data size is bigger than set MTU packet size";
        }
        else
        {
            pUARTcharacteristic->setValue((char *)data);
            pUARTcharacteristic->notify();
        }
    }

    return err;
}

// * Manager internal object status
void BluetoothLowEnergyServer::clientConnectionManager()
{ // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        isAdvertising = false;
        oldDeviceConnected = deviceConnected;
    }
}
