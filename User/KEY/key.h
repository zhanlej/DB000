#include "sys.h"

/* 按键滤波时间50ms, 单位10ms
 *只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
 */
#define BUTTON_FILTER_TIME         5
#define BUTTON_LONG_TIME         200                /* 持续2秒，认为长按事件 */
#define KEY_POWER GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define KEY_MODE GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define PRESS_SIZE 20

typedef enum
{
  KEY_TPYE_POWER = 0,                        /* 表示power按键 */
	KEY_TPYE_MODE               			         /* 表示mode按键 */
} KEY_TPYE_ENUM;

typedef enum
{
  KEY_NONE = 0,                        /* 0 表示按键事件 */

  KEY_DOWN_Power,                        /* 按键键按下 */
  KEY_UP_Power,                        /* 按键键弹起 */
  KEY_LONG_Power,                        /* 按键键长按 */

  KEY_DOWN_Power_TAMPER        /* 组合键，Power键和WAKEUP键同时按下 */
} KEY_ENUM;

/*
        每个按键对应1个全局的结构体变量。
        其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct
{
  /* 下面是一个函数指针，指向判断按键手否按下的函数 */
	KEY_TPYE_ENUM  key_type;           /* 按键的类型 */
  unsigned char  (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */
  unsigned char  Count;                        /* 滤波器计数器 */
  unsigned char  FilterTime;                /* 滤波时间(最大255,表示2550ms) */
  unsigned short LongCount;                /* 长按计数器 */
  unsigned short LongTime;                /* 按键按下持续时间, 0表示不检测长按 */
  unsigned char   State;                        /* 按键当前状态（按下还是弹起） */
  unsigned char  KeyCodeUp;                /* 按键弹起的键值代码, 0表示不检测按键弹起 */
  unsigned char  KeyCodeDown;        /* 按键按下的键值代码, 0表示不检测按键按下 */
  unsigned char  KeyCodeLong;        /* 按键长按的键值代码, 0表示不检测长按 */
  unsigned char  RepeatSpeed;        /* 连续按键周期 */
  unsigned char  RepeatCount;        /* 连续按键计数器 */
	unsigned char  IsLong;       			 /* 判断是不是长按 */
	unsigned char  IsComb;      			 /* 判断是否有组合键发生 */
} BUTTON_T;

typedef struct
{
	unsigned char ChildLock_flag;	//童锁标示
	unsigned char Comb_flag;			//组合按键标示
} KEY_FLAG_T;


//extern BUTTON_T s_Powerkey;
extern KEY_FLAG_T key_flag;
extern char mqtt_mode[2]; //通过mqtt接收到的指令
//extern short All_State;	//为了过滤正常连接时多次按键产生的数组
//extern volatile int Conce_PM2_5;       // PM2.5浓度
//extern volatile int Conce_PM10;        // PM10浓度
//extern volatile int AQI_2_5;
//extern volatile int AQI_10;
//extern volatile int AQI_Max;								//MAX(AQI_2_5,AQI_10)

//extern unsigned char wait_send_press;
//extern int press_len;
//extern char press_buf[PRESS_SIZE][2];
//extern u32 press_time_log[PRESS_SIZE];
////extern u32 press_HCHO[PRESS_SIZE];
//extern u32 press_C1[PRESS_SIZE];
//extern u32 press_C2[PRESS_SIZE];
//extern u32 press_AQI[PRESS_SIZE];

void Panakey_Init(void);
void Pannelkey_Polling(void);
