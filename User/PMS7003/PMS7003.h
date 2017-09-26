#include "sys.h"

//甲醛传感器
#define RECV_WAIT 0
#define RECV_42 1
#define RECV_OVER 2

extern vu8 PM2_5_OK;										//pm2.5传感器是否工作的标志
extern volatile int Conce_PM2_5;       // PM2.5浓度
extern volatile int Conce_PM10;        // PM10浓度
extern volatile int Max_PM;
extern volatile int AQI_2_5;
extern volatile int AQI_10;
extern volatile int AQI_Max;								//MAX(AQI_2_5,AQI_10)
extern char PMS7003_RX_BUF[32];
extern u16 pm_check_short;
extern int pm_data_equivalent;
extern u8 pm_state;
extern u8 pm_PMS7003_RX_STA;

//void DSHCHO_Init(unsigned int bps,void (*uart_init)(unsigned int),void (*uart_send)(unsigned char));
void Recive_PM(u8 Res);
void Send_PMS7003_Cmd(void);

