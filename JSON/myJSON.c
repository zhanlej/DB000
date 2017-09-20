#include "myJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cJSON.h"
#include "key.h"
#include "sim800c.h"
#include "stmflash.h"
#include "uart1.h"
#include "rtc.h"
#include "DSHCHO.h"

void SendDataMode_group(cJSON *root);
void ConnectionMode_group(cJSON *root);
void NoPress_Connection_group(cJSON *root);
void PressMode_group(cJSON *root);
void GprsRecordMode_group(cJSON *root);

//从外部引用到内部的
extern volatile int Conce_PM2_5;       // PM2.5浓度
extern volatile int Conce_PM10;        // PM10浓度
extern volatile int AQI_Max;
extern char http_buf[512];
extern unsigned int current_interval;
extern char mqtt_mode[2];
extern volatile unsigned char fan_level;
extern char payload[MQTT_SEND_SIZE];

int CSQ[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int HCHO[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int C1[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int C2[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int AQI1[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int AQI2[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int AQI[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
int L[CLOSE_INTERVAL / COUNT_INTERVAL] = {0};
//断网测试记录用
unsigned char gprs_connect_cnt = 0;
unsigned char gprs_break_cnt = 0;
int gprs_connect_time[GPRS_STATE_TIME_SIZE];
int gprs_break_time[GPRS_STATE_TIME_SIZE];
u32 break_time;

void SendDataMode_group(cJSON *root)
{
//  int i;
  int trans_size;
  cJSON *aqi = NULL;
//  cJSON *j_csq = NULL;
//  cJSON *j_c1 = NULL;
//  cJSON *j_c2 = NULL;
//  cJSON *j_aqi1 = NULL;
//  cJSON *j_aqi2 = NULL;
//  cJSON *j_aqi = NULL;
//  cJSON *j_l = NULL;

  trans_size = current_interval / COUNT_INTERVAL;

  cJSON_AddItemToObject(root, "aqi", aqi = cJSON_CreateObject());
//		cJSON_AddItemToObject(root, "CSQ", cJSON_CreateIntArray((const int *)CSQ, trans_size));
  cJSON_AddStringToObject(aqi, "mode", (const char *)mqtt_mode);

//  cJSON_AddItemToObject(root, "CSQ", j_csq = cJSON_CreateArray());
//  cJSON_AddItemToObject(aqi, "C1", j_c1 = cJSON_CreateArray());
//  cJSON_AddItemToObject(aqi, "C2", j_c2 = cJSON_CreateArray());
//  cJSON_AddItemToObject(aqi, "AQI1", j_aqi1 = cJSON_CreateArray());
//  cJSON_AddItemToObject(aqi, "AQI2", j_aqi2 = cJSON_CreateArray());
//  cJSON_AddItemToObject(aqi, "AQI", j_aqi = cJSON_CreateArray());
//  cJSON_AddItemToObject(aqi, "L", j_l = cJSON_CreateArray());
//  for(i = 0; i < trans_size; i++)
//  {
//    cJSON_AddItemToArray(j_csq, cJSON_CreateNumber(CSQ[i]));
//    cJSON_AddItemToArray(j_c1, cJSON_CreateNumber(C1[i]));
//    cJSON_AddItemToArray(j_c2, cJSON_CreateNumber(C2[i]));
//    cJSON_AddItemToArray(j_aqi1, cJSON_CreateNumber(AQI1[i]));
//    cJSON_AddItemToArray(j_aqi2, cJSON_CreateNumber(AQI2[i]));
//    cJSON_AddItemToArray(j_aqi, cJSON_CreateNumber(AQI[i]));
//    cJSON_AddItemToArray(j_l, cJSON_CreateNumber(L[i]));
//  }

	cJSON_AddItemToObject(root, "CSQ", cJSON_CreateIntArray(CSQ, trans_size));
	cJSON_AddItemToObject(aqi, "HCHO", cJSON_CreateIntArray(HCHO, trans_size));
	cJSON_AddItemToObject(aqi, "C1", cJSON_CreateIntArray(C1, trans_size));
	cJSON_AddItemToObject(aqi, "C2", cJSON_CreateIntArray(C2, trans_size));
	cJSON_AddItemToObject(aqi, "AQI1", cJSON_CreateIntArray(AQI1, trans_size));
	cJSON_AddItemToObject(aqi, "AQI2", cJSON_CreateIntArray(AQI2, trans_size));
	cJSON_AddItemToObject(aqi, "AQI", cJSON_CreateIntArray(AQI, trans_size));
	cJSON_AddItemToObject(aqi, "L", cJSON_CreateIntArray(L, trans_size));



  if(fan_level == 0) current_interval = CLOSE_INTERVAL;
  else current_interval = OPEN_INTERVAL;

  memset(C1, 0, sizeof(C1));	//每次发送完之后需要将数组清零
  memset(C2, 0, sizeof(C2));	//每次发送完之后需要将数组清零
  memset(AQI1, 0, sizeof(AQI1));	//每次发送完之后需要将数组清零
  memset(AQI2, 0, sizeof(AQI2));	//每次发送完之后需要将数组清零
  memset(AQI, 0, sizeof(AQI));	//每次发送完之后需要将数组清零
  memset(L, 0, sizeof(L));	//每次发送完之后需要将数组清零
}

void ConnectionMode_group(cJSON *root)
{
  cJSON *j_connection = NULL;

  cJSON_AddItemToObject(root, "connection", j_connection = cJSON_CreateObject());

  if(wait_send_press)
  {
    PressMode_group(j_connection);
    wait_send_press = 0;
  }
  else
  {
    NoPress_Connection_group(j_connection);
  }
}

void NoPress_Connection_group(cJSON *root)
{
	cJSON *j_hcho = NULL;
  cJSON *j_c1 = NULL;
  cJSON *j_c2 = NULL;
  cJSON *j_aqi = NULL;

  cJSON_AddStringToObject(root, "mode", (const char *)mqtt_mode);

	cJSON_AddItemToObject(root, "HCHO", j_hcho = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "C1", j_c1 = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "C2", j_c2 = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "AQI", j_aqi = cJSON_CreateArray());
	cJSON_AddItemToArray(j_hcho, cJSON_CreateNumber(Conce_HCHO));
  cJSON_AddItemToArray(j_c1, cJSON_CreateNumber(Conce_PM2_5));
  cJSON_AddItemToArray(j_c2, cJSON_CreateNumber(Conce_PM10));
  cJSON_AddItemToArray(j_aqi, cJSON_CreateNumber(AQI_Max));
}

void PressMode_group(cJSON *root)
{
  int i;
  cJSON *j_press = NULL;
  cJSON *j_press_time = NULL;
	cJSON *j_hcho = NULL;
  cJSON *j_c1 = NULL;
  cJSON *j_c2 = NULL;
  cJSON *j_aqi = NULL;

  cJSON_AddItemToObject(root, "press", j_press = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "time", j_press_time = cJSON_CreateArray());
	cJSON_AddItemToObject(root, "HCHO", j_hcho = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "C1", j_c1 = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "C2", j_c2 = cJSON_CreateArray());
  cJSON_AddItemToObject(root, "AQI", j_aqi = cJSON_CreateArray());
  for(i = 0; i < press_len; i++)
  {
    cJSON_AddItemToArray(j_press, cJSON_CreateString(press_buf[i]));
//			count2date(press_time_log[i]);
//			sprintf(DBG_BUF, "%04d%02d%02d%02d%02d%02d", calendar_tmp.w_year, calendar_tmp.w_month, calendar_tmp.w_date, calendar_tmp.hour, calendar_tmp.min, calendar_tmp.sec);
//			DBG(DBG_BUF);
//			cJSON_AddItemToArray(press_time, cJSON_CreateString(DBG_BUF));
    cJSON_AddItemToArray(j_press_time, cJSON_CreateNumber(press_time_log[i]));
		cJSON_AddItemToArray(j_hcho, cJSON_CreateNumber(press_HCHO[i]));
    cJSON_AddItemToArray(j_c1, cJSON_CreateNumber(press_C1[i]));
    cJSON_AddItemToArray(j_c2, cJSON_CreateNumber(press_C2[i]));
    cJSON_AddItemToArray(j_aqi, cJSON_CreateNumber(press_AQI[i]));
  }
  memset(press_buf, 0, sizeof(press_buf));	//每次发送完之后需要将数组清零
  memset(press_time_log, 0, sizeof(press_time_log));	//每次发送完之后需要将数组清零
	memset(press_HCHO, 0, sizeof(press_HCHO));	//每次发送完之后需要将数组清零
  memset(press_C1, 0, sizeof(press_C1));	//每次发送完之后需要将数组清零
  memset(press_C2, 0, sizeof(press_C2));	//每次发送完之后需要将数组清零
  memset(press_AQI, 0, sizeof(press_AQI));	//每次发送完之后需要将数组清零
  press_len = 0;
}

void GprsRecordMode_group(cJSON *root)
{
  //eATCSQ(&sim_csq);	//获取当前csq值
  cJSON_AddNumberToObject(root, "CSQ", sim_csq);
  cJSON_AddNumberToObject(root, "current_date", UNIXtime2date(RTC_GetCounter()));
  cJSON_AddNumberToObject(root, "current_time", RTC_GetCounter());
  //cJSON_AddNumberToObject(root, "time_out", UNIXtime2date(BKP_ReadBackupRegister(BKP_DR2) +  (BKP_ReadBackupRegister(BKP_DR3)<<16)));
  cJSON_AddNumberToObject(root, "time_out", Get_Flash_TimeOut(FLASH_SAVE_ADDR));
  cJSON_AddBoolToObject(root, "childLock", s_Powerkey.ChildLock_flag);
  cJSON_AddItemToObject(root, "connect", cJSON_CreateIntArray(gprs_connect_time, gprs_connect_cnt));
  cJSON_AddItemToObject(root, "break", cJSON_CreateIntArray(gprs_break_time, gprs_break_cnt));
  cJSON_AddNumberToObject(root, "C1", Conce_PM2_5);
  cJSON_AddNumberToObject(root, "AQI_MAX", AQI_Max);
}

void group_json(unsigned char mode)
{
  cJSON *root = NULL;
  cJSON *http_root = NULL;
  char *out = NULL;
  int JSON_len = 0;

  root = cJSON_CreateObject();

  switch(mode)
  {
    case CONNECTION_MODE:
      ConnectionMode_group(root);
      break;
    case GEO_MODE:
      http_root = cJSON_Parse(http_buf);
      if(http_root != NULL)	cJSON_AddItemReferenceToObject(root, "geo", http_root);
      break;
    case SENDDATA_MODE:
      SendDataMode_group(root);
      break;
    case PRESS_MODE:
      PressMode_group(root);
      break;
    case GPRS_RECORD_MODE:
      GprsRecordMode_group(root);
      break;
    default:
      DBG("group_json 's mode is error!");
      cJSON_Delete(root);
      return;
  }

  out = cJSON_Print(root);
  JSON_len = strlen(out);
  strcpy(payload, out);

  sprintf(DBG_BUF, "JSON_len = %d", JSON_len);
  DBG(DBG_BUF);
  //DBG(out);

  free(out);
  if(mode == GEO_MODE) cJSON_Delete(http_root);
  cJSON_Delete(root);
}





