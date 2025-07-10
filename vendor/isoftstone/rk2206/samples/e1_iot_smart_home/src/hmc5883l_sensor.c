#include "hmc5883l_sensor.h"
#include "iot_i2c.h"
#include <math.h>
#include <stdio.h>

#define HMC5883L_ADDR 0x1E
#define I2C_HANDLE EI2C0_M2

void HMC5883L_Init(void)
{
    // 配置寄存器A：平均8样本，15Hz，正常测量模式
    uint8_t regA[2] = {0x00, 0x70};
    IoTI2cWrite(I2C_HANDLE, HMC5883L_ADDR, regA, 2);

    // 配置寄存器B：增益设置
    uint8_t regB[2] = {0x01, 0xA0};
    IoTI2cWrite(I2C_HANDLE, HMC5883L_ADDR, regB, 2);

    // 模式寄存器：连续测量模式
    uint8_t mode[2] = {0x02, 0x00};
    IoTI2cWrite(I2C_HANDLE, HMC5883L_ADDR, mode, 2);
}

float HMC5883L_ReadAngle(void)
{
    uint8_t reg = 0x03;
    uint8_t buf[6] = {0};
    IoTI2cWrite(I2C_HANDLE, HMC5883L_ADDR, &reg, 1);
    IoTI2cRead(I2C_HANDLE, HMC5883L_ADDR, buf, 6);

    int16_t x = (buf[0] << 8) | buf[1];
    int16_t z = (buf[2] << 8) | buf[3];
    int16_t y = (buf[4] << 8) | buf[5];

    float angle = atan2((float)y, (float)x) * 180.0f / M_PI;
    if (angle < 0) angle += 360.0f;

    return angle;
}
