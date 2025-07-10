#include <stdio.h>
#include "los_task.h"
#include "ohos_init.h"
#include "iot_pwm.h"
#include "iot_errno.h"
#include "iot.h"
#include "smart_home_event.h"
#include "los_queue.h"
#include "autopilot_utils.h"

// ======== 电机控制指令宏定义 ========
#define IOT_CMD_FORWARD   1
#define IOT_CMD_LEFT      2
#define IOT_CMD_BACKWARD  3
#define IOT_CMD_RIGHT     4
#define IOT_CMD_STOP      5
#define IOT_CMD_AUTO_CIRCLE 7

// ======== 队列配置 ========
#define MOTOR_QUEUE_LEN 8
UINT32 g_motorQueueId;  // ✅ 队列ID就是 UINT32

// ======== PWM 引脚定义 ========
// 左电机（PB4、PB5）
#define MOTOR_L_PWM1 EPWMDEV_PWM0_M1  // PB4
#define MOTOR_L_PWM2 EPWMDEV_PWM1_M1  // PB5

// 右电机（PC5、PC6）
#define MOTOR_R_PWM1 EPWMDEV_PWM7_M1  // PC5
#define MOTOR_R_PWM2 EPWMDEV_PWM3_M1  // PD0

// ======== 初始化 motor 指令队列 ========
void init_motor_queue() {
    UINT32 ret = LOS_QueueCreate("motor_queue", MOTOR_QUEUE_LEN, &g_motorQueueId, 0, sizeof(UINT32));
    if (ret != LOS_OK) {
        printf("❌ 创建电机队列失败，代码: 0x%x\n", ret);
    } else {
        printf("✅ 电机指令队列创建成功，ID = 0x%x\n", g_motorQueueId);
    }
}

// ======== 控制一对电机运动方向 ========
void control_motor_pair(unsigned int pwm1, unsigned int pwm2, int state, unsigned int duty) {
    if (state == 1) {
        IoTPwmStart(pwm1, duty, 1000);
        IoTPwmStart(pwm2, 0, 1000);
    } else if (state == 2) {
        IoTPwmStart(pwm1, 0, 1000);
        IoTPwmStart(pwm2, duty, 1000);
    } else {
        IoTPwmStop(pwm1);
        IoTPwmStop(pwm2);
    }
}

// ======== 电机控制线程 ========
void motor_control_thread(void *arg) {
    init_motor_queue();

    IoTPwmInit(MOTOR_L_PWM1);
    IoTPwmInit(MOTOR_L_PWM2);
    IoTPwmInit(MOTOR_R_PWM1);
    IoTPwmInit(MOTOR_R_PWM2);

    while (1) {
        UINT32 cmd = 0;
        UINT32 len = sizeof(UINT32);
        if (LOS_QueueRead(g_motorQueueId, &cmd, &len, LOS_WAIT_FOREVER) == LOS_OK) {
            printf("收到电机控制指令：%u\n", cmd);
            printf("motor队列变量地址 = %p，当前ID = 0x%x\n", &g_motorQueueId, g_motorQueueId);

            switch (cmd) {
                case IOT_CMD_FORWARD:
                    printf("[FORWARD] 双电机正转\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 1, 60);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 1, 60);
                    break;
                case IOT_CMD_BACKWARD:
                    printf("[BACKWARD] 双电机反转\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 2, 60);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 2, 60);
                    break;
                case IOT_CMD_LEFT:
                    printf("[LEFT] 左转（左慢右快）\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 1, 30);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 1, 60);
                    break;
                case IOT_CMD_RIGHT:
                    printf("[RIGHT] 右转（右慢左快）\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 1, 60);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 1, 30);
                    break;
                case IOT_CMD_AUTO_CIRCLE:
                    printf("[AUTO-CIRCLE] 自动巡航开始，目标航点数：%d\n", waypointCount);

                    for (int i = 0; i < waypointCount; ++i) {
                        double targetLng = waypointList[i].lng;
                        double targetLat = waypointList[i].lat;
                        printf("导航至航点 %d: 经度 = %.6f，纬度 = %.6f\n", i + 1, targetLng, targetLat);

                        while (1) {
                            double currLng = get_current_gps_lng();
                            double currLat = get_current_gps_lat();

                            double distanceLng = fabs(currLng - targetLng);
                            double distanceLat = fabs(currLat - targetLat);
                            if (distanceLng < 0.0002 && distanceLat < 0.0002) {
                                printf("已到达航点 %d\n", i + 1);
                                break;
                            }

                            double targetHeading = calculate_bearing(currLat, currLng, targetLat, targetLng);
                            double currentHeading = get_current_heading();
                            if (is_obstacle_detected(30.0f)) {
                                // 停止前进，尝试避障或等待障碍物消失
                                control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 0, 0);
                                control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 0, 0);
                                printf("检测到前方障碍，暂停前进，等待...\n");
                                LOS_Msleep(1000);
                                continue; // 继续下一轮检测

                            if (is_heading_close(currentHeading, targetHeading, 10.0)) {
                                // 航向接近目标方向，前进
                                control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 1, 60);
                                control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 1, 60);
                                printf("航向 %.2f 接近目标 %.2f，前进中...\n", currentHeading, targetHeading);
                            } else if (((targetHeading - currentHeading + 360) % 360) < 180) {
                                // 需要右转
                                control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 1, 60);
                                control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 1, 30);
                                printf("右转调整航向：当前 %.2f → 目标 %.2f\n", currentHeading, targetHeading);
                            } else {
                                // 需要左转
                                control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 1, 30);
                                control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 1, 60);
                                printf("左转调整航向：当前 %.2f → 目标 %.2f\n", currentHeading, targetHeading);
                            }

                            LOS_Msleep(1000);  // 每秒判断一次
                        }

                        // 每个航点短暂停止
                        control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 0, 0);
                        control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 0, 0);
                        LOS_Msleep(500);
                    }

                    printf("自动巡航完成！\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 0, 0);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 0, 0);
                    break;

                case IOT_CMD_STOP:
                printf("[STOP] 停止\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 0, 0);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 0, 0);
                    break;
                default:
                    printf("[STOP] 停止\n");
                    control_motor_pair(MOTOR_L_PWM1, MOTOR_L_PWM2, 0, 0);
                    control_motor_pair(MOTOR_R_PWM1, MOTOR_R_PWM2, 0, 0);
                    break;
                
                
            }
        }
        LOS_Msleep(50);
    }
}

// ======== 系统入口：启动线程 ========
void ship_control_example(void) {
    printf("电机控制模块初始化开始\n");
    UINT32 thread_id;
    TSK_INIT_PARAM_S task = {0};
    task.pfnTaskEntry = (TSK_ENTRY_FUNC)motor_control_thread;
    task.uwStackSize = 2048;
    task.pcName = "dual_motor";
    task.usTaskPrio = 20;

    if (LOS_TaskCreate(&thread_id, &task) != LOS_OK) {
        printf("创建电机线程失败\n");
    } else {
        printf("电机线程已启动\n");
    }
}

APP_FEATURE_INIT(ship_control_example);
