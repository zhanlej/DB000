#include "PMS7003.h"
#include "uart1.h"
#include "stdio.h"
#include "aqi.h"

vu8 PM2_5_OK = 0;										//pm2.5传感器是否工作的标志
volatile int Conce_PM2_5 = 45;       // PM2.5浓度
volatile int Conce_PM10 = 55;        // PM10浓度
volatile int Max_PM = 55;
volatile int AQI_2_5 = 47;
volatile int AQI_10 = 46;
volatile int AQI_Max = 47;								//MAX(AQI_2_5,AQI_10)
char PMS7003_RX_BUF[32];
u16 pm_check_short;
int pm_data_equivalent;
u8 pm_state;
u8 pm_PMS7003_RX_STA;

//static void (*PMS7003_UartInit)(unsigned int);
//static void (*PMS7003_UartSend)(unsigned char);

//void PMS7003_Init(unsigned int bps,void (*uart_init)(unsigned int),void (*uart_send)(unsigned char))
//{
//	PMS7003_UartInit = uart_init;
//	PMS7003_UartSend = uart_send;
//	PMS7003_UartInit(bps);
//}


void Recive_PM(u8 Res)
{
	int i;
//	u16 pm_check_short;
//	int pm_data_equivalent;
//	static u8 pm_state;
//	static u8 pm_PMS7003_RX_STA;
	
	if(pm_state == RECV_WAIT && Res == 0x42)             //如果接收的第一位数据是0X42
	{
		pm_state = RECV_42;              //变量用于确定第二位是否接收到了0X4D
		pm_PMS7003_RX_STA = 0;    //让数组索引值从0开始
		PMS7003_RX_BUF[pm_PMS7003_RX_STA] = Res;
		pm_PMS7003_RX_STA++;
	}
	else if(pm_state == RECV_42)
	{
		PMS7003_RX_BUF[pm_PMS7003_RX_STA] = Res;
		if(pm_PMS7003_RX_STA == 1 && (Res!=0x4d)) pm_state = RECV_WAIT; //第二个字节不是4d
		else if(pm_PMS7003_RX_STA == 2 && (Res!=0x0)) pm_state = RECV_WAIT; //帧长度不是28
		else if(pm_PMS7003_RX_STA == 3 && (Res!=0x1c)) pm_state = RECV_WAIT; //帧长度不是28
		else if(pm_PMS7003_RX_STA >= 31) pm_state = RECV_OVER;
		pm_PMS7003_RX_STA++;
	}
	else
	{
		pm_state = RECV_WAIT; //如果所有条件都不满足则重置pm_state等待接收数据头
	}
	
	if(pm_state == RECV_OVER)	//接收完所有数据
	{
		pm_check_short = 0;
		//计算校验位
		for(i = 0; i < 30; i++)
		{
			pm_check_short += PMS7003_RX_BUF[i];
		}
		if(pm_check_short == ((PMS7003_RX_BUF[30]<<8)+PMS7003_RX_BUF[31]))
		{
			Conce_PM2_5 = (PMS7003_RX_BUF[12] << 8) + PMS7003_RX_BUF[13];
			Conce_PM10  = (PMS7003_RX_BUF[14] << 8) + PMS7003_RX_BUF[15];
			Max_PM = (Conce_PM2_5 > Conce_PM10) ? Conce_PM2_5 : Conce_PM10;
			PM2_5_OK = 1;
			AQI_Count(Conce_PM2_5, Conce_PM10, (int *)&AQI_2_5, (int *)&AQI_10, (int *)&AQI_Max);
			sprintf(DBG_BUF, "PMS7003_RX_BUF = %x %x %x %x, Conce_PM2_5 = %d, Conce_PM10 = %d", PMS7003_RX_BUF[12], PMS7003_RX_BUF[13], PMS7003_RX_BUF[14], PMS7003_RX_BUF[15], Conce_PM2_5, Conce_PM10);
			//DBG(DBG_BUF);
			pm_state = RECV_WAIT;
		}
		else
		{
			//DBG("PM2.5 pm_check_short is error!");
			return;
		}
	}

	
}
	
void Send_PMS7003_Cmd(void)
{
	int i = 0;
	u8 PM_CMD1[7] = {0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74};
	//u8 PM_CMD2[7] = {0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71};
	for(i = 0; i < 7; i++)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等待发送完毕
		USART_SendData(USART3 , PM_CMD1[i]);
//		PMS7003_UartSend(PM_CMD1[i]);
	}
}



