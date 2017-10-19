#ifndef __BUZZER_H
#define __BUZZER_H

#include "sys.h"

#define BUZZER_PIN PAout(8)
#define BEEP_TIME 200
//#define ACTIVE_BEEP 1

void TIM1_Int_Init(void);
void beep_on(unsigned int time);

#endif

