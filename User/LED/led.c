#include "led.h"

void led_handle(LED_MODE_ENUM led_mode)
{
	switch(led_mode)
	{
		case LED_POWER_ON:
			POWER_LED = LED_ON;
			MODE_LED = LED_ON;
			break;
		case LED_POWER_OFF:
			POWER_LED = LED_ON;
			MODE_LED = LED_OFF;
			break;
	}
}

void led_init(void)
{
	POWER_LED = LED_ON;
	MODE_LED = LED_OFF;
}


