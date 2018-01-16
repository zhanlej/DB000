//注意：由于wifi和串口都占用了较大的内存空间，目前已经尽可能减少内存配次数，如出现异常情况，可以多编译几次然后下载到单片机中
//tip：尽可能不要在函数中做较大的内存分配，建议直接拿到外面以全局变量的方式进行

//对 arduinoESP8266库部分函数由C++移植到C函数，方便51，ARM等C平台调用
//对返回值由原来的true or false 改为返回int型 0表示失败 其他表示成功或其他原因返回
#include <stdlib.h>
#include <string.h>
#include "serialportAPI.h"
#include "ESP8266.h"
#include "stringAPIext.h"
#include "uart.h"
#include "MyFifo.h"
#include "24tft.h"					//for OLED

volatile unsigned long sys_tick = 0;
unsigned char smartconfig_flag = 0;

char data_rec[RECV_BUF_SIZE];

static uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id);
static int sATCIPSEND(void);
static int sATCIPMODE(uint8_t mode);
static int eATUART(uint32_t baud);
static int eATRST(void);
static int eAT(void);
static int eATCWSTARTSMART(uint8_t type, char *link_msg);
static int eATCWSTOPSMART(void);
static int qATCWMODE(uint8_t *mode);
static int sATCWMODE(uint8_t mode);
static int sATCIPMUX(uint8_t mode);
static int sATCWAUTOCONN(uint8_t mode);

static int recvFindAndFilter(const char *target, const char *begin, const char *end, char *data_rec, uint32_t timeout);
static int recvFind(const char *target, uint32_t timeout);
static int recvString(char *rec_data, const char *target, uint32_t timeout);
static int recvString2(char *rec_data, const char *target1, const char *target2, uint32_t timeout);
static int eATCIPSTATUS(char *data_rec);
static int sATCIPSTARTSingle(const char *type, const char *addr, uint32_t port);
static int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len);
static int recvString3(char *rec_data, const char *target1, const char *target2, const char *target3, uint32_t timeout);

/*******************************************************************************
  函 数 名 ：AutoLink
  函数功能 ：自动连接，前10s自动连接，若连接失败则进入smartlink模式30s，若依然失败
             则再次回到自动连接，直到连接成功
  输    入 ：无
  输    出 ：无
*******************************************************************************/
int AutoLink(void)
{
  int status = STATUS_LOSTIP;
	uint32_t start_time = millis();
	
	printf("start auto link\r\n");
	//10s自动连接时间
	while ((millis() - start_time < 10000) && status != STATUS_GETIP)
	{
		status = getSystemStatus();
		delay(1000);
	}

//	//连接失败进入smartlink模式 30s
//	if (status != STATUS_GETIP)
//	{
//		char link_msg[RECV_BUF_SIZE];
//		printf("start smartlink\r\n");
//		stopSmartLink();

//		if (1 == smartLink((uint8_t)ESP_AIR, link_msg))
//		{
//			stopSmartLink(); //无论配网是否成功，都需要释放快连所占的内存
//			printf("%s\r\n", link_msg);
//			start_time = millis();//等待获取IP
//			while ((millis() - start_time < 5000) && status != STATUS_GETIP)
//			{
//				status = getSystemStatus();
//				delay(500);
//			}
//			sATCWAUTOCONN(1); //开启自动连接模式
//		}
//		else
//		{
//			stopSmartLink();
//			delay(500);
//			printf("link AP fail\r\n");
//			restart();
//		}
//	}
	
	if (status == STATUS_GETIP)
		return 1;
	else
		return 0;
}

/*******************************************************************************
  函 数 名 ：SmartConfig
  函数功能 ：开始smartconfig配网模式
  输    入 ：无
  输    出 ：无
*******************************************************************************/
int SmartConfig(void)
{
	int status = STATUS_LOSTIP;
	uint32_t start_time = millis();
	char link_msg[RECV_BUF_SIZE];
	
	printf("start smartlink\r\n");
	stopSmartLink();

	if (1 == smartLink((uint8_t)ESP_AIR, link_msg))
	{
		stopSmartLink(); //无论配网是否成功，都需要释放快连所占的内存
		printf("%s\r\n", link_msg);
		start_time = millis();//等待获取IP
		while ((millis() - start_time < 5000) && status != STATUS_GETIP)
		{
			status = getSystemStatus();
			delay(500);
		}
		sATCWAUTOCONN(1); //开启自动连接模式
		return 1;
	}
	else
	{
		stopSmartLink();
		delay(500);
		printf("link AP fail\r\n");
		return 0;
	}
}

int WifiInit(const char *addr, uint32_t port, char *http_data)
{
	unsigned long start;
	
	WIFI_RST_PIN = 1; //将RST置高
	//delay(2000); //延时等待一会
	start = millis();
	while (millis() - start < 3000)
	{
		if (eAT())
		{
			printf("AT is OK!\r\n");
			delay(1000); /* Waiting for stable */
			break;
		}
		delay(100);
	}
//	if(!restart()) return 0;
//  printf("restart() ok!\r\n");
	if(!setOprToStationSoftAP()) return 0;
  printf("setOprToStationSoftAP() ok!\r\n");
	
	if(smartconfig_flag)
	{
		OLED_wifi_status_set(OLED_WIFI_CONFIG);	//设置为配网模式
		smartconfig_flag = 0;
		if(!SmartConfig()) return 0;
		printf("SmartConfig() ok!\r\n");
	}
	else
	{
		OLED_wifi_status_set(OLED_WIFI_AUTO_CONNECT);	//设置为自动连接模式
		if(!AutoLink()) return 0;
		printf("AutoLink() ok!\r\n");
	}

  if(!disableMUX()) return 0;
	printf("disableMUX() ok!\r\n");
#ifdef TRANS_MODE
	if(!sATCIPMODE(1)) return 0;
	printf("sATCIPMODE = 1 is OK!\r\n");
#endif

  if (!createTCP(addr, port)) return 0;  //连接主机
  printf("create tcp ok\r\n");
	delay(1000);
  if (getSystemStatus() == STATUS_GETLINK)
	{
		if(!sATCIPSEND()) return 0;
		printf("sATCIPSEND is OK!\r\n");
		ClearRxBuf(); //清除串口中所有缓存的数据
    return 1;
	}
  else
    return 0;
}

void ClearRecBuf(void)
{
  data_rec[0] = '\0';
}

//由1ms定时器调用，该定时器需要有较高的优先级
void timer1msINT(void)
{
  sys_tick++;
}

unsigned long millis(void)
{
  return sys_tick;
}

//ms 延时函数，建议 ms值 > 10
void delay(unsigned int ms)
{
  unsigned long start = millis();
  while(millis() - start <= ms);
}

int SetBaud(uint32_t baud)
{
  return eATUART(baud);
}

int restart(void)
{
  unsigned long start;
  if (eATRST())
  {
    delay(2000);
    start = millis();
    while (millis() - start < 3000)
    {
      if (eAT())
      {
        delay(1500); /* Waiting for stable */
        return 1;
      }
      delay(100);
    }
  }
  return 0;
}

//切换到AP+station模式
int setOprToStationSoftAP(void)
{
  uint8_t mode = 0xff;
  if (!qATCWMODE(&mode))
  {
    return 0;
  }
  if (mode == 1)
  {
    return 1;
  }
  else
  {
    if (sATCWMODE(1) && restart())
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

//注意link_msg的大小要足够，防止内存溢出,还回SSID，pwd信息
int smartLink(uint8_t  type, char *link_msg)
{
  return eATCWSTARTSMART(type, link_msg);
}

int stopSmartLink(void)
{
  return eATCWSTOPSMART();
}

int getSystemStatus(void)
{
  int index;
  //  char list[16];
  char *list = malloc(16);//在ARM环境下，如果使用list[16]的方式，会出现出栈时产生异常，改用malloc方式

  eATCIPSTATUS(list);

  index = StringIndex(list, "STATUS:");
  if (index != -1)
  {
    int status;
    index += 7;
    status = (int)(list[index]);
    free(list);
    return (status - 0x30);//返回状态
  }
  else
  {
    free(list);
    return 0;
  }
}

int disableMUX(void)
{
  return sATCIPMUX(0);
}

int createTCP(const char *addr, uint32_t port)
{
  return sATCIPSTARTSingle("TCP", addr, port);
}

int wifi_send(const uint8_t *buffer, uint32_t len)
{
  return sATCIPSENDSingle(buffer, len);
}

int wifi_recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
  int i;
  int len = 0;
  len = recvPkg(buffer, buffer_size, NULL, timeout, NULL);
  //recv_fifo3是为了给MQTT用的
  for(i = 0; i < len; i++)
  {
    if (Fifo_canPush(&recv_fifo3)) Fifo_Push(&recv_fifo3, *buffer);
    buffer++;
  }
  return len;
}

/* +IPD,<id>,<len>:<data> */
/* +IPD,<len>:<data> */

uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id)
{
#ifdef TRANS_MODE
	unsigned long start;
	int ret = 0;
	char a;
	
	start = millis();
	while (millis() - start < timeout)
  {
		if(SerialAvailable() > 0)
    {
      a = SerialRead();
      buffer[ret++] = a;
    }
	}
	
	return ret;
#else
  char a;
  int32_t index_PIPDcomma = 0;
  int32_t index_colon = 0; /* : */
  int32_t index_comma = 0; /* , */
  int32_t len = 0;
  int8_t id = 0;
  int has_data = 0;
  uint32_t ret;
  unsigned long start;
  uint32_t i;
  ClearRecBuf();
  if (buffer == NULL)
  {
    return 0;
  }

  start = millis();
  while (millis() - start < timeout)
  {
    if(SerialAvailable() > 0)
    {
      a = SerialRead();
      StringAddchar(data_rec, a);
    }

    index_PIPDcomma = StringIndex(data_rec, "+IPD,");
    if (index_PIPDcomma != -1)
    {
      index_colon = StringIndexCharOffset(data_rec, ':', index_PIPDcomma + 5);
      if (index_colon != -1)
      {
        index_comma = StringIndexCharOffset(data_rec, ',', index_PIPDcomma + 5);
        /* +IPD,id,len:data */
        if (index_comma != -1 && index_comma < index_colon)
        {
          char str_temp[5];
          StringSubstring(str_temp, data_rec, index_PIPDcomma + 5, index_comma);
          id = atoi(str_temp);
          if (id < 0 || id > 4)
          {
            return 0;
          }
          StringSubstring(str_temp, data_rec, index_comma + 1, index_colon);
          len = atoi(str_temp);
          if (len <= 0)
          {
            return 0;
          }
        }
        else     /* +IPD,len:data */
        {
          char str_temp[5];
          StringSubstring(str_temp, data_rec, index_PIPDcomma + 5, index_colon);
          len = atoi(str_temp);
          if (len <= 0)
          {
            return 0;
          }
        }
        has_data = 1;
        break;
      }
    }
  }

  if (has_data)
  {
    i = 0;
    ret = len > buffer_size ? buffer_size : len;
    start = millis();
    while (millis() - start < 3000)
    {
      while(SerialAvailable() > 0 && i < ret)
      {
        a = SerialRead();
        buffer[i++] = a;
      }
      if (i == ret)
      {
        rx_empty();
        if (data_len)
        {
          *data_len = len;
        }
        if (index_comma != 0 && coming_mux_id)
        {
          *coming_mux_id = id;
        }
        return ret;
      }
    }
  }
  return 0;
#endif
}

void rx_empty(void)
{
//    while(SerialAvailable() > 0) {
//        SerialRead();
//    }
  ClearRxBuf();
}

int sATCIPSEND(void)
{
  rx_empty();
  SerialPrintln("AT+CIPSEND", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int sATCIPMODE(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CIPMODE=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATUART(uint32_t baud)
{
  int int_baud = baud;
  rx_empty();
  SerialPrint("AT+UART=", STRING_TYPE);
  SerialPrint(&int_baud, INT_TYPE);
  SerialPrintln(",8,1,0,0", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATRESTORE(void)
{
  rx_empty();
  SerialPrintln("AT+RESTORE", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATRST(void)
{
  rx_empty();
  SerialPrintln("AT+RST", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eAT(void)
{
  rx_empty();
  SerialPrintln("AT", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

/*******************************************************************************
* 函 数 名 ：eATCWSTARTSMART
* 函数功能 ：启动smartlink模式，需要在30s内连接//add by LC 2016.01.05 16:27
* 输    入 ：type 启动方式 0 -AL-LINK    1 - ESP-TOUCH    2 - AIR-KISS
			link_msg 返回的SSID和PSD
* 输    出 ：无
*******************************************************************************/
int eATCWSTARTSMART(uint8_t type, char *link_msg)
{
  int flag;
  int int_type = type;
  rx_empty();
  SerialPrint("AT+CWSTARTSMART=", STRING_TYPE);
  SerialPrintln(&int_type, INT_TYPE);
  flag = recvFind("OK", TIME_OUT);
  printf("AT+CWSTARTSMART=3 is OK!\r\n");
  if(flag == 0) return flag;
  delay(50);//延时之后等待自动连接
  rx_empty();
  //return recvFindAndFilter("OK", "SMART SUCCESS", "\r\n\r\nOK", link_msg,30000);
  return recvFindAndFilter("smartconfig connected wifi", "Smart get wifi info", "WIFI CONNECTED", link_msg, 60000);
}

//add by LC 2016.01.05 16:27
int eATCWSTOPSMART(void)
{
  rx_empty();
  SerialPrintln("AT+CWSTOPSMART", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int qATCWMODE(uint8_t *mode)
{
  char str_mode[5];
  int ret;
  if (!mode)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+CWMODE?", STRING_TYPE);

  ret = recvFindAndFilter("OK", "+CWMODE:", "\r\n\r\nOK", str_mode, TIME_OUT);
  if (ret != 0)
  {
    *mode = (uint8_t)atoi(str_mode);
    return 1;
  }
  else
  {
    return 0;
  }
}

int sATCWMODE(uint8_t mode)
{
  int int_mode = mode;
  ClearRecBuf();
  rx_empty();
  SerialPrint("AT+CWMODE=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);

//  recvString2(data_rec, "OK", "no change", TIME_OUT);
//  if (StringIndex(data_rec, "OK") != -1 || StringIndex(data_rec, "no change") != -1)
//  {
//    return 0;
//  }
//  return 1;
	return recvFind("OK", 300);
}

int sATCWAUTOCONN(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CWAUTOCONN=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATCIPSTATUS(char *data_list)
{
  //delay(100);//去掉延时 modfied by LC 2016.01.06 23:46
  rx_empty();
  SerialPrintln("AT+CIPSTATUS", STRING_TYPE);
  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", data_list, TIME_OUT);
}

//在接收的字符串中查找 target 并获取 begin 和 end 中间的子串写到data
int recvFindAndFilter(const char *target, const char *begin, const char *end, char *data_get, uint32_t timeout)
{
  ClearRecBuf();
  recvString(data_rec, target, timeout);
  if (StringIndex(data_rec, target) != -1)
  {
    int32_t index1 = StringIndex(data_rec, begin);
    int32_t index2 = StringIndex(data_rec, end);
    if (index1 != -1 && index2 != -1)
    {
      index1 += StringLenth(begin);
      StringSubstring(data_get, data_rec, index1, index2 - 1);
      return 1;
    }
  }
  ClearRecBuf();
  return 0;
}

int sATCIPMUX(uint8_t mode)
{
  int int_mode = mode;

  rx_empty();
  SerialPrint("AT+CIPMUX=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  ClearRecBuf();
  recvString2(data_rec, "OK", "Link is builded", TIME_OUT);
  if (StringIndex(data_rec, "OK") != -1)
  {
    return 1;
  }
  return 0;
}

int sATCIPSTARTSingle(const char *type, const char *addr, uint32_t port)
{
  int int_port = port;
  ClearRecBuf();
  rx_empty();
  SerialPrint("AT+CIPSTART=\"", STRING_TYPE);
  SerialPrint(type, STRING_TYPE);
  SerialPrint("\",\"", STRING_TYPE);
  SerialPrint(addr, STRING_TYPE);
  SerialPrint("\",", STRING_TYPE);
  SerialPrintln(&int_port, INT_TYPE);

  recvString3(data_rec, "OK", "ERROR", "ALREADY CONNECT", 10000);
  if (StringIndex(data_rec, "OK") != -1 || StringIndex(data_rec, "ALREADY CONNECT") != -1)
  {
    return 1;
  }
  return 0;
}

int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len)
{
#ifdef TRANS_MODE
	int i;
	for (i = 0; i < len; i++)
	{
		SerialWrite(buffer[i]);
	}
#else
  uint8_t i;
  int int_len = len;
  rx_empty();
  SerialPrint("AT+CIPSEND=", STRING_TYPE);
  SerialPrintln(&int_len, INT_TYPE);
  if (-1 != recvFind(">", 5000))  //5000
  {
    rx_empty();
    for (i = 0; i < len; i++)
    {
      SerialWrite(buffer[i]);
    }
    return recvFind("SEND OK", 10000);//10000
  }
#endif
  return 0;
}



int recvFind(const char *target, uint32_t timeout)
{
  ClearRecBuf();
  recvString(data_rec, target, timeout);
  if (StringIndex(data_rec, target) != -1)
  {
    return 1;
  }
  return 0;
}

//功能：在串口接收的数据rec_data中索引是否有target字符串。超时时间为timeout
int recvString(char *rec_data, const char *target, uint32_t timeout)
{
  char a;
  unsigned long start;
  start = millis();
  while (millis() - start < timeout)
  {
    while(SerialAvailable() > 0)
    {
      a = SerialRead();
      if(a == '\0') continue; //因为在StringAddchar()中有在最末尾添加'\0'的操作
      StringAddchar(rec_data, a); //将串口读到的数据添加到rec_data中
    }
    if (StringIndex(rec_data, target) != -1)  //最后查找一下target是否在rec_data中
    {
      break;
    }
  }
  return 1;
}

int recvString2(char *rec_data, const char *target1, const char *target2, uint32_t timeout)
{
  char a;
  unsigned long start = millis();
  while (millis() - start < timeout)
  {
    while(SerialAvailable() > 0)
    {
      a = SerialRead();
      if(a == '\0') continue;
      StringAddchar(rec_data, a);
    }
    if (StringIndex(rec_data, target1) != -1)
    {
      break;
    }
    else if (StringIndex(rec_data, target2) != -1)
    {
      break;
    }
  }
  return 1;
}

int recvString3(char *rec_data, const char *target1, const char *target2, const char *target3, uint32_t timeout)
{
  char a;
  unsigned long start = millis();
  while (millis() - start < timeout)
  {
    while(SerialAvailable() > 0)
    {
      a = SerialRead();
      if(a == '\0') continue;
      StringAddchar(rec_data, a);
    }
    if (StringIndex(rec_data, target1) != -1)
    {
      break;
    }
    else if (StringIndex(rec_data, target2) != -1)
    {
      break;
    }
    else if (StringIndex(rec_data, target3) != -1)
    {
      break;
    }
  }
  return 1;
}

void WIFI_restart(void)
{
	unsigned long start;
	
	printf("WIFI_restart()\r\n");
	WIFI_RST_PIN = 0;
	delay(1000);
	WIFI_RST_PIN = 1;
	//delay(2000);
	start = millis();
	while (millis() - start < 3000)
	{
		if (eAT())
		{
			delay(1000); /* Waiting for stable */
			printf("WIFI_restart() is OK!\r\n");
			return;
		}
		delay(100);
	}
}
	
int esp8266_send(const uint8_t *buffer, uint32_t len)
{
	printf("---------- esp8266_send len = %d ----------\r\n", len);
  return sATCIPSENDSingle(buffer, len);
}

int esp8266_recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
	int i;
  int len = 0;
	memset(buffer, 0, buffer_size);	//为了清空原来的数据
	
  len = recvPkg(buffer, buffer_size, NULL, timeout, NULL);
  //recv_fifo3是为了给MQTT用的
  for(i = 0; i < len; i++)
  {
    if (Fifo_canPush(&recv_fifo3)) Fifo_Push(&recv_fifo3, *buffer);
    buffer++;
  }
	if(len != 0)
	{
		printf("########## esp8266_recv len = %d ##########\r\n", len);
	}
  return len;
}
	
int mqtt_recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
	int i;
	int len;
	for(i = 0; i < 5; i++)
	{
		if((len = esp8266_recv(buffer, buffer_size, timeout)) != 0) break;
	}
	
	return len;
}










