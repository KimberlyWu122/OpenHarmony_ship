# UART概述

## 简介
- 通用异步收发传输器（Universal Asynchronous Receiver/Transmitter)，通常称作UART。
- 它将要传输的资料在串行通信与并行通信之间加以转换。作为把并行输入信号转成串行输出信号的芯片，UART通常被集成于其他通讯接口的连结上。
- UART是一种通用串行数据总线，用于异步通信。该总线双向通信，可以实现全双工传输和接收。
- 主要参数
    1 波特率
    表示每秒钟传送的码元符号的个数
    2 报文格式
    起始位（1bit）+ 数据位（5-8bit）+ 奇偶校验位（1bit）+ 停止位（1,1.5,2bit）

## 接口说明

### 包含头文件： 

```c
#include "iot_uart.h"
```

#### 1. IoTUartInit

```c
unsigned int IoTUartInit(unsigned int id, const IotUartAttribute *param);
```

> IoTUartInit初始化UART设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | uart id号      |
| param         | IotUartAttribute *  | uart配置       |

#### 2. IoTUartDeinit

```c
unsigned int IoTUartDeinit(unsigned int id);
```

> IoTUartDeinit注销UART设备。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description    |
| ------------- | ------------------- | -------------- |
| id            | unsigned int        | uart id号      |

#### 3. IoTUartRead

```c
int IoTUartRead(unsigned int id, unsigned char *data, unsigned int dataLen);
```

> IoTUartRead读取串口数据。放回数据长度。

| Parameters(T) | Data Type           | Description  |
| ------------- | ------------------- | ------------ |
| id            | unsigned int        | uart id号    |
| data          | unsigned char *     | 数据         |
| dataLen       | unsigned int        | 读取数据长度 |

#### 4. IoTUartWrite

```c
int IoTUartWrite(unsigned int id, const unsigned char *data, unsigned int dataLen);
```

> IoTUartWrite读取串口数据。放回数据长度。

| Parameters(T) | Data Type           | Description  |
| ------------- | ------------------- | ------------ |
| id            | unsigned int        | uart id号    |
| data          | unsigned char *     | 数据         |
| dataLen       | unsigned int        | 写入数据长度 |

#### 5. IoTUartSetFlowCtrl

```c
unsigned int IoTUartSetFlowCtrl(unsigned int id, IotFlowCtrl flowCtrl);
```

> IoTUartSetFlowCtrl设置UART流控。成功返回IOT_SUCCESS，否则返回IOT_FAILURE。

| Parameters(T) | Data Type           | Description  |
| ------------- | ------------------- | ------------ |
| id            | unsigned int        | uart id号    |
| flowCtrl      | IotFlowCtrl         | 流控参数     |

IotFlowCtrl:
```c
/**
 * @brief Enumerates hardware flow control modes.
 *
 * @since 2.2
 * @version 2.2
 */
typedef enum {
    /** Hardware flow control disabled */
    IOT_FLOW_CTRL_NONE,
    /** RTS and CTS hardware flow control enabled */
    IOT_FLOW_CTRL_RTS_CTS,
    /** RTS hardware flow control enabled */
    IOT_FLOW_CTRL_RTS_ONLY,
     /** CTS hardware flow control enabled */
    IOT_FLOW_CTRL_CTS_ONLY,
} IotFlowCtrl;
```
