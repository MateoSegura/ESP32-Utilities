#include "save_data.h"
#include "../terminal/terminal.h"

String testSD_CardSpeed(uint16_t blocks, uint8_t *buffer);

void setupSaveDataTask(void *parameters)
{
    long initial_time = micros();
    TerminalMessage save_data_debug_message;
    uint8_t debug_message_queue_ticks = 0;

    //* 1. Initialize eMMC
    ESP_ERROR initialize_emmc = emmcMemory.begin(eMMC0_EN_PIN, eMMC0_CD_PIN, eMMC_MODE, MODE_1_BIT);

    if (initialize_emmc.on_error)
    {
        save_data_debug_message = TerminalMessage(initialize_emmc.debug_message,
                                                  "EMM", ERROR, micros(), micros() - initial_time);

        addDebugMessageToQueue(&save_data_debug_message);
        vTaskDelete(NULL);
    }

    save_data_debug_message = TerminalMessage("eMMC Initialized",
                                              "EMM", ERROR, micros(), micros() - initial_time);

    addDebugMessageToQueue(&save_data_debug_message);

    //* 2. Create data buffer & create file in sd card
    uint8_t buffer[512];

    for (int i = 0; i < 512; i++)
    {
        buffer[i] = 'a';
    }

    initial_time = micros();

    emmcMemory.writeFile("/write_speed_test.txt", buffer);

    save_data_debug_message = TerminalMessage("Write speed file created", // Message
                                              "MEM", INFO, micros(),      // System, message type, message timestamp,
                                              micros() - initial_time);   // process time

    addDebugMessageToQueue(&save_data_debug_message, debug_message_queue_ticks);

    //* 3. Loop saving 512 * blocks bytes.
    while (1)
    {
        initial_time = micros();

        String card_info = testSD_CardSpeed(1000, buffer);

        save_data_debug_message = TerminalMessage(card_info,                // Message
                                                  "MEM", INFO, micros(),    // System, message type, message timestamp,
                                                  micros() - initial_time); // process time

        addDebugMessageToQueue(&save_data_debug_message, debug_message_queue_ticks);
    }

    vTaskDelete(NULL);
}

double total_written_bytes = 0;

String testSD_CardSpeed(uint16_t blocks, uint8_t buffer[512])
{
    int write_blocks = blocks;
    uint8_t averages[write_blocks];

    long initial_time = micros();

    for (int i = 0; i < write_blocks; i++)
    {
        // * Second write without open
        long init_time = micros();

        emmcMemory.appendFile("/write_speed_test.txt", buffer);

        averages[i] = micros() - init_time;

        total_written_bytes = total_written_bytes + 512;
    }

    double sum = 0;

    for (int i = 0; i < write_blocks; i++)
    {
        sum += averages[i];
    }

    double avg_write_time = sum / (write_blocks * 1000);
    double avg_write_speed = (512 * 1000) / avg_write_time;

    String card_info;
    card_info += "Avg write time ->  ";
    card_info += avg_write_time;
    card_info += ". Write Speed -> ";
    card_info += String((double)avg_write_speed / 10000000);
    card_info += " MB/s. Total Bytes written -> ";
    card_info += String((double)total_written_bytes / 1000000, 3);
    card_info += " MB";

    return card_info;
}