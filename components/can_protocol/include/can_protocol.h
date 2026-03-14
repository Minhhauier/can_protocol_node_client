#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H
#include <isotp.h>
#include <esp_err.h>
esp_err_t twai_init_can(void);
void isotp_send_message(IsoTpLink *link,const char *message, int len);
void isotp_poll_task(void *arg);
void can_receive_task(void *arg);
void read_queue_can_message(void *arg);
void isotp_receive_task(void *arg);

#endif