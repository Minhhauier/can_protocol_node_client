#include <stdio.h>
#include <stdlib.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <string.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "uart.h"
#include "config_prameter.h"
#include "at_command.h"
#include "encrypt_decrypt.h"
#include "mqtt.h"
// #include "control_relay.h"

QueueHandle_t sim_at_queue_handle;
QueueHandle_t mqtt_queue_handle;
QueueHandle_t gps_queue_handle;
QueueHandle_t publish_queue_handle;
int queue_created = 0;
static char data[BUF_SIZE_SIM];
void send_at_get_respond(char *cmd, int timeout)
{
    ESP_LOGI("SIM", "sent: %s", cmd);
    uart_write_bytes(UART_SIM_NUM, cmd, strlen(cmd));
    uart_write_bytes(UART_SIM_NUM, "\r\n", 2);
    int total_len = 0;
    int count = 0;
    for (int i = 0; i < timeout; i += 100)
    {
        int len = uart_read_bytes(UART_SIM_NUM, data + total_len, BUF_SIZE_SIM - total_len - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            total_len = total_len + len;
            count = 0;
        }
        else
            count++;
        if (count > 3)
            break;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    if (total_len > 0)
    {
        data[total_len] = '\0';
      // printf("RX_sent_at: %s\r\n", data);
    }
}

void send_at(char *cmd)
{
    ESP_LOGI("SIM","sent: %s",cmd);
    uart_write_bytes(UART_SIM_NUM, cmd, strlen(cmd));
    uart_write_bytes(UART_SIM_NUM, "\r\n", 2);
}
char *get_respond(int timeout)
{
    int total_len = 0;
    int count = 0;
    for (int i = 0; i < timeout; i += 100)
    {
        int len = uart_read_bytes(UART_SIM_NUM, data + total_len, BUF_SIZE_SIM - total_len - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            total_len = total_len + len;
            count = 0;
        }
        else
            count++;
        if (count > 3)
            break;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    if (total_len > 0)
    {
        data[total_len] = '\0';
       // printf("RX: %s\r\n", data);
        char *data_new = calloc(total_len + 1, sizeof(char));
        strcpy(data_new, data);
        return data_new;
    }
    return NULL;
}
void copy_respond_to_array(char *cmd,char *array){
    send_at(cmd);
    char *data = get_respond(1000);
    if(strcmp(cmd,"AT+CCLK?")==0 && data!=NULL){
        char *start = strchr(data,'"');
        char *end = strrchr(data,'"');
        if(start!=NULL && end!=NULL && end>start){
            int len = end - start - 1;
            if(len<64){
                memcpy(array,start+1,len);
                array[len]='\0';
            }
        }
        // printf("Date time: %s\r\n",array);
    }
    else if(data != NULL) strcpy(array,data);
    // strcpy(array,data);
    free(data);
}
void init_queues() {
    sim_at_queue_handle = xQueueCreate(10, BUF_SIZE_SIM);
    mqtt_queue_handle = xQueueCreate(10, BUF_SIZE_SIM);
    gps_queue_handle = xQueueCreate(10, BUF_SIZE_SIM);
    publish_queue_handle = xQueueCreate(10, BUF_SIZE_SIM);
    queue_created = 1;
}
void read_and_send_to_queue_task(void *pvParameters)
{
    char *data_receiver=malloc(2048);
    char data_copy[2048];
   // printf("Start read_and_send_to_queue_task\r\n");
    while (1)
    {
        if (read_enable)
        {
            int len = uart_read_bytes(UART_SIM_NUM, data_receiver, 2048, 30 / portTICK_PERIOD_MS);
            if (len > 0)
            {
                data_receiver[len] = '\0';
               // printf("data_rx: %s\r\n",data_receiver);
                if (strstr(data_receiver, "+QMTRECV:") != NULL)
                {
                    memcpy(data_copy,data_receiver,len+1);
              
                  //   printf("data_rx: %s\r\n",data_receiver);
                     convert_to_json_update(data_copy);
                 //   printf("Send to mqtt queue: %s\r\n", data_receiver);
                  //  xQueueSend(mqtt_queue_handle, data_receiver, portMAX_DELAY);
                }
                else if (strstr(data_receiver, "+QGPSLOC:") != NULL)
                    xQueueSend(gps_queue_handle, data_receiver, portMAX_DELAY);
                else
                    xQueueSend(sim_at_queue_handle, data_receiver, portMAX_DELAY);
            }
        }
        else
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    free(data_receiver);
}


