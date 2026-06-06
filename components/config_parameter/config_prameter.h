#ifndef CONFIG_PARAMETER_H
#define CONFIG_PARAMETER_H

// config parameter for can protocol
#define CAN_TX 17
#define CAN_RX 16
#define BUFFER_SIZE_CAN_TX 1024
#define BUFFER_SIZE_CAN_RX 1024
// config parameter for sim module
#define UART_SIM_NUM UART_NUM_1
#define BUF_SIZE_SIM 1024
#define TX_SIM 37
#define RX_SIM 36
// config parameter for temp and humi
#define CAN_FRAME_ID_TEMP_HUMI 211
#define CAN_FRAME_ID_GAS 221
// config parameter for mqtt
#define MQTT_BROKER "mqtt://test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32_client"
#define MQTT_USER "username"
#define MQTT_PASSWORD "password"
#define MQTT_TOPIC "gateway/sub"
#define MQTT_TOPIC_PUB "gateway/pub"
#define MQTT_QOS 0
#define MQTT_RETAIN 0


#endif