#include "terminal.h"

void setupTerminal(void *parameters)
{
    // Local Variables
    TerminalMessage debug_message;
    String file_content;

    terminal.begin(&esp.uart0);

    terminal.printMessage(TerminalMessage("Terminal has been inititialized", "TER", INFO, micros()));

    while (1)
    {
        // * Print message to console
        if (xQueueReceive(app.rtos.terminal_message_queue, (void *)&debug_message, 1) == pdTRUE)
        {
            terminal.printMessage(debug_message);
        }

        // * Print file to console
        if (xQueueReceive(app.rtos.terminal_file_queue, (void *)&file_content, 1) == pdTRUE)
        {
            terminal.println("\n");
            terminal.print(file_content);
            terminal.println("\n\n");
        }
    }
}

void addDebugMessageToQueue(TerminalMessage *message, uint16_t port_ticks)
{
    xSemaphoreTake(app.rtos.terminal_message_queue_mutex, port_ticks);
    xQueueSend(app.rtos.terminal_message_queue, (void *)message, 0);
    xSemaphoreGive(app.rtos.terminal_message_queue_mutex);
}

void addFileToPrintQueue(String &file_content, uint16_t port_ticks)
{
    xSemaphoreTake(app.rtos.terminal_file_queue_mutex, port_ticks);
    xQueueSend(app.rtos.terminal_file_queue, (void *)&file_content, 0);
    xSemaphoreGive(app.rtos.terminal_file_queue_mutex);
}