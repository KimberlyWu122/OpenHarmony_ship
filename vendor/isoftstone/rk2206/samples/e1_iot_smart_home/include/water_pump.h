#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include "los_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

// 全局队列ID：必须是 UINT32
extern UINT32 g_pumpQueueId;

void PumpInit(void);
void PumpOn(void);
void PumpOff(void);
void WaterPumpExample(void);
void water_pump_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif // WATER_PUMP_H
