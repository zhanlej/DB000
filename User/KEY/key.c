#include "key.h"
#include "delay.h"
#include "uart.h"
#include "string.h"
#include "main.h"
//#include "MyFifo.h"
#include "stdio.h"
#include "aqi.h"
//#include "DSHCHO.h"
#include "24tft.h"

//unsigned char wait_send_press;
//int press_len;
//char press_buf[PRESS_SIZE][2];
//u32 press_time_log[PRESS_SIZE];
////u32 press_HCHO[PRESS_SIZE];
//u32 press_C1[PRESS_SIZE];
//u32 press_C2[PRESS_SIZE];
//u32 press_AQI[PRESS_SIZE];

KEY_FLAG_T key_flag;	//按键相关标示结构体
BUTTON_T s_Powerkey;	//power按键的结构体
BUTTON_T s_Modekey;		//mode按键的结构体

//是否有POWER按键按下接口函数
unsigned char  IsKeyDownPower(void)
{
  if (0 == KEY_POWER) return 1;
  return 0;
}

//是否有MODE按键按下接口函数
unsigned char  IsKeyDownMode(void)
{
  if (0 == KEY_MODE) return 1;
  return 0;
}

void  PanakeyHard_Init(void)
{
  //GPIO_Init (POWER_KEY_PORT, POWER_KEY_PIN, GPIO_MODE_IN_FL_NO_IT);//power key
  //GPIO的初始化在main函数中操作
}

void  PanakeyVar_Init(void)
{
	key_flag.ChildLock_flag = 0;	//童锁标志，0为童锁无效，1为童锁有效
	key_flag.Comb_flag = 0;				//组合按键标示
	
  /* 初始化POWER按键变量，支持按下、弹起、长按 */
	s_Powerkey.key_type = KEY_TPYE_POWER;
  s_Powerkey.IsKeyDownFunc = IsKeyDownPower;                /* 判断按键按下的函数 */
  s_Powerkey.FilterTime = BUTTON_FILTER_TIME;                /* 按键滤波时间 */
  s_Powerkey.LongTime = BUTTON_LONG_TIME;                        /* 长按时间 */
  s_Powerkey.Count = s_Powerkey.FilterTime / 2;                /* 计数器设置为滤波时间的一半 */
  s_Powerkey.State = 0;                                                        /* 按键缺省状态，0为未按下 */
  s_Powerkey.KeyCodeDown = KEY_DOWN_Power;                        /* 按键按下的键值代码 */
  s_Powerkey.KeyCodeUp = KEY_UP_Power;                               /* 按键弹起的键值代码 */
  s_Powerkey.KeyCodeLong = KEY_LONG_Power;                        /* 按键被持续按下的键值代码 */
  s_Powerkey.RepeatSpeed = 0;                                                /* 按键连发的速度，0表示不支持连发 */
  s_Powerkey.RepeatCount = 0;                                                /* 连发计数器 */
	s_Powerkey.IsLong = 0;
	s_Powerkey.IsComb = 0;
	
	/* 初始化MODE按键变量，支持按下、弹起、长按 */
	s_Modekey.key_type = KEY_TPYE_MODE;
  s_Modekey.IsKeyDownFunc = IsKeyDownMode;                /* 判断按键按下的函数 */
  s_Modekey.FilterTime = BUTTON_FILTER_TIME;                /* 按键滤波时间 */
  s_Modekey.LongTime = BUTTON_LONG_TIME;                        /* 长按时间 */
  s_Modekey.Count = s_Powerkey.FilterTime / 2;                /* 计数器设置为滤波时间的一半 */
  s_Modekey.State = 0;                                                        /* 按键缺省状态，0为未按下 */
  s_Modekey.KeyCodeDown = KEY_DOWN_Power;                        /* 按键按下的键值代码 */
  s_Modekey.KeyCodeUp = KEY_UP_Power;                               /* 按键弹起的键值代码 */
  s_Modekey.KeyCodeLong = KEY_LONG_Power;                        /* 按键被持续按下的键值代码 */
  s_Modekey.RepeatSpeed = 0;                                                /* 按键连发的速度，0表示不支持连发 */
  s_Modekey.RepeatCount = 0;                                                /* 连发计数器 */
	s_Modekey.IsLong = 0;
	s_Modekey.IsComb = 0;
}

void Panakey_Init(void)
{
  PanakeyHard_Init();                /* 初始化按键变量 */
  PanakeyVar_Init();                /* 初始化按键硬件 */
}

//void SavePressLog(void)
//{
//	int tmp_press_time = 0;
//	
//	//筛选在断网期间连续多次物理按键的情况，如果两次物理按键间隔不超过1S则最后一次的物理按键覆盖前一次的物理按键
//	tmp_press_time = RTC_GetCounter();
//	if(press_len > 0)
//	{
//		if((tmp_press_time - press_time_log[press_len-1]) <= 1)
//		{
//			press_len--;
//		}
//	}
//	
//	//if (Fifo_canPush(&recv_fifo1)) Fifo_Push(&recv_fifo1, *mqtt_mode);
//	//将按下按键后的状态和按下按键的时间记录在下面的数组中
//	if(All_State == sendPM) press_len = 0;
//	else if(press_len >= PRESS_SIZE) press_len = PRESS_SIZE-1;	//当数组满的时候新的数据只替换末尾的一个数据
//	strcpy(press_buf[press_len], mqtt_mode);
//	press_time_log[press_len] = RTC_GetCounter();
//	//press_HCHO[press_len] = Conce_HCHO;
//	press_C1[press_len] = Conce_PM2_5;
//	press_C2[press_len] = Conce_PM10;			
//	press_AQI[press_len] = AQI_Max;			
//	press_len++;
//	wait_send_press = 1;
//}

void Pannelkey_Put(KEY_TPYE_ENUM key_type, unsigned char KeyCode)
{
	if(key_type == KEY_TPYE_POWER)
	{
		// 定义一个队列 放入按键值
		if(KeyCode == KEY_DOWN_Power)
		{
			printf("POWER press!\r\n");
		}
		else if(KeyCode == KEY_UP_Power)
		{
			printf("POWER key up\r\n");
			if(OLED_screenlight_get())
			{
				if(*mqtt_mode == '0')
					PowerOnOff(1);
				else
					PowerOnOff(0);
//				SavePressLog();
			}
			else	//如果是在息屏状态下，按下按键先跳转到主菜单
			{
				OLED_uitype_change(UI_MAIN);
			}
		}
	}
	else if(key_type == KEY_TPYE_MODE)
	{
		// 定义一个队列 放入按键值
		if(KeyCode == KEY_DOWN_Power)
		{
			printf("MODE press!\r\n");
		}
		else if(KeyCode == KEY_UP_Power)
		{
			printf("MODE key up\r\n");
			if(OLED_screenlight_get())
			{
				switch(*mqtt_mode)
				{
					case 'A':
						strcpy(mqtt_mode,"1");
						ModeCountrol();
						break;
					case '1':
						strcpy(mqtt_mode,"2");
						ModeCountrol();
						break;
					case '2':
						strcpy(mqtt_mode,"3");
						ModeCountrol();
						break;
					case '3':
						strcpy(mqtt_mode,"4");
						ModeCountrol();
						break;
					case '4':
						strcpy(mqtt_mode,"A");
						ModeCountrol();
						break;
				}
//				ModeCountrol();
//				SavePressLog();
			}
			else	//如果是在息屏状态下，按下按键先跳转到主菜单
			{
				OLED_uitype_change(UI_MAIN);
			}
		}
	}
	
	//只有两个按键都长按才能触发组合键
	if(KeyCode == KEY_LONG_Power && s_Powerkey.IsLong == 1 && s_Modekey.IsLong == 1)
	{
		//设置组合键的flag
		if(OLED_screenlight_get())
		{
			s_Powerkey.IsComb = 1;
			s_Modekey.IsComb = 1;
			printf("Combine key - wifi restore!\r\n");
			key_flag.Comb_flag = 1;
		}
		else	//如果是在息屏状态下，按下按键先跳转到主菜单
		{
			OLED_uitype_change(UI_MAIN);
		}
	}
}

/*
*********************************************************************************************************
*        函 数 名: bsp_DetectButton
*        功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*        形    参：按键结构变量指针
*        返 回 值: 无
*********************************************************************************************************
*/
void Button_Detect(BUTTON_T *_pBtn)
{
  if (_pBtn->IsKeyDownFunc())
  {
    if (_pBtn->Count < _pBtn->FilterTime)
    {
      _pBtn->Count = _pBtn->FilterTime;
    }
    else if(_pBtn->Count < 2 * _pBtn->FilterTime)
    {
      _pBtn->Count++;
    }
    else
    {
      if (_pBtn->State == 0)
      {
        _pBtn->State = 1;

        /* 发送按钮按下的消息 */
        if (_pBtn->KeyCodeDown > 0)
        {
          /* 键值放入按键FIFO */
          Pannelkey_Put(_pBtn->key_type, _pBtn->KeyCodeDown);// 记录按键按下标志，等待释放

        }
      }

      if (_pBtn->LongTime > 0)
      {
        if (_pBtn->LongCount < _pBtn->LongTime)
        {
          /* 发送按钮持续按下的消息 */
          if (++_pBtn->LongCount == _pBtn->LongTime)
          {
            /* 键值放入按键FIFO */
						_pBtn->IsLong = 1;
            Pannelkey_Put(_pBtn->key_type, _pBtn->KeyCodeLong);
          }
        }
        else
        {
          if (_pBtn->RepeatSpeed > 0)
          {
            if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)
            {
              _pBtn->RepeatCount = 0;
              /* 常按键后，每隔10ms发送1个按键 */
              Pannelkey_Put(_pBtn->key_type, _pBtn->KeyCodeDown);

            }
          }
        }
      }
    }
  }
  else
  {
    if(_pBtn->Count > _pBtn->FilterTime)
    {
      _pBtn->Count = _pBtn->FilterTime;
    }
    else if(_pBtn->Count != 0)
    {
      _pBtn->Count--;
    }
    else
    {
      if (_pBtn->State == 1)
      {
        _pBtn->State = 0;

        /* 发送按钮弹起的消息 */
        if (_pBtn->KeyCodeUp > 0) /*按键释放*/
        {
          /* 键值放入按键FIFO */
					if(_pBtn->IsComb != 1) Pannelkey_Put(_pBtn->key_type, _pBtn->KeyCodeUp);
					_pBtn->IsComb = 0;
					_pBtn->IsLong = 0;
        }
				_pBtn->LongCount = 0;
				_pBtn->RepeatCount = 0;
      }
    }
  }
}


//功能说明: 检测所有按键。10MS 调用一次
void Pannelkey_Polling(void)
{
  Button_Detect(&s_Powerkey);                /* POWER 键 */
	Button_Detect(&s_Modekey);                /* MODE 键 */
}

