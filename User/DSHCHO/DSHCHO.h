#include "sys.h"

//¼×È©´«¸ÐÆ÷
#define RECV_WAIT 0
#define RECV_42 1
#define RECV_OVER 2

extern float Conce_HCHO;
extern char DSHCHO_RX_BUF[10];
extern u16 check_short;
extern int data_equivalent;
extern u8 state;
extern u8 DSHCHO_RX_STA;

void DSHCHO_Init(unsigned int bps,void (*uart_init)(unsigned int),void (*uart_send)(unsigned char));
void Recive_HCHO(u8 Res);
void Send_DSHCHO_Cmd(void);

