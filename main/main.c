#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "can_protocol.h"

void app_main(void)
{
    if (twai_init_can() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize CAN");
        return;
    }
    xTaskCreate(can_receive_task, "can_receive_task", 1024*4, NULL, 10, NULL);
    xTaskCreate(isotp_poll_task, "isotp_poll_task", 1024*4, NULL, 10, NULL);
    xTaskCreate(isotp_receive_task, "isotp_receive_task", 1024*4, NULL, 10, NULL);
    xTaskCreate(read_queue_can_message, "read_queue_can_message", 1024*4, NULL, 10, NULL);
    printf("Setup complete\n");
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}
