#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/twai.h>
#include <string.h>
#include <esp_log.h>
#include <esp_err.h>

#include "config_prameter.h"
#include "isotp.h"

#define TAG "CAN_PROTOCOL"

IsoTpLink link;
static QueueHandle_t can_rx_queue = NULL;
static char buffer_can_tx[BUFFER_SIZE_CAN_TX];
static char buffer_can_rx[BUFFER_SIZE_CAN_RX];
static uint16_t app_len;

esp_err_t twai_init_can(void){
    twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = CAN_TX,
        .rx_io = CAN_RX,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 20,
        .rx_queue_len = 100,
        .alerts_enabled = 0,
        .clkout_divider = 0,
        .intr_flags = 0,
        .general_flags = {
            .sleep_allow_pd = 0,
        },
    };
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    ESP_LOGI(TAG, "Installing TWAI driver on GPIO27(TX)/GPIO36(RX)...");
    esp_err_t r = twai_driver_install(&g_config, &t_config, &f_config);
    if (r != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install TWAI: %s", esp_err_to_name(r));
        return r;
    }

    ESP_LOGI(TAG, "Starting TWAI...");
    r = twai_start();
    if (r != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start TWAI: %s", esp_err_to_name(r));
        return r;
    }
    // for(int i=0;i<10;i++){
    //     isotp_init_link(&node_link[i], 0x100 + i, node_send_buffer[i], BUFFER_SIZE_CAN_TX, node_receive_buffer[i], BUFFER_SIZE_CAN_RX);
    //     node_link[i].receive_arbitration_id = 0x200 + i; // Set receive ID for node1
    // }
    isotp_init_link(&link, 0x300, (uint8_t *)buffer_can_tx, BUFFER_SIZE_CAN_TX, (uint8_t *)buffer_can_rx, BUFFER_SIZE_CAN_RX);
    link.receive_arbitration_id = 0x400;
    ESP_LOGI(TAG, "✓ TWAI initialized successfully");
    return ESP_OK;
}

void isotp_send_message(IsoTpLink *link,const char *message, int len) {
   // printf("GỬI CHUỖI (%u bytes)\n", len);
  //   printf("Send: %s\n", message);
    
    int ret = isotp_send(link, (const uint8_t *)message, len);
    if (ret != ISOTP_RET_OK) {
        ESP_LOGE(TAG, "✗ isotp_send failed: %d", ret);
    }
    
    isotp_poll(link);
}

void isotp_poll_task(void *arg) {
    bool detect_can_process = false;
    while (1) 
    {   detect_can_process = false;
        if(link.send_status == ISOTP_SEND_STATUS_INPROGRESS || link.receive_status == ISOTP_RECEIVE_STATUS_INPROGRESS){
            isotp_poll(&link);
            detect_can_process = true;
        }
        if(detect_can_process){
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else{
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void can_receive_task(void *arg) {
    twai_message_t rx_msg;
    while (1) {
        esp_err_t r = twai_receive(&rx_msg, pdMS_TO_TICKS(10));
        if (r == ESP_OK) {
            if (xQueueSend(can_rx_queue, &rx_msg, 0) != pdTRUE) {
                ESP_LOGE(TAG, "Failed to enqueue received TWAI message");
            }
        } 
        taskYIELD(); // Yield to allow other tasks to run
    }
}

void read_queue_can_message(void *arg){
    twai_message_t rx_msg;
    while (1) {
        if (xQueueReceive(can_rx_queue, &rx_msg, pdMS_TO_TICKS(10)) == pdTRUE) {
            if (rx_msg.identifier == link.receive_arbitration_id) {
                isotp_on_can_message(&link, rx_msg.data, rx_msg.data_length_code);
            }
        }
        // taskYIELD(); // Yield to allow other tasks to run
    }
}

void isotp_receive_task(void *arg) {
    twai_message_t rx_msg;
    while (1) {
        int ret = isotp_receive(&link, (uint8_t *)buffer_can_rx, BUFFER_SIZE_CAN_RX, &app_len);
        if (ret == ISOTP_RET_OK) {
            buffer_can_rx[app_len] = '\0'; // Null-terminate the received message
            ESP_LOGI(TAG, "Received message: %s", buffer_can_rx);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Yield to allow other tasks to run
    }
}
