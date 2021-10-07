#pragma once

#include "gas_bottle_alarm/app/app.h"

void bleServerTask(void *parameters);
void addBluetoothTXMessageToQueue(String *bluetooth_message, uint16_t port_ticks);
