#include <stdio.h>
#include "iot_gpio.h"
#include "mux_selector.h"
#include "los_task.h"

#define usleep(x) LOS_Udelay((x))

// 定义你的 S0/S1/S2 对应的引脚
#define S0_PIN GPIO0_PB6
#define S1_PIN GPIO0_PA2
#define S2_PIN GPIO0_PB0


// 初始化 S0~S2 为输出模式
void MuxGpioInit(void)
{
    IoTGpioInit(S0_PIN);
    IoTGpioSetDir(S0_PIN, IOT_GPIO_DIR_OUT);

    IoTGpioInit(S1_PIN);
    IoTGpioSetDir(S1_PIN, IOT_GPIO_DIR_OUT);

    IoTGpioInit(S2_PIN);
    IoTGpioSetDir(S2_PIN, IOT_GPIO_DIR_OUT);

    printf("多路选择器 GPIO 初始化完成\n");
}

// 通道选择函数：0~7
void SelectAnalogChannel(uint8_t channel)
{
    IoTGpioSetOutputVal(S0_PIN, channel & 0x01);         // S0
    IoTGpioSetOutputVal(S1_PIN, (channel >> 1) & 0x01);   // S1
    IoTGpioSetOutputVal(S2_PIN, (channel >> 2) & 0x01);   // S2
    LOS_Msleep(10); 
}
