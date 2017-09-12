#include "uart1.h"
#include "interface.h"

char DBG_BUF[DBG_BUF_SIZE];

//UART function
//UART3 TxD GPIOB10   RxD GPIOB11
void USART1Conf(u32 baudRate)
{
	USART_InitTypeDef USART_InitSturct;//定义串口1的初始化结构体

//USART3 Configure	
	USART_InitSturct.USART_BaudRate = baudRate;//波特率19200
	USART_InitSturct.USART_WordLength = USART_WordLength_8b;//数据宽度8位
	USART_InitSturct.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitSturct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitSturct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitSturct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送与接收
	USART_Init(USART1 , &USART_InitSturct);//将初始化好的结构体装入寄存器	
	//USART1_INT Configure
	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);//使能接收中断
//	USART_ITConfig(USART3 , USART_IT_TXE , ENABLE);
	USART_Cmd(USART1 , ENABLE);//打开串口
	USART_ClearFlag(USART1 , USART_FLAG_TC);//解决第一个数据发送失败的问题
}

void U1_PutChar(u8 Data)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART1 , Data);
}
void U1_PutStr(char *str)//发送一个字符串
{
	while(*str != '\0')
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等待发送完毕
		USART_SendData(USART1 , *str++);
	}
}

void U1_PutNChar(u8 *buf , u16 size)
{
  u8 i;
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //防止第一字节丢失
	for(i=0;i<size;i++)
	{
		 USART_SendData(USART1 , buf[i]);
		 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等待发送完毕
	}
}

void U2_PutDbgStrln(char *str)//发送一个字符串并换行
{
	while(*str != '\0')
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完毕
		USART_SendData(USART2 , *str++);
	}		
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART2 , '\r');
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART2 , '\n');
}

void U3_PutDbgStrln(char *str)//发送一个字符串并换行
{
	while(*str != '\0')
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
		USART_SendData(USART3 , *str++);
	}		
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART3 , '\r');
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART3 , '\n');
}
