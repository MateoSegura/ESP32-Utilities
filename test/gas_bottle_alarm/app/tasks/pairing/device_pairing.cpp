#include "device_pairing.h"
#include "../terminal/terminal.h"
#include "../bluetooth_server/ble_server.h"

void pairingTask(void *parameters)
{
    TerminalMessage pairing_debug_message;
    uint8_t debug_message_queue_ticks = 0;

    //* 1. Await binary semaphore
    xSemaphoreTake(app.start_pairing, portMAX_DELAY);

    pairing_debug_message = TerminalMessage("Starting pair up process", "PAIR", INFO, micros());
    addDebugMessageToQueue(&pairing_debug_message, debug_message_queue_ticks);

    //* 2. Give binary semaphore to bluetooth server task
    xSemaphoreGive(app.start_ble_server);

    //* 3. Wait for BLE instruction to be received.
    xSemaphoreTake(app.handle_ble_instruction, portMAX_DELAY);

    pairing_debug_message = TerminalMessage("BLE instruction received", "PAIR", INFO, micros());
    addDebugMessageToQueue(&pairing_debug_message, debug_message_queue_ticks);

    vTaskDelete(NULL);
}