#include <stdio.h>
#include <unistd.h>
#include "iot_adc.h"
#include "tds_sensor.h"

#define TDS_ADC_CHANNEL 4
#define ADC_MAX_VALUE 4095.0
#define ADC_REF_VOLTAGE 3.3
#define TDS_TEMP_COEFF 0.5

// 初始化 ADC 通道
void TdsSensorInit(void)
{
    if (IoTAdcInit(TDS_ADC_CHANNEL) != 0) {
        printf("TDS ADC 初始化失败\n");
    } else {
        printf("TDS ADC 初始化成功\n");
    }
}

// 读取 ADC 电压 → 转换为 TDS ppm 值
float TdsSensorRead(void)
{
    unsigned int adc_value = 0;

    if (IoTAdcGetVal(TDS_ADC_CHANNEL, &adc_value) != 0) {
        printf("读取 TDS ADC 失败\n");
        return -1;
    }

    float voltage = adc_value * ADC_REF_VOLTAGE / ADC_MAX_VALUE;

    float tds = (133.42 * voltage * voltage * voltage
               - 255.86 * voltage * voltage
               + 857.39 * voltage) * TDS_TEMP_COEFF;

    printf("[TDS] ADC原始值: %u, 电压: %.2f V, TDS: %.2f ppm\n", adc_value, voltage, tds);
    return tds;
}