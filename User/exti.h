#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"
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
//物理按键数组最大长度
#define PRESS_SIZE 50
#define EXTI_GPIOB_4

void EXTIX_Init(void);//外部中断初始化		
void SavePressLog(void);

extern char mqtt_mode[2]; //通过mqtt接收到的指令
extern short All_State;	//为了过滤正常连接时多次按键产生的数组
extern volatile int Conce_PM2_5;       // PM2.5浓度
extern volatile int Conce_PM10;        // PM10浓度
extern volatile int AQI_2_5;
extern volatile int AQI_10;
extern volatile int AQI_Max;								//MAX(AQI_2_5,AQI_10)

extern unsigned long press_time;
extern unsigned char press_flag;
extern unsigned char wait_send_press;
extern int press_len;
extern char press_buf[PRESS_SIZE][2];
extern u32 press_time_log[PRESS_SIZE];
extern u16 press_C1[PRESS_SIZE];
extern u16 press_C2[PRESS_SIZE];
extern u16 press_AQI[PRESS_SIZE];
#endif

