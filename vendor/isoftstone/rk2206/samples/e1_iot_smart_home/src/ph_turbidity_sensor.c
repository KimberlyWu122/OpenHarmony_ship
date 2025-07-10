#include <stdio.h>
#include <unistd.h>
#include "iot_adc.h"
#include "ph_sensor.h"
#include "turbidity_sensor.h"
#include <stdint.h>
#include "mux_selector.h"


#define PH_ADC_CHANNEL 4          
#define TURBIDITY_ADC_CHANNEL 4   
#define ADC_MAX_VALUE 4095.0
#define ADC_REF_VOLTAGE 3.3


extern void SelectAnalogChannel(uint8_t channel);

// ========== pH 模块 ==========
void PhSensorInit(void)
{
    if (IoTAdcInit(PH_ADC_CHANNEL) != 0) {
        printf("pH ADC 初始化失败\n");
    } else {
        printf("pH ADC 初始化成功\n");
    }
}

float PhSensorRead(void)
{
    SelectAnalogChannel(2); // Y2 通道
    usleep(10000);

    unsigned int adc_value = 0;
    if (IoTAdcGetVal(PH_ADC_CHANNEL, &adc_value) != 0) {
        printf("读取 pH ADC 失败\n");
        return -1;
    }

    float voltage = adc_value * ADC_REF_VOLTAGE / ADC_MAX_VALUE;
    float ph = 7 + ((2.5 - voltage) / 0.18);  
    printf("[pH] ADC: %u, 电压: %.2f V, pH: %.2f\n", adc_value, voltage, ph);
    return ph;
}
void TurbiditySensorInit(void)
{
    if (IoTAdcInit(TURBIDITY_ADC_CHANNEL) != 0) {
        printf("浊度 ADC 初始化失败\n");
    } else {
        printf("浊度 ADC 初始化成功\n");
    }
}

float TurbiditySensorRead(void)
{
    SelectAnalogChannel(1); // Y1 通道
    usleep(10000);

    unsigned int adc_value = 0;
    if (IoTAdcGetVal(TURBIDITY_ADC_CHANNEL, &adc_value) != 0) {
        printf("读取浊度 ADC 失败\n");
        return -1;
    }

    float voltage = adc_value * ADC_REF_VOLTAGE / ADC_MAX_VALUE;
    float turbidity = (3.3 - voltage) * 1000;

    printf("[Turbidity] ADC: %u, 电压: %.2f V, 浊度: %.2f NTU\n", adc_value, voltage, turbidity);
    return turbidity;
}
