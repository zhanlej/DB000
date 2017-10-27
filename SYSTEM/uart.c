#include "uart.h"


u8 USART_RX_BUF[USART_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.    
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       	//接收状态标记	  
u16 USART_RX_CNT=0;			//接收的字节数	   

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
#ifdef DBG_USART1
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
#elif DBG_USART2
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	USART2->DR = (u8) ch;  
#elif DBG_USART3
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
	USART3->DR = (u8) ch;  
#endif
	return ch;
}
#endif 

//UART1 function
//UART1 TxD GPIOA9   RxD GPIOA10
void USART1Conf(u32 baudRate, u32 nvicPre, u32 nvicSub)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //使能USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//GPIOA时钟
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10 

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=nvicPre ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = nvicSub;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART1 Configure	
	USART_InitStructure.USART_BaudRate = baudRate;//波特率19200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据宽度8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送与接收
	USART_Init(USART1 , &USART_InitStructure);//将初始化好的结构体装入寄存器	
	
	//USART1_INT Configure
	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);//使能接收中断
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

//UART2 function
//UART2 TxD GPIOA2   RxD GPIOA3
void USART2Conf(u32 baudRate, u32 nvicPre, u32 nvicSub)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //使能USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//GPIOA时钟
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART2_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=nvicPre ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = nvicSub;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART2 Configure	
	USART_InitStructure.USART_BaudRate = baudRate;//波特率19200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据宽度8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送与接收
	USART_Init(USART2 , &USART_InitStructure);//将初始化好的结构体装入寄存器	
	
	//USART2_INT Configure
	USART_ITConfig(USART2 , USART_IT_RXNE , ENABLE);//使能接收中断
	USART_Cmd(USART2 , ENABLE);//打开串口
	USART_ClearFlag(USART2 , USART_FLAG_TC);//解决第一个数据发送失败的问题
}

void U2_PutChar(u8 Data)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART2 , Data);
}
void U2_PutStr(char *str)//发送一个字符串
{
	while(*str != '\0')
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完毕
		USART_SendData(USART2 , *str++);
	}
}

void U2_PutNChar(u8 *buf , u16 size)
{
  u8 i;
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); //防止第一字节丢失
	for(i=0;i<size;i++)
	{
		 USART_SendData(USART2 , buf[i]);
		 while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完毕
	}
}

//UART1 function
//UART1 TxD GPIOB10   RxD GPIOB11
void USART3Conf(u32 baudRate, u32 nvicPre, u32 nvicSub)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //使能USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//GPIOA时钟
	
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10
   
  //USART3_RX	  GPIOB.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10 

  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=nvicPre ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = nvicSub;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART3 Configure	
	USART_InitStructure.USART_BaudRate = baudRate;//波特率19200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据宽度8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送与接收
	USART_Init(USART3 , &USART_InitStructure);//将初始化好的结构体装入寄存器	
	
	//USART3_INT Configure
	USART_ITConfig(USART3 , USART_IT_RXNE , ENABLE);//使能接收中断
	USART_Cmd(USART3 , ENABLE);//打开串口
	USART_ClearFlag(USART3 , USART_FLAG_TC);//解决第一个数据发送失败的问题
}

void U3_PutChar(u8 Data)
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
	USART_SendData(USART3 , Data);
}
void U3_PutStr(char *str)//发送一个字符串
{
	while(*str != '\0')
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
		USART_SendData(USART3 , *str++);
	}
}

void U3_PutNChar(u8 *buf , u16 size)
{
  u8 i;
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); //防止第一字节丢失
	for(i=0;i<size;i++)
	{
		 USART_SendData(USART3 , buf[i]);
		 while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
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


