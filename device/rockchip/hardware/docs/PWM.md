# PWM概述

## 简介

- 脉冲宽度调制(PWM)，是英文“Pulse Width Modulation”的缩写，简称**脉宽调制**，是利用微处理器的数字输出来对模拟电路进行控制的一种非常有效的技术，广泛应用在从测量、通信到功率控制与变换的许多领域中.

- PWM的频率：是指1秒钟内信号从高电平到低电平再回到高电平的次数(一个周期)；

- PWM占空比：是一个脉冲周期内，高电平的时间与整个周期时间的比例：

- 总结：
  PWM就是在合适的信号频率下，通过一个周期里改变占空比的方式来改变输出的有效电压

  PWM频率越大，响应越快，

## 接口说明

### 包含头文件： 

```c
#include "iot_pwm.h"
```

#### 1. IoTPwmInit

```c
unsigned int IoTPwmInit(unsigned int port);
```

> IoTPwmInit初始化PWM设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| port          | unsigned int        | pwm端口号      |

#### 2. IoTPwmDeinit

```c
unsigned int IoTPwmDeinit(unsigned int port);
```

> IoTPwmDeinit注销PWM设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| port          | unsigned int        | pwm端口号      |

#### 3. IoTPwmStart

```c
unsigned int IoTPwmStart(unsigned int port, unsigned short duty, unsigned int freq);
```

> IoTPwmStart开启PWM设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| port          | unsigned int        | pwm端口号      |
| duty          | unsigned short      | pwm占空比      |
| freq          | unsigned int        | pwm频率        |

#### 4. IoTPwmStop

```c
unsigned int IoTPwmStop(unsigned int port);
```

> IoTPwmStop停止PWM设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| port          | unsigned int        | pwm端口号      |
