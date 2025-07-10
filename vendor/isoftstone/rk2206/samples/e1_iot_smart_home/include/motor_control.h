#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include "iot_pwm.h"
#include "iot_command.h"
#include "los_queue.h"  // 虽然改成int了，保留它以防LOS_Queue相关依赖

// 电机PWM端口定义
#define MOTOR_LEFT_PWM_PORT  EPWMDEV_PWM6_M0 // GPIO_PC6
#define MOTOR_RIGHT_PWM_PORT EPWMDEV_PWM7_M0 // GPIO_PC7

// 船舶运动方向
typedef enum {
    SHIP_STOP = 0,
    SHIP_FORWARD,
    SHIP_BACKWARD,
    SHIP_TURN_LEFT,
    SHIP_TURN_RIGHT,
} ShipMovement;

// 电机控制占空比结构
typedef struct {
    unsigned int leftDuty;
    unsigned int rightDuty;
} MotorControl;

extern UINT32 g_motorQueueId;


// 功能函数声明
MotorControl set_ship_movement(ShipMovement movement, unsigned int speed);
int control_motors(MotorControl control);
void ship_control_example(void);
void motor_control_thread(void *arg);

#endif // __MOTOR_CONTROL_H__
