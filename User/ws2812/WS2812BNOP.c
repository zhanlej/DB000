#include "WS2812BNOP.h"
#include "stm32f10x.h"
#include "delay.h"


void RGB_Set_Up(void)
{
	int i;
	//GPIO_SetBits(GPIOB, GPIO_Pin_12);
	GPIOB->BSRR = 0x1000;
	//delay_us_div9(21600);
	for(i = 0; i < 5; i++)
	{
		__nop();
	}
	//GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIOB->BRR = 0x1000;
//	//delay_us_div9(14400);	
////	for(i = 0; i < 2; i++)
////	{
////		____nop();
////	}
//	____nop();
}

void RGB_Set_Down(void)
{
	int i = 0;
	
	//GPIO_SetBits(GPIOB, GPIO_Pin_12);
	GPIOB->BSRR = 0x1000;
	for(i = 0; i < 2; i++)
	{
		__nop();
	}
	//delay_us_div9(1);
	//GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIOB->BRR = 0x1000;
	for(i = 0; i < 4; i++)
	{
		__nop();
	}
	//delay_us_div9(1);	
}

void RGB_Set(u32 G8R8B8, int len)
{
	int i;
	int j;
	//u8 byte=0;
	for(j=0; j<len; j++)
	{
		for(i=23; i>=0; i--)
		{
			//byte = ((G8R8B8>>i)&0x1);
			GPIOB->BSRR = 0x1000;
			if((G8R8B8>>i)&0x1)
			{
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				GPIOB->BRR = 0x1000;
				__nop();__nop();__nop();__nop();__nop();
			}
			else
			{
				__nop();__nop();__nop();
				GPIOB->BRR = 0x1000;
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();
			}
		}
	}
}

void RGB_Rst(void)
{
	//GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIOB->BRR = 0x1000;
	delay_us(100);
}


