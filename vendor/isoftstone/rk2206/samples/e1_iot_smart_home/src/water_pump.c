#include "water_pump.h"
#include <stdio.h>
#include <unistd.h>
#include "iot_gpio.h"
#include "iot_errno.h"
#include "los_task.h"
#include "los_queue.h"

#define IOT_CMD_PUMP 6
#define PUMP_QUEUE_LEN 4
UINT32 g_pumpQueueId;

#define PUMP_RELAY_GPIO GPIO0_PA5
#define PUMP_DURATION_SECONDS 10

void PumpInit(void) {
    IoTGpioInit(PUMP_RELAY_GPIO);
    IoTGpioSetDir(PUMP_RELAY_GPIO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(PUMP_RELAY_GPIO, 1);
}

void PumpOn(void) {
    IoTGpioSetOutputVal(PUMP_RELAY_GPIO, 0);
    printf("[水泵]已开启\n");
}

void PumpOff(void) {
    IoTGpioSetOutputVal(PUMP_RELAY_GPIO, 1);
    printf("[水泵]已关闭\n");
}

void init_pump_queue(void) {
    UINT32 ret = LOS_QueueCreate("pump_queue", PUMP_QUEUE_LEN, &g_pumpQueueId, 0, sizeof(UINT32));
    if (ret != LOS_OK) {
        printf("创建水泵队列失败，代码: 0x%x\n", ret);
    } else {
        printf("水泵指令队列创建成功\n");
    }
}

void water_pump_thread(void *arg)
{
    // 创建水泵队列
    UINT32 ret = LOS_QueueCreate("pump_queue", PUMP_QUEUE_LEN, &g_pumpQueueId, 0, sizeof(UINT32));
    if (ret != LOS_OK) {
        printf("创建水泵队列失败，错误码: 0x%x\n", ret);
        return;
    }
    printf("水泵队列创建成功，ID = 0x%x\n", g_pumpQueueId);

    PumpInit();

    while (1) {
        UINT32 pumpCmd = 0;
        UINT32 len = sizeof(UINT32);
        ret = LOS_QueueRead(g_pumpQueueId, &pumpCmd, &len, LOS_WAIT_FOREVER);
        if (ret == LOS_OK) {
            printf("pump 线程中 g_pumpQueueId 地址: %p，值: 0x%x\n", &g_pumpQueueId, g_pumpQueueId);
            printf("[水泵线程] 收到指令: %u\n", pumpCmd);
            if (pumpCmd == IOT_CMD_PUMP) {
                PumpOn();
                LOS_Msleep(3000);  // 启动 3 秒
                PumpOff();
            } else {
                printf("⚠️ [水泵线程] 未知指令: %u\n", pumpCmd);
            }
        } else {
            printf("[水泵线程] 读取队列失败，错误码: 0x%x\n", ret);
        }

        LOS_Msleep(50);
    }
}
