#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

#define LED_ON 0
#define LED_OFF 1
#define POWER_LED PBout(6)
#define MODE_LED PBout(7)

typedef enum
{
	LED_POWER_ON,
	LED_POWER_OFF,
} LED_MODE_ENUM;

void led_handle(LED_MODE_ENUM led_mode);
void led_init(void);

#endif	//__LED_H__

