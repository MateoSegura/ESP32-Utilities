#include "main_app.h"
#include "../terminal/terminal.h"

void startMainApp(void *parameters)
{
    TerminalMessage pairing_debug_message;
    uint8_t debug_message_queue_ticks = 0;

    //* 1. Await binary semaphore
    xSemaphoreTake(app.rtos.start_main, portMAX_DELAY);

    pairing_debug_message = TerminalMessage("Starting main app", "MAI", INFO, micros());
    addDebugMessageToQueue(&pairing_debug_message, debug_message_queue_ticks);

    //* 2. 

    vTaskDelete(NULL);
}