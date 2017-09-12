#include "stm32f10x.h"
#include "aqi.h"


//extern volatile int  Conce_PM2_5;       // PM2.5浓度
//extern volatile int  Conce_PM10;        // PM10浓度

int IAQI[8]={0,50,100,150,200,300,400,500};
int PM10[8]={0,50,150,250,350,420,500,600};
int PM25[8]={0,35,75,115,150,250,350,500};

int CpPM10;		//PM10浓度
int CpPM25;		//PM2.5浓度
int BpHi;			//与Cp相近的污染物浓度限值的高值
int BpLo;			//与Cp相近的污染物浓度限值的低值
int IAQIHi;		//与BpHi对应的空气质量分指数
int IAQILo;		//与BpLo对应的空气质量分指数

void AQI_Count(int pm_2_5, int pm_10, int * iaq_2_5, int * iaq_10, int * iaq_max)				//计算AQI指数，判断首要污染物
{
		u8 i=0;
		u8 j=0;

		CpPM25=pm_2_5;
				
		for(j=0;j<7;j++)
		{
				if((CpPM25>PM25[j])&&(CpPM25<PM25[j+1]))
				{
						BpHi=PM25[j+1];
						BpLo=PM25[j];
						IAQIHi=IAQI[j+1];
						IAQILo=IAQI[j];
						*iaq_2_5=(IAQIHi-IAQILo)*(CpPM25-BpLo)/(BpHi-BpLo)+IAQILo;
				}
				else if(CpPM25==PM25[j])
				{
						*iaq_2_5=IAQI[j];
				}
				else if(CpPM25>=500)
				{
						*iaq_2_5=CpPM25;	 //爆表
				}
		}
	
		CpPM10=pm_10;	
		
		for(i=0;i<7;i++)
		{
				if((CpPM10>PM10[i])&&(CpPM10<PM10[i+1]))
				{
						BpHi=PM10[i+1];
						BpLo=PM10[i];
						IAQIHi=IAQI[i+1];
						IAQILo=IAQI[i];
						*iaq_10=(IAQIHi-IAQILo)*(CpPM10-BpLo)/(BpHi-BpLo)+IAQILo;
				}
				else if(CpPM10==PM10[i])
				{
						*iaq_10=IAQI[i];
				}
				else if(CpPM10>=600)
				{
						*iaq_10=((50/6)*(CpPM10-600))/10+500;	 //爆表
				}
		}

		*iaq_max = (*iaq_2_5 > *iaq_10) ? *iaq_2_5 : *iaq_10;
}

