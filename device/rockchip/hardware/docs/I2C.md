# I2C概述

## 简介

-   I2C\(Inter Integrated Circuit\)总线是由Philips公司开发的一种简单、双向二线制同步串行总线。
-   I2C以主从方式工作，通常有一个主设备和一个或者多个从设备，主从设备通过SDA\(SerialData\)串行数据线以及SCL\(SerialClock\)串行时钟线两根线相连。

-   I2C数据的传输必须以一个起始信号作为开始条件，以一个结束信号作为传输的停止条件。数据传输以字节为单位，高位在前，逐个bit进行传输。
-   I2C总线上的每一个设备都可以作为主设备或者从设备，而且每一个设备都会对应一个唯一的地址，当主设备需要和某一个从设备通信时，通过广播的方式，将从设备地址写到总线上，如果某个从设备符合此地址，将会发出应答信号，建立传输。

-   I2C接口定义了完成I2C传输的通用方法集合，包括：

    -   I2C控制器管理:  打开或关闭I2C控制器
    -   I2C消息传输：通过消息传输结构体数组进行自定义传输

## 接口说明

### 包含头文件： 

```c
#include "iot_i2c.h"
```

#### 1. IoTI2cInit

```C
unsigned int IoTI2cInit(unsigned int id, unsigned int baudrate);
```

> IoTI2cInit初始化I2C设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | i2c id号       |
| baudrate      | unsigned int        | i2c速率        |

```c
/* 定义I2C速率 */
enum EnumI2cFre {
  EI2C_FRE_100K = 0,
  EI2C_FRE_400K,
  EI2C_FRE_1000K,
  EI2C_FRE_MAX,
};
```
  
#### 2. IoTI2cDeinit

```C
unsigned int IoTI2cDeinit(unsigned int id);
```

> IoTI2cDeinit注销I2C设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | i2c id号       |

#### 3. IoTI2cWrite

```C
unsigned int IoTI2cWrite(unsigned int id, unsigned short deviceAddr, const unsigned char *data, unsigned int dataLen);
```

> IoTI2cWrite通过I2C写数据。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | i2c id号       |
| deviceAddr    | unsigned short      | 设备地址       |
| data          | unsigned char *     | 数据           |
| dataLen       | unsigned int        | 写入数据长度   |

#### 4. IoTI2cRead

```C
unsigned int IoTI2cRead(unsigned int id, unsigned short deviceAddr, unsigned char *data, unsigned int dataLen);
```

> IoTI2cRead通过I2C读数据。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | i2c id号       |
| deviceAddr    | unsigned short      | 设备地址       |
| data          | unsigned char *     | 数据           |
| dataLen       | unsigned int        | 读取数据长度   |

#### 5. IoTI2cSetBaudrate

```C
unsigned int IoTI2cSetBaudrate(unsigned int id, unsigned int baudrate);
```

> IoTI2cSetBaudrate设置I2C速率。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | i2c id号       |
| baudrate      | unsigned int        | i2c速率        |

#### 6. IoTI2cScan

```C
unsigned int IoTI2cScan(unsigned int id, unsigned short *slaveAddr, unsigned int slaveAddrLen);
```

> IoTI2cScan扫描挂载I2C总线上的设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | i2c id号       |
| slaveAddr     | unsigned short *    | 从机地址       |
| slaveAddrLen  | unsigned int        | 从机地址长度   |
