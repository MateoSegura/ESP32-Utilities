#pragma once

#include "gas_bottle_alarm/app/app.h"

void setupTerminal(void *parameters);
void addDebugMessageToQueue(TerminalMessage *message, uint16_t port_ticks = 0); // Add to terminal Queue
void addFileToPrintQueue(String &file_content);                                 // Add file to terminal Queue
