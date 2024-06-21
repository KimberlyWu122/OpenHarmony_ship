# GPIO概述

## 简介

- GPIO（General-purpose input/output）即通用型输入输出。通常，GPIO控制器通过分组的方式管理所有GPIO管脚，每组GPIO有一个或多个寄存器与之关联，通过读写寄存器完成对GPIO管脚的操作。
- GPIO接口定义了操作GPIO管脚的标准方法集合，包括
  
  - 设置管脚方向： 方向可以是输入或者输出\(暂不支持高阻态\)
  - 读写管脚电平值： 电平值可以是低电平或高电平
  - 设置管脚中断服务函数：设置一个管脚的中断响应函数，以及中断触发方式
  - 使能和禁止管脚中断：禁止或使能管脚中

## 接口说明

### 包含头文件：

```c
#include "iot_gpio.h"
```

#### 1. IoTGpioInit

```c
unsigned int IoTGpioInit(unsigned int id);
```

> IoTGpioInit初始化GPIO设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |

#### 2. IoTGpioDeinit

```c
unsigned int IoTGpioDeinit(unsigned int id);
```

> IoTGpioDeinit释放GPIO设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |

#### 3.IoTGpioSetDir

```c
unsigned int IoTGpioSetDir(unsigned int id, IotGpioDir dir);
```

> IoTGpioSetDir配置GPIO方向。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |
| dir           | IotGpioDir      | gpio方向       |

IotGpioDir：
```c
/**
 * @brief Enumerates GPIO directions.
 */
typedef enum {
    /** Input */
    IOT_GPIO_DIR_IN = 0,
    /** Output */
    IOT_GPIO_DIR_OUT
} IotGpioDir;
```

#### 4.IoTGpioGetDir

```c
unsigned int IoTGpioGetDir(unsigned int id, IotGpioDir *dir);
```

> IoTGpioGetDir获取GPIO方向。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |
| dir           | IotGpioDir      | gpio方向       |

IotGpioDir：
```c
/**
 * @brief Enumerates GPIO directions.
 */
typedef enum {
    /** Input */
    IOT_GPIO_DIR_IN = 0,
    /** Output */
    IOT_GPIO_DIR_OUT
} IotGpioDir;
```

#### 5.IoTGpioSetOutputVal

```c
unsigned int IoTGpioSetOutputVal(unsigned int id, IotGpioValue val);
```

> IoTGpioSetOutputVal设置GPIO输出电平。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |
| val           | IotGpioValue    | gpio电平       |

IotGpioValue：
```c
/**
 * @brief Enumerates GPIO level values.
 */
typedef enum {
    /** Low GPIO level */
    IOT_GPIO_VALUE0 = 0,
    /** High GPIO level */
    IOT_GPIO_VALUE1
} IotGpioValue;
```

#### 6.IoTGpioGetOutputVal

```c
unsigned int IoTGpioGetOutputVal(unsigned int id, IotGpioValue *val);
```

> IoTGpioSetOutputVal获取GPIO输出电平。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |
| val           | IotGpioValue *  | gpio电平       |

IotGpioValue：
```c
/**
 * @brief Enumerates GPIO level values.
 */
typedef enum {
    /** Low GPIO level */
    IOT_GPIO_VALUE0 = 0,
    /** High GPIO level */
    IOT_GPIO_VALUE1
} IotGpioValue;
```

#### 7.IoTGpioGetInputVal

```c
unsigned int IoTGpioGetInputVal(unsigned int id, IotGpioValue *val);
```

> IoTGpioSetOutputVal获取GPIO输入电平。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type       | Description    |
| ------------- | --------------- | -------------- |
| id            | unsigned int    | gpio id号      |
| val           | IotGpioValue *  | gpio电平       |

IotGpioValue：
```c
/**
 * @brief Enumerates GPIO level values.
 */
typedef enum {
    /** Low GPIO level */
    IOT_GPIO_VALUE0 = 0,
    /** High GPIO level */
    IOT_GPIO_VALUE1
} IotGpioValue;
```

#### 8.IoTGpioRegisterIsrFunc

```c
unsigned int IoTGpioRegisterIsrFunc(unsigned int id, IotGpioIntType intType, IotGpioIntPolarity intPolarity, GpioIsrCallbackFunc func, char *arg);
```

> IoTGpioRegisterIsrFunc注册gpio中断。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type            | Description      |
| ------------- | -------------------- | ---------------- |
| id            | unsigned int         | gpio id号        |
| intType       | IotGpioIntType       | 中断类型         |
| intPolarity   | IotGpioIntPolarity   | 中断极性         |
| func          | GpioIsrCallbackFunc  | 中断回调函数     |
| arg           | char *               | 中断回调函数参数 |

IotGpioIntType:
```c
/**
 * @brief Enumerates GPIO interrupt trigger modes.
 */
typedef enum {
    /** Level-sensitive interrupt */
    IOT_INT_TYPE_LEVEL = 0,
    /** Edge-sensitive interrupt */
    IOT_INT_TYPE_EDGE
} IotGpioIntType;
```

IotGpioIntPolarity:
```c
/**
 * @brief Enumerates I/O interrupt polarities.
 */
typedef enum {
    /** Interrupt at a low level or falling edge */
    IOT_GPIO_EDGE_FALL_LEVEL_LOW = 0,
    /** Interrupt at a high level or rising edge */
    IOT_GPIO_EDGE_RISE_LEVEL_HIGH
} IotGpioIntPolarity;
```

#### 9.IoTGpioUnregisterIsrFunc

```c
unsigned int IoTGpioUnregisterIsrFunc(unsigned int id);
```

> IoTGpioRegisterIsrFunc注销gpio中断。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type            | Description      |
| ------------- | -------------------- | ---------------- |
| id            | unsigned int         | gpio id号        |

#### 10.IoTGpioSetIsrMask

```c
unsigned int IoTGpioSetIsrMask(unsigned int id, unsigned char mask);
```

> IoTGpioSetIsrMask屏蔽gpio中断。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type            | Description           |
| ------------- | -------------------- | --------------------- |
| id            | unsigned int         | gpio id号             |
| mask          | unsigned char        | 0:开启中断 1:屏蔽中断 |
