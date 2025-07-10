#ifndef __TDS_SENSOR_H__
#define __TDS_SENSOR_H__

// 初始化 TDS 传感器（ADC）
void TdsSensorInit(void);

// 读取 TDS ppm 值并返回
float TdsSensorRead(void);

#endif
