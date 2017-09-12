#include "exti.h"
#include "delay.h"
#include "uart1.h"
#include "string.h"
#include "main.h"
//#include "MyFifo.h"
#include "stdio.h"
#include "aqi.h"


extern volatile unsigned long sys_tick;

unsigned long press_time = 0;
unsigned char press_flag = 0;
unsigned char wait_send_press;
int press_len;
char press_buf[PRESS_SIZE][2];
u32 press_time_log[PRESS_SIZE];
u16 press_C1[PRESS_SIZE];
u16 press_C2[PRESS_SIZE];
u16 press_AQI[PRESS_SIZE];

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

#ifdef EXTI_GPIOB_4
	//GPIOA.0 中断线以及中断初始化配置   下降沿触发
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);

	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//物理按键
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
#else
	//GPIOA.0 中断线以及中断初始化配置   下降沿触发
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);

	EXTI_InitStructure.EXTI_Line=EXTI_Line2;	//物理按键
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
#endif
}

#ifdef EXTI_GPIOB_4
//外部中断4服务程序 
void EXTI4_IRQHandler(void)
{
	unsigned char it_start;
	it_start = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
	delay_ms(10);//消抖
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)==0 && it_start == 0)	 	 //WK_UP按键
	{				 
		DBG("press");
		press_time = sys_tick;
		press_flag = 1;
		wait_send_press = 0;
	}
	if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)==1) && (press_flag==1) && (it_start == 1))	 	 //WK_UP按键
	{				 
		press_flag = 0;
		//DBG("press");
		press_time = sys_tick - press_time;
		if(press_time < 2000)	//这里是短按的处理，长按的判断和处理放在了TIM2中断中处理，为了实现检测超过2s就立即响应
		{
			DBG("short press");
			switch(*mqtt_mode)
			{
				case '0':
					strcpy(mqtt_mode,"A");
					break;
				case 'A':
					strcpy(mqtt_mode,"1");
					break;
				case '1':
					strcpy(mqtt_mode,"2");
					break;
				case '2':
					strcpy(mqtt_mode,"3");
					break;
				case '3':
					strcpy(mqtt_mode,"4");
					break;
				case '4':
					strcpy(mqtt_mode,"A");
					break;
			}
			ModeCountrol();
			SavePressLog();
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE0上的中断标志位  
}
#else
//外部中断2服务程序 
void EXTI2_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)	 	 //WK_UP按键
	{				 
		DBG("press");
		press_time = sys_tick;
		press_flag = 1;
	}
	if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==1) && (press_flag==1))	 	 //WK_UP按键
	{				 
		press_flag = 0;
		//DBG("press");
		press_time = sys_tick - press_time;
		if(press_time < 2000)	//这里是短按的处理，长按的判断和处理放在了TIM2中断中处理，为了实现检测超过2s就立即响应
		{
			DBG("short press");
			switch(*mqtt_mode)
			{
				case '0':
					strcpy(mqtt_mode,"A");
					break;
				case 'A':
					strcpy(mqtt_mode,"1");
					break;
				case '1':
					strcpy(mqtt_mode,"2");
					break;
				case '2':
					strcpy(mqtt_mode,"3");
					break;
				case '3':
					strcpy(mqtt_mode,"4");
					break;
				case '4':
					strcpy(mqtt_mode,"A");
					break;
			}
			ModeCountrol();
			SavePressLog();
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //清除LINE0上的中断标志位  
}
#endif

void SavePressLog(void)
{
	//if (Fifo_canPush(&recv_fifo1)) Fifo_Push(&recv_fifo1, *mqtt_mode);
	//将按下按键后的状态和按下按键的时间记录在下面的数组中
	if(press_len >= PRESS_SIZE || All_State == sendPM) press_len = 0;
	strcpy(press_buf[press_len], mqtt_mode);
	press_time_log[press_len] = RTC_GetCounter();
	AQI_Count(Conce_PM2_5, Conce_PM10, (int *)&AQI_2_5, (int *)&AQI_10, (int *)&AQI_Max);
	press_C1[press_len] = Conce_PM2_5;
	press_C2[press_len] = Conce_PM10;			
	press_AQI[press_len] = AQI_Max;			
	press_len++;
	wait_send_press = 1;
}
	


