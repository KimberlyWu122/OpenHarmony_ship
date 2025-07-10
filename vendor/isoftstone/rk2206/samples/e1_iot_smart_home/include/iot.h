#ifndef _IOT_H_
#define _IOT_H_

#include <stdbool.h>
#include "iot_command.h"

// IoT 传感器数据结构
typedef struct {
    double illumination;
    double temperature;
    double humidity;
    bool motor_state;
    bool light_state;
    bool auto_state;
    float tds_value;
    float ph_value;       
    float turbidity;
    float ultrasonic;
    float latitude;
    float longitude;
} e_iot_data;


// 函数声明
int wait_message();
void mqtt_init();
unsigned int mqtt_is_connected();
void send_msg_to_mqtt(e_iot_data *iot_data);

#endif  // _IOT_H_