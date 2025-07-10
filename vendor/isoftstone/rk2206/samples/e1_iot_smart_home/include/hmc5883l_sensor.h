#ifndef __HMC5883L_SENSOR_H__
#define __HMC5883L_SENSOR_H__

#include <stdint.h>

void HMC5883L_Init(void);
float HMC5883L_ReadAngle(void);

#endif
