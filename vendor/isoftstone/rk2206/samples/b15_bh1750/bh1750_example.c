/*
 * Copyright (c) 2022 FuZhou Lockzhiner Electronic Co., Ltd. All rights reserved.
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
#include "bh1750.h"

void bh1750_proress(void *arg)
{
    double bh1750_data = 0.0;

    bh1750_init();

    while (1)
    {
        printf("************ bh1750 Process ************\n");

        bh1750_read_data(&bh1750_data);

        printf("lux: %.2f\r\n", bh1750_data);

        printf("\r\n");

        LOS_Msleep(1000);
    }
}

void bh1750_example()
{
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};
    unsigned int ret = LOS_OK;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)bh1750_proress;
    task.uwStackSize = 2048;
    task.pcName = "bh1750 process";
    task.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK)
    {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }
}

APP_FEATURE_INIT(bh1750_example);


