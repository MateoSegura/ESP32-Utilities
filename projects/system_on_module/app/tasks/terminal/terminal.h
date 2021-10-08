#pragma once
/*
* File Name: terminal.h
* Project: Gas Bottle Alarm
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: Oct 8, 2021
* Copyright 2021, Mateo Segura, All rights reserved.
*/

#include "system_on_module/app/app.h"

void setupTerminal(void *parameters);                                           // Create Terminal Tasks
void addDebugMessageToQueue(TerminalMessage *message, uint16_t port_ticks = 0); // Add to terminal Queue
void addFileToPrintQueue(String &file_content);                                 // Add file to terminal Queue

//* End.