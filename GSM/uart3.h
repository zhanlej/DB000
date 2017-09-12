//#ifndef __UART3_H_
//#define __UART3_H_

#include "stm32f10x.h"

//#define DBG U2_PutDbgStrln
//#define DBG U1_PutDbgStrln
//#define DBG_BUF_SIZE 512
//extern char DBG_BUF[DBG_BUF_SIZE];

void USART3Conf(u32 baudRate);
void U3_PutChar(u8 Data);
void U3_PutNChar(u8 *buf , u16 size);
void U3_PutStr(char *str);
//void U2_PutDbgStrln(char *str);
//void U1_PutDbgStrln(char *str);
//void USART1Conf(u32 baudRate);
//#endif
