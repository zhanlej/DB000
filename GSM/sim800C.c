//注意：由于wifi和串口都占用了较大的内存空间，目前已经尽可能减少内存配次数，如出现异常情况，可以多编译几次然后下载到单片机中
//tip：尽可能不要在函数中做较大的内存分配，建议直接拿到外面以全局变量的方式进行

//对 arduinoESP8266库部分函数由C++移植到C函数，方便51，ARM等C平台调用
//对返回值由原来的true or false 改为返回int型 0表示失败 其他表示成功或其他原因返回
#include <stdlib.h>
#include "serialportAPI.h"
#include "sim800C.h"
#include "stringAPIext.h"
#include "uart.h"
#include "MyFifo.h"
#include "string.h"
#include "stdio.h"
#include "rtc.h"

#define AT_DELAY 1000

volatile unsigned long sys_tick = 0;

char data_rec[RECV_BUF_SIZE];
uint8_t sim_csq = 0;
char sim_ip[15];
char sim_imei[16];
unsigned int sim_loc;
unsigned int sim_ci;

uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id);
int eATUART(uint32_t baud);
int eATRST(void);

/******  查询sim模块的固件版本号  ******/
int eATGSV(char * recv_buf);
/******  GPRS模块初始化连接TCP  ******/
int sATIPR(unsigned int baudrate);
int eAT(void);
int qATCPIN(char *CODE);
int eATCSQ(uint8_t *csq);
int eATCSQ_TRANS(uint8_t *csq);
int qATCREG0(uint8_t *n, uint8_t *stat);
int qATCGATT(uint8_t *attach);
int sATCIPMODE(uint8_t mode);
int sATCSTT(char *apn);
int eATCIICR(void);
int eATCIFSR(char * ip_addr);
int sATCIPHEAD(uint8_t mode);
int eATCIPCLOSE(void);
/******  基站定位API需要的数据  ******/
int eATGSN(char * imei);
int sATCREG(uint8_t n);
int qATCREG2(unsigned int *lac, unsigned int *ci);
unsigned int a16toi(char * s);
/******  初始化HTTP，并获取基站定位的数据  ******/
int sATSAPBR1(uint8_t cmd_type, uint8_t cid, char * tag, char * value);
int sATSAPBR2(uint8_t cmd_type, uint8_t cid);
int eATHTTPINIT(void);
int eATHTTPTERM(void);
int sATHTTPPARA(char * tag, char * value);
int sATHTTPACTION(uint8_t method, char * status);
int qATHTTPREAD(char * buf);
int sATCPOWD(uint8_t mode);
/******  sim800c的NTP功能，用来同步RTC时间的  ******/
int sATCNTPCID(uint8_t mode);
int sATCNTP(const char * server_ip, u8 time_zone);
int eATCNTP(void);
int qATCCLK(_calendar_obj * calendar);
/******  文件系统操作  ******/
int sATFSDRIVE(u8 n);
int sATFSLS(char * filepath, char * list_file);
int sATFSCREATE(char * filename);
int sATFSDEL(char * filename);
int sATFSFLSIZE(char * filename, u32 * size);
int sATFSWRITE(char * filename, u8 mode, int size, int timeout, const char *buffer);
int sATFSREAD(char * filename, u8 mode, int size, int position, char * read_buf);
/******  SSL设置  ******/
int qATSSLSETCERT(char * read_buf);
int sATSSLSETCERT(char * file_name, char * pass_word);
int qATCIPSSL(u8 *n);
int sATCIPSSL(u8 n);
/******  透传模式切换到命令行模式  ******/
int eEXIT_TRANS(void);
int eATO(void);
/******  拨号  ******/
int sATD(int number);
int sATS0(int number);

int eATCWSTARTSMART(uint8_t type, char *link_msg);
int eATCWSTOPSMART(void);
int qATCWMODE(uint8_t *mode);
int sATCWMODE(uint8_t mode);
int sATCIPMUX(uint8_t mode);
int sATCWAUTOCONN(uint8_t mode);

int recvFindAndFilter(const char *target, const char *begin, const char *end, char *data_rec, uint32_t timeout);
int recvFind(const char *target, uint32_t timeout);
int recvString(char *rec_data, const char *target, uint32_t timeout);
int recvString2(char *rec_data, const char *target1, const char *target2, uint32_t timeout);
int eATCIPSTATUS(const char * status);
int sATCIPSTARTSingle(const char *type, const char *addr, uint32_t port);
int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len);
int recvString3(char *rec_data, const char *target1, const char *target2, const char *target3, uint32_t timeout);

/*******************************************************************************
  函 数 名 ：AutoLink
  函数功能 ：自动连接，前10s自动连接，若连接失败则进入smartlink模式30s，若依然失败
             则再次回到自动连接，直到连接成功
  输    入 ：无
  输    出 ：无
*******************************************************************************/
void AutoLink(void)
{
  int status = STATUS_LOSTIP;
  while (status != STATUS_GETIP)
  {
    uint32_t start_time = millis();
    printf("start auto link\r\n");
    //10s自动连接时间
    while ((millis() - start_time < 10000) && status != STATUS_GETIP)
    {
      status = getSystemStatus();
      delay(1000);
    }

    //连接失败进入smartlink模式 30s
    if (status != STATUS_GETIP)
    {
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
      }
      else
      {
        stopSmartLink();
        delay(500);
        printf("link AP fail\r\n");
        //restart();
      }
    }
  }
  printf("link AP OK\r\n");
  //sATCWAUTOCONN(0); //开启自动连接模式
}

int GSMInit(const char *addr, uint32_t port, char *http_data)
{	
	static u8 first_flag = 1;
	
	VBAT = 0;
	POWERKEY = 0;
	//open the GSM
	delay(1000);
	//GPIO_SetBits(GPIOB, GPIO_Pin_0);
	POWERKEY = 1;
	delay(1000);
	//GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	POWERKEY = 0;
	delay(1000);
	
	//if(!sATIPR(115200)) goto RESTART; //设置波特率
	if(!ConectTest())	return 0;
#ifdef RF_TEST
	delay(5000);
//	if(!sATD(112)) return 0;
//	printf("sATD(112) is OK\r\n");
	if(!sATS0(1)) return 0;
	printf("sATS0(1) is OK!\r\n");
	while(1);
#endif
	if(!CheckState()) return 0;
	//if(!FSInit()) return 0;
	if(!LSB_API_data()) return 0;
	if(!HttpInit(http_data)) return 0;
	if(first_flag)
	{
		//NTP时间同步的操作每次上电只执行一次。
		if(!SimNTP_get()) return 0;
		first_flag = 0;
	}
	
	if(!TCPInit(addr, port)) return 0;
	ClearRxBuf(); //清除串口中所有缓存的数据
	return 1;
}

int ConectTest(void){
	unsigned char i = 0;
	for(i = 0; i < 40; i++){
		if(!eAT()) delay(AT_DELAY);
		else break;
	}
	if(i >= 40) return 0;
	printf("AT is OK!\r\n");
	if(recvFind("SMS Ready", 5000)) printf("SMS Ready!\r\n"); //等待30S
//	if(!recvFind("SMS Ready", 30000)) return 0; //等待30S
//	printf("SMS Ready!\r\n");
//	delay(AT_DELAY);
	
	return 1;
}

int CheckState(void)
{
	unsigned char i=0;
	char CODE[10];
	uint8_t CREG_n, CREG_stat, attach;
	
	for(i=0; i<30; i++){
		if(!qATCPIN(CODE)) return 0;
		if(strcmp(CODE,"READY") == 0) break;
		else delay(AT_DELAY);
	}
	if(i >= 30) return 0;
	printf("CPIN=READY!\r\n");
	delay(AT_DELAY);
	
	for(i = 0; i < 30; i++)
	{
		if(!eATCSQ(&sim_csq)) return 0;
		if(sim_csq != 0) break;
		else delay(AT_DELAY);
	}
	if(i >= 30) return 0;
	printf("csq = %d\r\n", sim_csq);
	delay(AT_DELAY);
	
	for(i=0; i<30; i++){
		if(!qATCREG0(&CREG_n, &CREG_stat)) return 0;
		if(CREG_stat == 1 || CREG_stat == 5) break;
		else delay(AT_DELAY);
	}
	if(i >= 30) return 0;
	printf("CREG_stat=1!\r\n");
	delay(AT_DELAY);
	
	for(i=0; i<30; i++){
		if(!qATCGATT(&attach)) return 0;
		if(attach == 1) break;
		else delay(AT_DELAY);
	}
	if(i >= 30) return 0;
	printf("CGATT = 1!\r\n");
	
#ifdef TRANS_MODE
	if(!sATCIPMODE(1)) return 0;
	printf("sATCIPMODE = 1 is OK!\r\n");
#endif
	if(!sATCSTT("CMNET")) return 0;
	printf("CSTT = CMNET is OK!\r\n");
	if(!eATCIICR()) return 0;
	printf("CIICR is OK!\r\n");
	if(!eATCIFSR(sim_ip)) return 0;
	printf("sim_ip = %s\r\n", sim_ip);
	return 1;
}

//#define FILESIZE 1326
//const char sslfile_crt [FILESIZE+2] = "-----BEGIN CERTIFICATE-----\nMIIDpzCCAo+gAwIBAgIJAKmRLSNFsnKMMA0GCSqGSIb3DQEBDQUAMGoxFzAVBgNV\nBAMMDkFuIE1RVFQgYnJva2VyMRYwFAYDVQQKDA1Pd25UcmFja3Mub3JnMRQwEgYD\nVQQLDAtnZW5lcmF0ZS1DQTEhMB8GCSqGSIb3DQEJARYSbm9ib2R5QGV4YW1wbGUu\nbmV0MB4XDTE3MDgxODA5MjMzM1oXDTMyMDgxNDA5MjMzM1owajEXMBUGA1UEAwwO\nQW4gTVFUVCBicm9rZXIxFjAUBgNVBAoMDU93blRyYWNrcy5vcmcxFDASBgNVBAsM\nC2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2JvZHlAZXhhbXBsZS5uZXQw\nggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCy3QOdHVmYTP8b9stxERSg\nTENxeYI0aIDS6BBL5QlYLqQDXvr/SMlWieee8kF8A9wmt/beCGJZymNsSj3KxcmI\nRbAtSFxbu9bddHha/uNYJuwiRzrzONFY6BR/zqQ0WlYizXD0sCArcfDCUUCepVv4\nBFWZ5Qru9ZzTI1eSDkFmRm/mTOew9baR7w5ROKolHDTOAvMmpATtLi1CSuhg5fFo\nrauQAvOthkUqSW0tcim4GMr8iBJuGDkmAu/1D0z+uxxw9QUN35deQ8Op+WPTTFNY\nXWjHhUYqdVqNDMlEqyP6azgeyKTuspN9JfHLqq11RHibXDVa3pPS5O/EEpQsjJBd\nAgMBAAGjUDBOMB0GA1UdDgQWBBSHcMjPh75l4mC90uYMm1TxzkptBDAfBgNVHSME\nGDAWgBSHcMjPh75l4mC90uYMm1TxzkptBDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3\nDQEBDQUAA4IBAQBz7gSChZTvxMe6L7/q6FU1wXzAxs4xIYKREkTEpHDViHvSXQ2K\nQ1M04mbENL3Yd9GqV/Rl8Wvm/EfrnlHVDNU0L8NK5fIDWTa80nveyF8D7WR0r/1K\nT1GGczL2fbT/1ACe7yw+zgUvJDkYre2kB0OrZtnS1WTp3NfHFrWYhqlgOoH+8TZI\nNO5pBfHg4AzdNVRpGhBvnSnnlwCKkp8Qtcawfw9g89p8Vm0e7X/9blTAIShkTCpC\nXp9Thl3oYWu7iqMPMFQpWQKCHg/vK+Jy+i374TfDbjI+uTsUdhgPsJHyyn4P7m7I\n7rQsV9hpMmstuhRx9HJHhvoQCOK6CBzYxTcM\n-----END CERTIFICATE-----\n";
#define FILESIZE 2
const char sslfile_crt [FILESIZE+2] = "dd";
#define FSWRITE_TIMEOUT 100
int FSInit(void)
{
	const char * drive = "C:";
	const char * filename = "ca.crt";
	char path_buf[20];
	//char input_buf[20] = "ab";
	char recv_buf[2048];
	u32 filesize;
	
	if(!sATFSDRIVE(0)) return 0;
	printf("sATFSDRIVE(0) is OK\r\n");
	sprintf(path_buf, "%s\\", drive);
	if(!sATFSLS(path_buf, recv_buf)) return 0;
	//if(!sATFSLS("C:\\", recv_buf)) return 0;
	printf("%s\r\n", recv_buf);
	sprintf(path_buf, "%s\\%s", drive, filename);
	if(sATFSREAD(path_buf, 0, FILESIZE, 0, recv_buf))
	{
		printf("read %s is %s\r\n", path_buf, recv_buf);
		if(!strcmp(recv_buf, sslfile_crt))
		{
			printf("recv_buf == sslfile_crt\r\n");
			return 1;
		}
		else
		{
			printf("recv_buf != sslfile_crt\r\n");
		}
	}
	
	if(!sATFSCREATE(path_buf)) return 0;
	printf("creat %s is OK\r\n", path_buf);
	if(!sATFSFLSIZE(path_buf, &filesize)) return 0;
	printf("%s size is %d\r\n", path_buf, filesize);
	printf("strlen(sslfile_crt) = %d\r\n", strlen(sslfile_crt));
	if(strlen(sslfile_crt) != FILESIZE) return 0;
	if(!sATFSWRITE(path_buf, 0, FILESIZE, FSWRITE_TIMEOUT, sslfile_crt)) return 0;
	printf("%s write %s is OK\r\n", path_buf, sslfile_crt);
	if(!sATFSFLSIZE(path_buf, &filesize)) return 0;
	printf("%s size is %d\r\n", path_buf, filesize);
	if(!sATFSREAD(path_buf, 0, FILESIZE, 0, recv_buf)) return 0;
	printf("read %s is %s\r\n", path_buf, recv_buf);
	//sATFSDEL(char * filename);
	
	if(!eATGSV(recv_buf)) return 0;
	printf("eATGSV recv is %s\r\n", recv_buf);
	if(!qATSSLSETCERT(recv_buf)) return 0;
	printf("qATSSLSETCERT recv is %s\r\n", recv_buf);
	return 1;
}

int TCPInit(const char *addr, uint32_t port)
{
#ifdef SSL_MODE
	if(!sATCIPSSL(1)) return 0;
	printf("CIPSSL = 1 OK\r\n");
#endif
	if(!sATCIPHEAD(1)) return 0;
	printf("CIPHEAD = 1 OK\r\n");
	if(!createTCP(addr, port)) return 0;
	printf("create tcp ok! server_ip = %s, port = %d\r\n", addr, port);
#ifndef TRANS_MODE
	if(!eATCIPSTATUS("CONNECT OK")) return 0;
	printf("STATUS: CONNECT OK!\r\n");
#endif
	
	return 1;
}

int LSB_API_data(void)
{
	if(!eATGSN(sim_imei)) return 0;
	printf("sim_imei = %s\r\n", sim_imei);
	if(!sATCREG(2)) return 0;
	printf("AT+CREG = 2 is OK!\r\n");
	if(!qATCREG2(&sim_loc, &sim_ci)) return 0;
	printf("sim_loc = %d, sim_ci = %d\r\n", sim_loc, sim_ci);
	if(!sATCREG(0)) return 0;
	printf("AT+CREG = 0 is OK!\r\n");
		
	return 1;
}

int HttpInit(char * http_data)
{
	char * http_action_status;
	char http_send_data[SEND_BUF_SIZE];
	char http_recv_data[RECV_BUF_SIZE];
	
	/*http init*/
	if(!sATSAPBR1(3,1,"CONTYPE","GPRS")) return 0;
	printf("http 1 OK!\r\n");
	if(!sATSAPBR1(3,1,"APN","CMNET")) return 0;
	printf("http 2 OK!\r\n");
	if(!sATSAPBR2(1,1)) return 0;
	printf("http 3 OK!\r\n");
	if(!sATSAPBR2(2,1)) return 0;
	printf("http 4 OK!\r\n");
	if(!eATHTTPINIT()) return 0;
	printf("http 5 OK!\r\n");
	if(!sATHTTPPARA("CID","1")) return 0;
	printf("http 6 OK!\r\n");
	sprintf(http_send_data, "http://apilocate.amap.com/position?accesstype=0&imei=%s&cdma=0&bts=460,00,%d,%d,%d&serverip=%s&output=josn&key=%s",sim_imei,sim_loc,sim_ci,(sim_csq*2-113),sim_ip,GAODE_API_KEY);
	printf("%s\r\n", http_send_data);
	if(!sATHTTPPARA("URL",http_send_data)) return 0;
	//if(!sATHTTPPARA("URL","http://apilocate.amap.com/position?accesstype=0&imei=862631037454138&cdma=0&bts=460,00,4566,6758,-101&nearbts=460,00,4566,8458,-110|460,00,4566,31248,-107&serverip=10.76.18.147&output=josn&key=9eced1b11c8c7ffd5447eb0ba28748d8")) return 0;
	printf("http 7 OK!\r\n");
	if(!sATHTTPACTION(0, http_action_status)) return 0;
	printf("http 8 OK!\r\n");
	if(!qATHTTPREAD(http_recv_data)) return 0;
	sprintf(http_data,"{%s", http_recv_data);
	printf("%s\r\n", http_data);
	printf("http 9 OK!\r\n");
	if(!eATHTTPTERM()) return 0;
	printf("http 10 OK!\r\n");
	
	return 1;
}

int NtpServer_Try(const char * ntp_server)
{
	if(!sATCNTP(ntp_server, 0)) return 0;
	printf("NTP 2 OK\r\n");
	if(!eATCNTP()) return 0;
	printf("NTP 3 OK\r\n");
	return 1;
}

#define NTPTRY 10
int SimNTP_get(void)
{
	int i;
	const char ntp_server[NTPTRY][20] = {"ntp1.aliyun.com","ntp2.aliyun.com","ntp3.aliyun.com","ntp4.aliyun.com","ntp5.aliyun.com","ntp6.aliyun.com","ntp7.aliyun.com","time.windows.com","time.nist.gov"};
	if(!sATCNTPCID(1)) return 0;
	printf("NTP 1 OK\r\n");
	for(i = 0; i < NTPTRY; i++)
	{
		if(!NtpServer_Try(ntp_server[i]))
		{
			printf("NTP server try %d is error\r\n", i+1);
		}
		else break;
	}
	if(i >= NTPTRY) return 0;
	printf("NTP server try OK\r\n");
	if(!qATCCLK(&calendar_tmp)) return 0;
	printf("NTP 4 OK\r\n");
	RTC_Get();
	printf("current RTC: %d/%d/%d %d:%d:%d\r\n", calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	if(calendar_tmp.w_year!=calendar.w_year || calendar_tmp.w_month!=calendar.w_month || calendar_tmp.w_date!=calendar.w_date || calendar_tmp.hour!=calendar.hour || ((calendar_tmp.min-(calendar.min+1)%60) > 1) || ((calendar.min-(calendar_tmp.min+1)%60) > 1))
	{
		printf("calendar_tmp != calendar, RTC need to update!\r\n");
		RTC_Set(calendar_tmp.w_year,calendar_tmp.w_month,calendar_tmp.w_date,calendar_tmp.hour,calendar_tmp.min,calendar_tmp.sec);
		RTC_Get();
		printf("current RTC: %d/%d/%d %d:%d:%d\r\n", calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	}

	return 1;
}

void ClearRecBuf(void)
{
	//memset(data_rec, '\0', sizeof(data_rec));
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

void GSM_restart(void)
{
	printf("GSM_restart()\r\n");
	POWERKEY = 1;
	delay(2500);
	POWERKEY = 0;
	delay(8000);
	VBAT = 1;
	POWERKEY = 1;
	delay(4500);
}

//切换到AP+station模式
int setOprToStationSoftAP(void)
{
//  uint8_t mode = 0xff;
//  if (!qATCWMODE(&mode))
//  {
//    return 0;
//  }
//  if (mode == 1)
//  {
//    return 1;
//  }
//  else
//  {
//    if (sATCWMODE(1) && restart())
//    {
//      return 1;
//    }
//    else
//    {
//      return 0;
//    }
//  }
	return 1;
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
  if(!eATCIPSTATUS("CONNECT OK")) return 0;
	else return 1;
}

int disableMUX(void)
{
  return sATCIPMUX(0);
}

int createTCP(const char *addr, uint32_t port)
{
  return sATCIPSTARTSingle("TCP", addr, port);
}

void closeTCP(void)
{
	if(eATCIPCLOSE()) printf("close TCP is OK!\r\n");
	else printf("close TCP is FAILED!\r\n");
}

int sim800C_send(const uint8_t *buffer, uint32_t len)
{
	printf("---------- sim800C_send len = %d ----------\r\n", len);
  return sATCIPSENDSingle(buffer, len);
}

int sim800C_recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
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
		printf("########## sim800C_recv len = %d ##########\r\n", len);
	}
  return len;
}

//由于1S内可能接收不到返回数据因此在这里做了循环，封装sim800C_recv()函数。
int mqtt_recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
	int i;
	int len;
	for(i = 0; i < 5; i++)
	{
		if((len = sim800C_recv(buffer, buffer_size, timeout)) != 0) break;
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

int eATUART(uint32_t baud)
{
  int int_baud = baud;
  rx_empty();
  SerialPrint("AT+UART=", STRING_TYPE);
  SerialPrint(&int_baud, INT_TYPE);
  SerialPrintln(",8,1,0,0", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATRST(void)
{
  rx_empty();
  SerialPrintln("AT+RST", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

/******  查询sim模块的固件版本号  ******/
int eATGSV(char * recv_buf)
{
	char buf[SEND_BUF_SIZE];
	int ret;
	
	rx_empty();
	sprintf(buf, "AT+GSV");
	SerialPrintln(buf, STRING_TYPE);
	ret = recvFindAndFilter("\r\nOK", buf, "\r\nOK", recv_buf, TIME_OUT);
	if(ret != 1) return 0;
	return 1;
}

/******  GPRS模块初始化连接TCP  ******/
int sATIPR(unsigned int baudrate)
{
  int int_baudrate = baudrate;
  rx_empty();
  SerialPrint("AT+IPR=", STRING_TYPE);
  SerialPrintln(&int_baudrate, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eAT(void)
{
  rx_empty();
  SerialPrintln("AT", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int qATCPIN(char *CODE)
{
  char str_CODE[10];
  int ret;
  if (!CODE)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+CPIN?", STRING_TYPE);

  ret = recvFindAndFilter("OK", "+CPIN: ", "\r\n\r\nOK", str_CODE, TIME_OUT);
  if (ret != 0)
  {
		strcpy(CODE,str_CODE);
    //CODE = str_CODE;
    return 1;
  }
  return 0;
}

int eATCSQ(uint8_t *csq)
{
  char str_csq[5];
  int ret;
  if (!csq)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+CSQ", STRING_TYPE);

  ret = recvFindAndFilter("OK", "+CSQ: ", ",", str_csq, TIME_OUT);
  if (ret != 0)
  {
    *csq = (uint8_t)atoi(str_csq);
    return 1;
  }
  return 0;
}

int eATCSQ_TRANS(uint8_t *csq)
{
	if(!eEXIT_TRANS()) return 0;
	printf("eEXIT_TRANS is OK!\r\n");
	if(!eATCSQ(csq)) return 0;
	if(!eATO()) return 0;
	printf("eATO is OK!\r\n");
	return 1;
}

int qATCREG0(uint8_t *n, uint8_t *stat)
{

	char str_stat[2];
  int ret_stat;
  if (!n || !stat)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+CREG?", STRING_TYPE);

	ret_stat = recvFindAndFilter("OK", ",", "\r\n\r\nOK", str_stat, TIME_OUT);
  if (ret_stat != 0)
  {
    *n = 0;
		*stat = (uint8_t)atoi(str_stat);
    return 1;
  }
  return 0;
}

int qATCGATT(uint8_t *attach)
{
  char str_attach[5];
  int ret;
  if (!attach)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+CGATT?", STRING_TYPE);

  ret = recvFindAndFilter("OK", "+CGATT: ", "\r\n\r\nOK", str_attach, TIME_OUT);
  if (ret != 0)
  {
    *attach = (uint8_t)atoi(str_attach);
    return 1;
  }
  return 0;
}

int sATCIPMODE(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CIPMODE=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int sATCSTT(char *apn)
{
  //int int_apn = apn;
  rx_empty();
  SerialPrint("AT+CSTT=\"", STRING_TYPE);
  SerialPrint(apn, STRING_TYPE);
	SerialPrintln("\"", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATCIICR(void)
{
  rx_empty();
  SerialPrintln("AT+CIICR", STRING_TYPE);
  return recvFind("OK", 10000);
}

int eATCIFSR(char * ip_addr)
{
	int32_t index1;
	int32_t index2;
  rx_empty();
  SerialPrintln("AT+CIFSR", STRING_TYPE);
	recvString(data_rec, "*", TIME_OUT);
	index1 = StringIndex(data_rec, "AT+CIFSR");
	if (index1 != -1)
	{
		index1 += StringLenth("AT+CIFSR") + 3;	//后面有两个0xd和一个0xa,因此+3
		index2 = strlen(data_rec) - 3;  //因为在末尾有\n\r两个字符存在，因此要减3
		StringSubstring(ip_addr, data_rec, index1, index2);
		return 1;
	}
  return 0;
}

int sATCIPHEAD(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CIPHEAD=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATCIPCLOSE(void)
{
	rx_empty();
  SerialPrintln("AT+CIPCLOSE", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATGSN(char * imei)
{
  int ret;
  if (!imei)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+GSN", STRING_TYPE);

  ret = recvFindAndFilter("OK", "AT+GSN\r\r\n", "\r\n\r\nOK", imei, TIME_OUT);
  if (ret != 1) return 0;
  return 1;
}

int sATCREG(uint8_t n)
{
	char buf[32];
  rx_empty();
	sprintf(buf,"AT+CREG=%d",n);
	SerialPrintln(buf, STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int qATCREG2(unsigned int *lac, unsigned int *ci)
{
	int ret;
  char str_lac_ci[16];
	char str_lac[5];
	char str_ci[5];
	
  if (!lac || !ci)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+CREG?", STRING_TYPE);

  ret = recvFindAndFilter("OK", "+CREG: 2,1,", "\r\n\r\nOK", str_lac_ci, TIME_OUT);
  if (ret != 0)
  {
		int32_t index1 = 1;
		int32_t index2 = StringIndex(str_lac_ci, "\",\"") - 1;
    int32_t index3 = index2 + 4;
		int32_t index4 = strlen(str_lac_ci)-2;
    if (index2 != -1 && index4 != -1)
    {
      StringSubstring(str_lac, str_lac_ci, index1, index2);
			StringSubstring(str_ci, str_lac_ci, index3, index4);
			*lac = a16toi(str_lac);
			*ci = a16toi(str_ci);
			return 1;
		}
  }
  return 0;
}

unsigned int a16toi(char * s)
{
	int i =0;
	unsigned int a=0;
	unsigned int b=0;
	for(i=0;s[i]!='\0';i++)
	{
		if(s[i]>='a'&&s[i]<='f')b=s[i]-'a'+10;
		if(s[i]>='A'&&s[i]<='F')b=s[i]-'A'+10;
		if(s[i]>='0'&&s[i]<='9')b=s[i]-'0';
		a=a*16+b;
	}
	return a;
}

int sATSAPBR1(uint8_t cmd_type, uint8_t cid, char * tag, char * value)
{
	char buf[32];
  rx_empty();
	sprintf(buf,"AT+SAPBR=%d,%d,\"%s\",\"%s\"",cmd_type,cid,tag,value);
	SerialPrintln(buf, STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int sATSAPBR2(uint8_t cmd_type, uint8_t cid)
{
	char buf[32];
  rx_empty();
	sprintf(buf,"AT+SAPBR=%d,%d",cmd_type,cid);
	SerialPrintln(buf, STRING_TYPE);
  return recvFind("OK", 5000);
}

int eATHTTPINIT(void)
{
	rx_empty();
  SerialPrintln("AT+HTTPINIT", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATHTTPTERM(void)
{
	rx_empty();
  SerialPrintln("AT+HTTPTERM", STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int sATHTTPPARA(char * tag, char * value)
{
	char buf[SEND_BUF_SIZE];
  rx_empty();
	sprintf(buf,"AT+HTTPPARA=\"%s\",\"%s\"", tag, value);
	SerialPrintln(buf, STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int sATHTTPACTION(uint8_t method, char * status)
{
	char buf[32];
	//int index1, index2;
	
  rx_empty();
	sprintf(buf,"AT+HTTPACTION=%d",method);
	SerialPrintln(buf, STRING_TYPE);
  //if(!recvFind("OK", TIME_OUT)) return 0;
	if(!recvFind("200", 10000)) return 0;
	return 1;
}

int qATHTTPREAD(char * buf)
{
  int ret;
  if (!buf)
  {
    return 0;
  }
  rx_empty();
  SerialPrintln("AT+HTTPREAD", STRING_TYPE);

  ret = recvFindAndFilter("\r\nOK", "{", "\r\nOK", buf, TIME_OUT);
  if (ret != 1) return 0;
  return 1;
}

int sATCPOWD(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CPOWD=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("NORMAL POWER DOWN", 5000);
}

int sATCNTPCID(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CNTPCID=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int sATCNTP(const char * server_ip, u8 time_zone)
{
	char buf[SEND_BUF_SIZE];
  rx_empty();
	if(time_zone != 0)
	{
		sprintf(buf, "AT+CNTP=\"%s\",%d", server_ip, time_zone);
	}
	else
	{
		sprintf(buf, "AT+CNTP=\"%s\"", server_ip);
	}
	SerialPrintln(buf, STRING_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATCNTP(void)
{
	rx_empty();
  SerialPrintln("AT+CNTP", STRING_TYPE);
  return recvFind("+CNTP: 1", 5000);
}

int qATCCLK(_calendar_obj * calendar_ntp)
{
	char * buf_p;
	char buf[25];
	char year[2];
	char month[2];
	char day[2];
	char hour[2];
	char min[2];
	char sec[2];
  int ret;
  rx_empty();
  SerialPrintln("AT+CCLK?", STRING_TYPE);

  ret = recvFindAndFilter("\r\nOK", "+CCLK: \"", "\"\r\n\r\nOK", buf, TIME_OUT);
	if (ret != 1) return 0;
	
	printf("%s\r\n", buf);
	buf_p = buf;
	strncpy(year, buf_p, 2);
	calendar_ntp->w_year = 2000 + atoi(year);
	buf_p+=3;
	strncpy(month, buf_p, 2);
	calendar_ntp->w_month = atoi(month);
	buf_p+=3;
	strncpy(day, buf_p, 2);
	calendar_ntp->w_date = atoi(day);
	buf_p+=3;
	strncpy(hour, buf_p, 2);
	calendar_ntp->hour = atoi(hour);
	buf_p+=3;
	strncpy(min, buf_p, 2);
	calendar_ntp->min = atoi(min);
	buf_p+=3;
	strncpy(sec, buf_p, 2);
	calendar_ntp->sec = atoi(sec);
	printf("get NTP: %d/%d/%d %d:%d:%d\r\n", calendar_ntp->w_year,calendar_ntp->w_month,calendar_ntp->w_date,calendar_ntp->hour,calendar_ntp->min,calendar_ntp->sec);
	
  return 1;
}


/******  文件系统操作  ******/
int sATFSDRIVE(u8 n)
{
	char buf[SEND_BUF_SIZE];
  rx_empty();
	sprintf(buf, "AT+FSDRIVE=%d", n);
	SerialPrintln(buf, STRING_TYPE);
	if(n == 0) return recvFind("+FSDRIVE: C\r\n\r\nOK", TIME_OUT);
	else return 0;
}

int sATFSLS(char * filepath, char * list_file)
{
	int ret;
	char buf[SEND_BUF_SIZE];
	
	if (filepath == NULL)
	{
		return 0;
	}
	
  rx_empty();
	sprintf(buf, "AT+FSLS=%s", filepath);
	SerialPrintln(buf, STRING_TYPE);
	sprintf(buf, "%s\r\r\n", buf);
	ret = recvFindAndFilter("\r\nOK", buf, "\r\n\r\nOK", list_file, TIME_OUT);
	if(ret != 1) return 0;
	return 1;
}

int sATFSCREATE(char * filename)
{
	char buf[SEND_BUF_SIZE];
	
	if (filename == NULL)
	{
		return 0;
	}
	
  rx_empty();
	sprintf(buf, "AT+FSCREATE=%s", filename);
	SerialPrintln(buf, STRING_TYPE);
	return recvFind("\r\nOK", TIME_OUT);
}

int sATFSDEL(char * filename)
{
	char buf[SEND_BUF_SIZE];
	
	if (filename == NULL)
	{
		return 0;
	}
	
  rx_empty();
	sprintf(buf, "AT+FSDEL=%s", filename);
	SerialPrintln(buf, STRING_TYPE);
	return recvFind("\r\nOK", TIME_OUT);
}

int sATFSFLSIZE(char * filename, u32 * size)
{
	int ret;
	char buf[SEND_BUF_SIZE];
	
	if (filename == NULL)
	{
		return 0;
	}
	
  rx_empty();
	sprintf(buf, "AT+FSFLSIZE=%s", filename);
	SerialPrintln(buf, STRING_TYPE);
	ret = recvFindAndFilter("\r\nOK", "+FSFLSIZE: ", "\r\n\r\nOK", buf, TIME_OUT);	
	if(ret != 1) return 0;
	*size = atoi(buf);
	return 1;
}

int sATFSWRITE(char * filename, u8 mode, int size, int timeout, const char *buffer)
{
	int i;
	char buf[SEND_BUF_SIZE];
	
	rx_empty();
	sprintf(buf, "AT+FSWRITE=%s, %d, %d, %d", filename, mode, size, timeout);
	SerialPrintln(buf, STRING_TYPE);
	if (-1 != recvFind(">", 1000))  //5000
  {
    rx_empty();
    for (i = 0; i < size; i++)
    {
      SerialWrite(buffer[i]);
    }
    return recvFind("OK", 3000);//10000
  }
	return 0;
}

int sATFSREAD(char * filename, u8 mode, int size, int position, char * read_buf)
{
	char buf[SEND_BUF_SIZE];
	int ret;
	
	rx_empty();
	sprintf(buf, "AT+FSREAD=%s, %d, %d, %d", filename, mode, size, position);
	SerialPrintln(buf, STRING_TYPE);
	ret = recvFindAndFilter("\r\nOK", "\r\r\n", "\r\nOK", read_buf, TIME_OUT);	
	if(ret != 1) return 0;
	return 1;
}
/******  文件系统操作  ******/

/******  SSL设置  ******/
int qATSSLSETCERT(char * read_buf)
{
	char buf[SEND_BUF_SIZE];
	int ret;
	
	rx_empty();
	sprintf(buf, "AT+SSLSETCERT=?");
	SerialPrintln(buf, STRING_TYPE);
	ret = recvFindAndFilter("\r\nOK", buf, "\r\nOK", read_buf, TIME_OUT);
	if(ret != 1) return 0;
	return 1;
}

int sATSSLSETCERT(char * file_name, char * pass_word)
{
	char buf[SEND_BUF_SIZE];
	
	rx_empty();
	if(pass_word == NULL)	sprintf(buf, "AT+SSLSETCERT=\"%s\"", file_name);
	else sprintf(buf, "AT+SSLSETCERT=\"%s\", \"%s\"", file_name, pass_word);
	SerialPrintln(buf, STRING_TYPE);
	return recvFind("+SSLSETCERT: 0", TIME_OUT);
}

int qATCIPSSL(u8 *n)
{
	char buf[SEND_BUF_SIZE];
	int ret;
	
	rx_empty();
	sprintf(buf, "AT+CIPSSL?");
	SerialPrintln(buf, STRING_TYPE);
	ret = recvFindAndFilter("\r\nOK", "+CIPSSL: ", "\r\n\r\nOK", buf, TIME_OUT);
	if(ret != 1) return 0;
	*n = atoi(buf);
	return 1;
}

int sATCIPSSL(u8 n)
{
	char buf[SEND_BUF_SIZE];
	
	rx_empty();
	sprintf(buf, "AT+CIPSSL=%d", n);
	SerialPrintln(buf, STRING_TYPE);
	return recvFind("\r\nOK", TIME_OUT);
}
/******  SSL设置  ******/

/******  透传模式切换到命令行模式  ******/
int eEXIT_TRANS(void)
{
	rx_empty();
	SerialPrint("+++", STRING_TYPE);
	return recvFind("OK\r\n", 1000);
}

int eATO(void)
{
	rx_empty();
	SerialPrintln("ATO", STRING_TYPE);
	return recvFind("CONNECT\r\n", TIME_OUT);
}
/******  透传模式切换到命令行模式  ******/

/******  拨号  ******/
int sATD(int number)
{
	char buf[SEND_BUF_SIZE];
	rx_empty();
	sprintf(buf, "ATD%d;", number);
	SerialPrintln(buf, STRING_TYPE);
	return recvFind("OK\r\n", 1000);
}

int sATS0(int number)
{
	char buf[SEND_BUF_SIZE];
	rx_empty();
	sprintf(buf, "ATS0=%d", number);
	SerialPrintln(buf, STRING_TYPE);
	return recvFind("OK\r\n", 1000);
}
/******  拨号  ******/
	

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

  recvString2(data_rec, "OK", "no change", TIME_OUT);
  if (StringIndex(data_rec, "OK") != -1 || StringIndex(data_rec, "no change") != -1)
  {
    return 0;
  }
  return 1;
}

int sATCWAUTOCONN(uint8_t mode)
{
  int int_mode = mode;
  rx_empty();
  SerialPrint("AT+CWAUTOCONN=", STRING_TYPE);
  SerialPrintln(&int_mode, INT_TYPE);
  return recvFind("OK", TIME_OUT);
}

int eATCIPSTATUS(const char * status)
{
  //delay(100);//去掉延时 modfied by LC 2016.01.06 23:46
  rx_empty();
  SerialPrintln("AT+CIPSTATUS", STRING_TYPE);
  return recvFind(status, TIME_OUT);
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
  SerialPrint("\",\"", STRING_TYPE);
  SerialPrint(&int_port, INT_TYPE);
	SerialPrintln("\"", STRING_TYPE);

//	ClearRecBuf();
//  //recvString3(data_rec, "OK", "ERROR", "CONNECT OK", 10000);
//	recvString(data_rec, "CONNECT OK", 10000);
//  if (StringIndex(data_rec, "CONNECT OK") != -1)
//  {
//    return 1;
//  }
#ifdef TRANS_MODE
	if(recvFind("CONNECT", 10000)) return 1;
#else
	if(recvFind("CONNECT OK", 10000)) return 1;
#endif
	//if(recvFind("CONNECT", 10000)) return 1;
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
  int i;
  int int_len = len;
  rx_empty();
  SerialPrint("AT+CIPSEND=", STRING_TYPE);
  SerialPrintln(&int_len, INT_TYPE);
  if (-1 != recvFind(">", 1000))  //5000
  {
    rx_empty();
    for (i = 0; i < len; i++)
    {
      SerialWrite(buffer[i]);
    }
    return recvFind("SEND OK", 3000);//10000
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












