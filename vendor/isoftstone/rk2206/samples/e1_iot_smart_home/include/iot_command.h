#ifndef _IOT_COMMAND_H_
#define _IOT_COMMAND_H_

// 电机相关命令
#define IOT_CMD_FORWARD     1   // 前进
#define IOT_CMD_LEFT        2   // 左转
#define IOT_CMD_BACKWARD    3   // 后退
#define IOT_CMD_RIGHT       4   // 右转
#define IOT_CMD_STOP        5   // 停止
#define IOT_CMD_PUMP        6   // 采样

// 灯光、自动控制等
#define IOT_CMD_LIGHT_ON    0x01
#define IOT_CMD_LIGHT_OFF   0x02
#define IOT_CMD_MOTOR_ON    0x03
#define IOT_CMD_MOTOR_OFF   0x04
#define IOT_CMD_AUTO_ON     0x05
#define IOT_CMD_AUTO_OFF    0x06

#endif
