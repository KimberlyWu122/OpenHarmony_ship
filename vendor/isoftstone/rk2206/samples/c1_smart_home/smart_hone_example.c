#include <stdio.h>
#include <stdbool.h>

#include "los_task.h"
#include "ohos_init.h"

#include "smart_home.h"

#define MSG_QUEUE_LENGTH                                16
#define BUFFER_LEN                                      50

static unsigned int m_msg_queue;

void smart_hone_thread(void *arg)
{
    double *data_ptr = NULL;

    bool motor_state = false;
    bool light_state = false;
    bool auto_state = true;

    double lllumination_range = 50.0;
    double temperature_range = 35.0;
    double humidity_range = 80.0;

    lcd_dev_init();
    motor_dev_init();
    light_dev_init();
    su03t_init();

    lcd_load_ui();

    while(1)
    {
        LOS_QueueRead(m_msg_queue, (void *)&data_ptr, BUFFER_LEN, LOS_WAIT_FOREVER);

        if (data_ptr[0] < lllumination_range && auto_state)
        {
            light_state = true;
            light_set_state(light_state);
        }
        else if (data_ptr[0] > lllumination_range && auto_state)
        {
            light_state = false;
            light_set_state(light_state);
        }

        if ((data_ptr[1] > temperature_range || data_ptr[2] > humidity_range) && auto_state)
        {
            motor_state = true;
            motor_set_state(motor_state);
        }
        else if ((data_ptr[1] < temperature_range || data_ptr[2] < humidity_range) && auto_state)
        {
            motor_state = false;
            motor_set_state(motor_state);
        }

        lcd_set_lllumination(data_ptr[0]);
        lcd_set_temperature(data_ptr[1]);
        lcd_set_humidity(data_ptr[2]);
        lcd_set_light_state(light_state);
        lcd_set_motor_state(motor_state);
        lcd_set_auto_state(auto_state);

        printf("============= smart home example ==============\n");
        printf("======== data ========\r\n");
        printf("lllumination:%5.2f\r\n", data_ptr[0]);
        printf("temperature:%5.2f\r\n", data_ptr[1]);
        printf("humidity:%5.2f\r\n", data_ptr[2]);
        printf("======== state ========\r\n");
        printf("light_state:%d\r\n", light_state);
        printf("motor_state:%d\r\n", motor_state);
        printf("auto_state:%d\r\n", auto_state);

        printf("\r\n");

        LOS_Msleep(500);
    }
}

void device_read_thraed(void *arg)
{
    double read_data[3] = {0};

    i2c_dev_init();

    while(1)
    {
        bh1750_read_data(&read_data[0]);
        sht30_read_data(&read_data[1]);
        LOS_QueueWrite(m_msg_queue, (void *)&read_data, sizeof(read_data), LOS_WAIT_FOREVER);
        LOS_Msleep(500);
    }
}

void smart_hone_example()
{
    unsigned int thread_id_1;
    unsigned int thread_id_2;
    TSK_INIT_PARAM_S task_1 = {0};
    TSK_INIT_PARAM_S task_2 = {0};
    unsigned int ret = LOS_OK;

    ret = LOS_QueueCreate("queue", MSG_QUEUE_LENGTH, &m_msg_queue, 0, BUFFER_LEN);
    if (ret != LOS_OK)
    {
        printf("Falied to create Message Queue ret:0x%x\n", ret);
        return;
    }

    task_1.pfnTaskEntry = (TSK_ENTRY_FUNC)smart_hone_thread;
    task_1.uwStackSize = 2048;
    task_1.pcName = "smart hone thread";
    task_1.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id_1, &task_1);
    if (ret != LOS_OK)
    {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }

    task_2.pfnTaskEntry = (TSK_ENTRY_FUNC)device_read_thraed;
    task_2.uwStackSize = 2048;
    task_2.pcName = "device read thraed";
    task_2.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id_2, &task_2);
    if (ret != LOS_OK)
    {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }
}

APP_FEATURE_INIT(smart_hone_example);
