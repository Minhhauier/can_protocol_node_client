#include <stdint.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "can_protocol.h"
#include "isotp_user.h"

// RX 37->esp32 tx, TX 36
void app_main(void)
{
    if (twai_init_can() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize CAN");
        return;
    }
    create_queue_can_message();
    xTaskCreate(can_receive_task, "can_receive_task", 1024*4, NULL, 10, NULL);
    xTaskCreate(isotp_poll_task, "isotp_poll_task", 1024*4, NULL, 10, NULL);
    xTaskCreate(isotp_receive_task, "isotp_receive_task", 1024*4, NULL, 10, NULL);
    xTaskCreate(read_queue_can_message, "read_queue_can_message", 1024*4, NULL, 10, NULL);
    printf("Setup complete\n");
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    while (1)
    {
        send_temp_humi_message(25.0, 50.0);
        send_gas_message(100.0);
        // vTaskDelay(pdMS_TO_TICKS(500));
        // ret = isotp_user_send_can(0x203, data, 8);
        // if(ret != ISOTP_RET_OK){
        //     ESP_LOGE("MAIN", "Failed to send CAN");
        // }
        // vTaskDelay(pdMS_TO_TICKS(500));
        // ret = isotp_user_send_can(0x304, data, 8);
        // if(ret != ISOTP_RET_OK){
        //     ESP_LOGE("MAIN", "Failed to send CAN");
        // }
        // vTaskDelay(pdMS_TO_TICKS(500));
        // ret = isotp_user_send_can(0x405, data, 8);
        // if(ret != ISOTP_RET_OK){
        //     ESP_LOGE("MAIN", "Failed to send CAN");
        // }
        // vTaskDelay(pdMS_TO_TICKS(500));
        // ret = isotp_user_send_can(0x506, data, 8);
        // if(ret != ISOTP_RET_OK){
        //     ESP_LOGE("MAIN", "Failed to send CAN");
        // }
        // vTaskDelay(pdMS_TO_TICKS(500));
        // ret = isotp_user_send_can(0x607, data, 8);
        // if(ret != ISOTP_RET_OK){
        //     ESP_LOGE("MAIN", "Failed to send CAN");
        // }
        // vTaskDelay(pdMS_TO_TICKS(500));
        // ret = isotp_user_send_can(0x708, data, 8);
        // if(ret != ISOTP_RET_OK){
        //     ESP_LOGE("MAIN", "Failed to send CAN");
        // }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    
}
