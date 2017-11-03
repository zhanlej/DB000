#ifndef __RTC_H
#define __RTC_H	    
//Mini STM32开发板
//RTC实时时钟 驱动代码			 
//正点原子@ALIENTEK
//2010/6/6

//如果想使用外置电池来实现断电依旧有效的RTC功能，必须使用RCC_LSE宏
#define RCC_LSE
//#define RCC_LSI
//#define RCC_HSE
#define UNIX_timestamp	//使用UNIX时间戳，1970.1.1 8:00:00开始

#include "sys.h"

//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;
}_calendar_obj;					 
extern _calendar_obj calendar;	//日历结构体
extern _calendar_obj calendar_tmp;

extern u8 const mon_table[12];	//月份日期数据表
void Disp_Time(u8 x,u8 y,u8 size);//在制定位置开始显示时间
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);//在指定位置显示星期
u8 RTC_Init(u32 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);        //初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);//平年,闰年判断
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 RTC_Get(void);         //更新时间   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间			 
u8 count2date(u32 current_count);
int timeout_SET(u32 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);	//设置time_out时间
int UNIXtime2date(u32 UNIXtime);
#endif


