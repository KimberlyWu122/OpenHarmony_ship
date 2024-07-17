/*
 * Copyright (c) 2024 iSoftStone Education Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include "los_task.h"
#include "ohos_init.h"
#include "iot_errno.h"
#include "iot_gpio.h"

/* GPIO按键IO口 */
#define GPIO_KEY_UP         GPIO0_PC7
// #define GPIO_KEY_DOWN       GPIO0_PA3
// #define GPIO_KEY_LEFT       GPIO0_PA3
// #define GPIO_KEY_RIIGHT     GPIO0_PA3

/* 记录中断触发次数 */
static unsigned int m_gpio_interrupt_count = 0;

/***************************************************************
* 函数名称: gpio_isr_func
* 说    明: gpio中断响应处理函数
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void gpio_isr_func(void *args)
{
    m_gpio_interrupt_count++;
}

#define PRESSED     1
#define NO_PRESSED  0

/***************************************************************
* 函数名称: gpio_process
* 说    明: gpio任务
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void gpio_process()
{
    unsigned int ret;

    /* 初始化引脚为GPIO */
    IoTGpioInit(GPIO_KEY_UP);
    /* 引脚配置为输入 */
    IoTGpioSetDir(GPIO_KEY_UP, IOT_GPIO_DIR_IN);

    int is_pressed = NO_PRESSED;
    
    while (1)
    {
        IotGpioValue val;
        IoTGpioGetInputVal(GPIO_KEY_UP, &val);
        if((val == 0) &&(is_pressed == NO_PRESSED)) {

            //消除抖动
            LOS_Msleep(10);
            IoTGpioGetInputVal(GPIO_KEY_UP, &val);
            if(val == 0){
                is_pressed = PRESSED;
                printf("pressed\n");
            }
        }else if((val == 1) &&(is_pressed == PRESSED)){
                is_pressed = NO_PRESSED;
                printf("no pressed\n");
        }
        
        /* 睡眠1秒 */
        LOS_Msleep(50);
    }
}


/***************************************************************
* 函数名称: gpio_interrupt_example
* 说    明: 开机自启动调用函数
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void key_scan_example()
{
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};
    unsigned int ret = LOS_OK;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)gpio_process;
    task.uwStackSize = 2048;
    task.pcName = "gpio process";
    task.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK)
    {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }
}

APP_FEATURE_INIT(key_scan_example);