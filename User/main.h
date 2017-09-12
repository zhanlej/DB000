#include "sys.h"

//#define SWIO_DEBUG

#define BEEP_TIME 100
//#define ACTIVE_BEEP 1

//ALL_Status
#define initialWAITOK -1
#define initialTCP -2
#define initialMQTT -3
#define sendPM -4

//控制电机速度
#define MOTORSPEED0 0x0
#define MOTORSPEED1 0x6
#define MOTORSPEED2 0xa
#define MOTORSPEED3 0xc
#define MOTORSPEED4 0x1

//自动档的临界值
#define PM2_5_LEVEL1 50
#define PM2_5_LEVEL2 100
#define PM2_5_LEVEL3 150
#define LEVEL_OFFSET 10

//LED
#define SIGNAL_LED PBout(3)
#define AUTO_LED PBout(5)
#define SLEEP_LED PBout(6)
#define SPEED1_LED PBout(7)
#define SPEED2_LED PBout(8)
#define SPEED3_LED PBout(9)


void USART2_Config(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void TIM1_Int_Init(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void beep_on(unsigned int time);

//通过状态机的方式控制整个代码流程
void restart_MCU(void);
void Initial_GSM(void);
void Initial_MQTT(void);
void MQTT_Sub0Pub1(void);
int Public_Open(int time);	//发布开机指令
void Transmission_State(void);
//int fifo1readdata(unsigned char* s, int maxlen);
//int fifo3readdata(unsigned char* s, int maxlen);
void SendJson(u8 mode);
void SendPress(void);
int SendPingPack(int times);
void recv_mqtt(unsigned char* recv_data, int data_len, char* return_data, int* return_len);	//接收mqtt的数据

void ModeCountrol(void);
void LedCountrol(unsigned short mode); //控制5个模式LED灯
void SetMotorLevel(int cmd);
void MotorCountrol(unsigned char level); //马达控制函数
void FanTest(char * fan_test);
void auto_mode(unsigned char *level); //自动调速模式
void AirLEDControl(void);	//控制空气质量灯
