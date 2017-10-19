#include "buzzer.h"
#include "delay.h"

void TIM1_Int_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

  //使能TIM1的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  //TIM1_CHN1 GPIO初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 72kHz
  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM1, ENABLE);

  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 500;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

//	 //死区设置
//	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
//	TIM_BDTRInitStructure.TIM_DeadTime = 0x90;  //这里调整死区大小0-0xff
//	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
//	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
//	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE ); //使能指定的TIM3中断,允许更新中断

  //TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable); //使能预装载寄存器
  //TIM_SetCompare4(TIM1,300);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	//TIM1_OC通道输出PWM（一定要加）
  //TIM_Cmd(TIM1, ENABLE); //使能 TIM1
}

void beep_on(unsigned int time)
{
#ifdef ACTIVE_BEEP
  BUZZER_PIN = 1;
  delay_ms(time);
  BUZZER_PIN = 0;
#else
  int time_array[6] = {19, 17, 15, 18, 19, 20};

  //TIM_SetCompare1(TIM1,time_count);
	TIM_Cmd(TIM1, ENABLE); //使能 TIM1
  TIM_PrescalerConfig(TIM1, time_array[1], TIM_PSCReloadMode_Update);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	//TIM1_OC通道输出PWM（一定要加）
  delay_ms(time);
  TIM_CtrlPWMOutputs(TIM1, DISABLE);
  TIM_Cmd(TIM1, DISABLE); //使能 TIM1
  BUZZER_PIN = 0;
//	if(time_count >= 1000) time_count = 0;
//	else time_count += 100;
#endif
}




