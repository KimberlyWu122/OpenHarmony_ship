#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "los_task.h"
#include "ohos_init.h"
#include "cmsis_os.h"
#include "iot_uart.h"
#include "iot_gpio.h"
#include "los_tick.h"
#include "gps.h"

#define GPS_UART_ID EUART2_M1
#define GPS_BAUDRATE 9600
GPS_Coordinate g_current_gps = {0};


// 初始化串口
void GPS_UART_Init(void) {
    IotUartAttribute attr = {
        .baudRate = GPS_BAUDRATE,
        .dataBits = IOT_UART_DATA_BIT_8,
        .stopBits = IOT_UART_STOP_BIT_1,
        .parity = IOT_UART_PARITY_NONE
    };
    IoTUartInit(GPS_UART_ID, &attr);
}

// 坐标转换
float convert_to_decimal(const char *nmea_coord, const char *direction) {
    float deg = 0.0f, minutes = 0.0f;
    int deg_part = 0;
    char buf[16] = {0};
    strncpy(buf, nmea_coord, sizeof(buf) - 1);
    char *dot = strchr(buf, '.');
    if (!dot) return 0.0f;
    int len = dot - buf;
    if (len <= 2) return 0.0f;

    buf[len - 2] = '\0';
    deg_part = atoi(buf);
    minutes = atof(nmea_coord + len - 2);
    deg = deg_part + (minutes / 60.0f);
    if (direction[0] == 'S' || direction[0] == 'W') deg *= -1.0f;
    return deg;
}

// GNRMC语句解析
void ParseGNRMC(char *line, float *latitude, float *longitude) {
    char *token;
    int field = 0;
    char lat_str[16] = {0}, lat_dir[2] = {0};
    char lon_str[16] = {0}, lon_dir[2] = {0};

    token = strtok(line, ",");
    while (token != NULL) {
        field++;
        switch (field) {
            case 4: strncpy(lat_str, token, sizeof(lat_str) - 1); break;
            case 5: strncpy(lat_dir, token, sizeof(lat_dir) - 1); break;
            case 6: strncpy(lon_str, token, sizeof(lon_str) - 1); break;
            case 7: strncpy(lon_dir, token, sizeof(lon_dir) - 1); break;
        }
        token = strtok(NULL, ",");
    }

    *latitude = convert_to_decimal(lat_str, lat_dir);
    *longitude = convert_to_decimal(lon_str, lon_dir);
}

// GPS 线程函数（持续读取）
void *GPS_Thread(const char *arg) {
    char line[128] = {0};
    int line_pos = 0;

    GPS_UART_Init();
    printf("📡 GPS 串口初始化完成，开始接收数据...\n");

    while (1) {
        unsigned char ch;
        if (IoTUartRead(GPS_UART_ID, &ch, 1) > 0) {
            printf("收到了字符: %c (%02X)\n", ch, ch);
            printf("原始GPS数据：%s\n", line);
            if (ch == '\n') {
                line[line_pos] = '\0';

                if (strstr(line, "$GNRMC")) {
                    printf("收到GNRMC: %s\n", line);
                    float lat = 0.0f, lon = 0.0f;
                    ParseGNRMC(line, &lat, &lon);
                    printf("当前坐标：纬度 %.6f°, 经度 %.6f°\n", lat, lon);

                    // 更新全局坐标
                    g_current_gps.latitude = lat;
                    g_current_gps.longitude = lon;
                    g_current_gps.valid = true;
                }


                line_pos = 0;
            } else {
                if (line_pos < sizeof(line) - 1) {
                    line[line_pos++] = ch;
                } else {
                    line_pos = 0;  // 缓冲溢出，清零
                }
            }
        }

        usleep(10000);  // 避免CPU爆转
    }

    return NULL;
}

// 创建任务
void StartGPSTask(void) {
    TSK_INIT_PARAM_S task;
    memset(&task, 0, sizeof(task));
    task.pfnTaskEntry = (TSK_ENTRY_FUNC)GPS_Thread;
    task.uwStackSize = 4096;
    task.pcName = "gps_thread";
    task.usTaskPrio = 24;

    if (LOS_TaskCreate(NULL, &task) == LOS_OK) {
        printf("GPS线程创建成功\n");
    } else {
        printf("GPS线程创建失败\n");
    }
}
