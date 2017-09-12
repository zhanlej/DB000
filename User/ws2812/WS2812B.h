#ifndef __WS2812B_H
#define	__WS2812B_H

#include "stm32f10x.h"
#include "delay.h"	

void Timer4_init(void);
void WS2812_send(uint8_t color[3], uint16_t len);

extern uint8_t colors[12][3];

#endif /* __LED_H */
