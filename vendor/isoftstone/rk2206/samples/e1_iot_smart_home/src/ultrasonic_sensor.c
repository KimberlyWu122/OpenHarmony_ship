#include "ultrasonic_sensor.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "los_tick.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define TRIG_GPIO GPIO0_PA4
#define ECHO_GPIO GPIO0_PA3

uint64_t GetMicros()
{
    return LOS_TickCountGet() * 1000ULL;
}

void UltrasonicSensorInit(void) {
    IoTGpioInit(TRIG_GPIO);
    IoTGpioSetDir(TRIG_GPIO, IOT_GPIO_DIR_OUT);

    IoTGpioInit(ECHO_GPIO);
    IoTGpioSetDir(ECHO_GPIO, IOT_GPIO_DIR_IN);
}

float UltrasonicSensorRead(void) {
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    IotGpioValue val = IOT_GPIO_VALUE0;
    const int timeout_us = 30000; // 超时30ms
    uint64_t timeMark = 0;

    // 触发信号：拉高10us
    IoTGpioSetOutputVal(TRIG_GPIO, IOT_GPIO_VALUE0);
    usleep(2);
    IoTGpioSetOutputVal(TRIG_GPIO, IOT_GPIO_VALUE1);
    usleep(10);
    IoTGpioSetOutputVal(TRIG_GPIO, IOT_GPIO_VALUE0);

    // 等待ECHO为高（上升沿）——带超时
    timeMark = GetMicros();
    while (1) {
        IoTGpioGetInputVal(ECHO_GPIO, &val);
        if (val == IOT_GPIO_VALUE1) break;
        if (GetMicros() - timeMark > timeout_us) {
            printf("[ULTRASONIC] 等待ECHO上升沿超时！\n");
            return -1.0f;
        }
    }
    startTime = GetMicros();

    // 等待ECHO为低（下降沿）——带超时
    timeMark = GetMicros();
    while (1) {
        IoTGpioGetInputVal(ECHO_GPIO, &val);
        if (val == IOT_GPIO_VALUE0) break;
        if (GetMicros() - timeMark > timeout_us) {
            printf("[ULTRASONIC] 等待ECHO下降沿超时！\n");
            return -1.0f;
        }
    }
    endTime = GetMicros();

    float duration = (float)(endTime - startTime); // us
    float distance = (duration * 0.0343f) / 2.0f; // cm

    printf("[ULTRASONIC] 距离: %.2f cm\n", distance);
    return distance;
}
