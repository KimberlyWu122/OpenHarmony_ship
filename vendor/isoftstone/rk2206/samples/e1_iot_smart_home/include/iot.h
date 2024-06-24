#ifndef _IOT_H_
#define _IOT_H_

#include <stdbool.h>

typedef struct
{
    double illumination;
    double temperature;
    double humidity;
    bool motor_state;
    bool light_state;
    bool auto_state;
} e_iot_data;

int wait_message();
void mqtt_init();
unsigned int mqtt_is_connected();
void send_msg_to_mqtt(e_iot_data *iot_data);

#endif // _IOT_H_