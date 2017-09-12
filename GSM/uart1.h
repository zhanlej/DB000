#ifndef __UART_H_
#define __UART_H_

#include "stm32f10x.h"

//#define DBG U2_PutDbgStrln
#define DBG U3_PutDbgStrln
#define DBG_BUF_SIZE 1024
extern char DBG_BUF[DBG_BUF_SIZE];

void USART1Conf(u32 baudRate);
void U1_PutChar(u8 Data);
void U1_PutNChar(u8 *buf , u16 size);
void U1_PutStr(char *str);
void U2_PutDbgStrln(char *str);
void U3_PutDbgStrln(char *str);
//void USART1Conf(u32 baudRate);
#endif
