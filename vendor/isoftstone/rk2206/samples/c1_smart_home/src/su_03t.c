#include "su_03t.h"
#include "los_task.h"
#include "ohos_init.h"

#include "iot_errno.h"
#include "iot_uart.h"

#include <stdio.h>
#include <stdint.h>

#define UART0_HANDLE EUART0_M0

static void su_03t_thread(void *arg)
{
    IotUartAttribute attr;
    unsigned int ret = 0;

    IoTUartDeinit(UART0_HANDLE);
    
    attr.baudRate = 115200;
    attr.dataBits = IOT_UART_DATA_BIT_8;
    attr.pad = IOT_FLOW_CTRL_NONE;
    attr.parity = IOT_UART_PARITY_NONE;
    attr.rxBlock = IOT_UART_BLOCK_STATE_NONE_BLOCK;
    attr.stopBits = IOT_UART_STOP_BIT_1;
    attr.txBlock = IOT_UART_BLOCK_STATE_NONE_BLOCK;
    
    ret = IoTUartInit(UART0_HANDLE, &attr);
    if (ret != IOT_SUCCESS)
    {
        printf("%s, %d: IoTUartInit(%d) failed!\n", __FILE__, __LINE__, ret);
        return;
    }

    while(1)
    {
        uint8_t data[64] = {0};

        uint8_t rec_len = IoTUartRead(UART0_HANDLE, data, sizeof(data));

        printf("----------------- %d ------------------\r\n", rec_len);  
        if (rec_len != 0)
        {
          printf("----------------- %x%x%x%x ------------------\r\n", data[0], data[1], data[2], data[3]);  
        }

        LOS_Msleep(100);
    }
}

void su03t_init(void)
{
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};
    unsigned int ret = LOS_OK;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)su_03t_thread;
    task.uwStackSize = 2048;
    task.pcName = "su-03t thread";
    task.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK)
    {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }
}
