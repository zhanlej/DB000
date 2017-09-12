#include "sys.h"

//group_json的模式选择
#define CONNECTION_MODE 0
#define GEO_MODE 1
#define SENDDATA_MODE 2
#define PRESS_MODE 3
#define GPRS_RECORD_MODE 4

//发送数据的时间间隔
#define CLOSE_INTERVAL 600
#define OPEN_INTERVAL 300
#define COUNT_INTERVAL 30
#define PING_SET 120
#define PING_INTERVAL 120
#define AUTOMODE_INTERVAL 5
//#define CLOSE_INTERVAL 120
//#define OPEN_INTERVAL 60
//#define COUNT_INTERVAL 6
//#define PING_SET 120
//#define PING_INTERVAL 15
//#define AUTOMODE_INTERVAL 5

//断网测试记录
#define GPRS_STATE_TIME_SIZE 50


//从内部引用到外部的
extern u16 CSQ[CLOSE_INTERVAL/COUNT_INTERVAL];
extern u16 C1[CLOSE_INTERVAL/COUNT_INTERVAL];
extern u16 C2[CLOSE_INTERVAL/COUNT_INTERVAL];
extern u16 AQI1[CLOSE_INTERVAL/COUNT_INTERVAL];
extern u16 AQI2[CLOSE_INTERVAL/COUNT_INTERVAL];
extern u16 AQI[CLOSE_INTERVAL/COUNT_INTERVAL];
extern u8 L[CLOSE_INTERVAL/COUNT_INTERVAL];
//断网测试记录用
extern unsigned char gprs_connect_cnt;
extern unsigned char gprs_break_cnt;
extern int gprs_connect_time[GPRS_STATE_TIME_SIZE];
extern int gprs_break_time[GPRS_STATE_TIME_SIZE];
extern u32 break_time;

void group_json(unsigned char mode);

