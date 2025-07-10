#ifndef __MUX_SELECTOR_H__
#define __MUX_SELECTOR_H__

#include <stdint.h>

void MuxGpioInit(void);                    // 初始化 GPIO（S0/S1/S2）
void SelectAnalogChannel(uint8_t channel); // 切换通道

#endif
