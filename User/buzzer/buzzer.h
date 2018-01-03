#ifndef __BUZZER_H
#define __BUZZER_H

/* #####   HEADER FILE INCLUDES   ################################################### */
#include "sys.h"

/* #####   EXPORTED MACROS   ######################################################## */
#define BUZZER_PIN_F PAout(11)
#define BUZZER_PIN_V PAout(12)
#define BUZZER_V_OFF 0
#define BUZZER_V_ON 1
#define BEEP_TIME 200
//#define ACTIVE_BEEP 1

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */
typedef enum
{
	SYSTEM_CLK = 72000000,
	BEEP_TIMER_PERIOD = 72,
	BEEP_TIMER_PWM_PLUSE = BEEP_TIMER_PERIOD/2
} timer_param_enum;

typedef enum
{
	BEEP_STATUS_STOP,       /* 蜂鸣器停止工作 */
	BEEP_STATUS_START,			/* 蜂鸣器开始工作 */
	BEEP_STATUS_KEEP,				/* 蜂鸣器正在工作 */
} beep_status_enum;

typedef enum
{
	BEEP_ON,				/* 开机时的声音 */
	BEEP_OFF,				/* 关机时的声音 */
	BEEP_CMD,       /* 接收指令的声音 */
	BEEP_CONNECT		/* 设备连接上网络后的声音 */
} beep_tpye_enum;

typedef enum
{
	F2_3 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/23,		/* 1000/2.3 */
	F2_6 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/26,		/* 1000/2.6 */
	F2_9 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/29,		/* 1000/2.9 */
	F4 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/40,			/* 1000/4 */
	F4_4 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/44,			/* 1000/4 */
	F3_7 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/37,			/* 1000/4 */
	F3_4 = SYSTEM_CLK/BEEP_TIMER_PERIOD/100/34			/* 1000/4 */
} freq_enum;

typedef struct _note_
{
	freq_enum freq;	/* 音符的频率 */
	u32 Tv;					/* 压给定持续时间（ms） */
	u32 Tf;					/* 频率给定持续时间（ms） */
} note_t;

typedef struct _beep_play_
{
	beep_status_enum beep_status;				/* 通知定时器中断服务函数 */
	beep_tpye_enum type;	/* 蜂鸣器发音的类型 */
	u8 note_num;					/* 播放音符的数量 */
	note_t note[4];				/* 播放音符的数组 */
} beep_play_t;

/* #####   EXPORTED DATA TYPES   #################################################### */
extern beep_play_t beep_play;

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
void TIM1_Int_Init(void);
void beep_init(void);
void beep_on(beep_tpye_enum beep_type);
void beep_handle(unsigned long current_tick);

#endif

