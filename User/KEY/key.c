#include "key.h"
#include "delay.h"
#include "uart1.h"
#include "string.h"
#include "main.h"
//#include "MyFifo.h"
#include "stdio.h"
#include "aqi.h"

unsigned char wait_send_press;
int press_len;
char press_buf[PRESS_SIZE][2];
u32 press_time_log[PRESS_SIZE];
u16 press_C1[PRESS_SIZE];
u16 press_C2[PRESS_SIZE];
u16 press_AQI[PRESS_SIZE];

BUTTON_T s_Powerkey;
//是否有按键按下接口函数
unsigned char  IsKeyDownUser(void)
{
  if (0 == KEY0) return 1;
  return 0;
}

void  PanakeyHard_Init(void)
{
  //GPIO_Init (POWER_KEY_PORT, POWER_KEY_PIN, GPIO_MODE_IN_FL_NO_IT);//power key
  //GPIO的初始化在main函数中操作
}

void  PanakeyVar_Init(void)
{
  /* 初始化USER按键变量，支持按下、弹起、长按 */
  s_Powerkey.IsKeyDownFunc = IsKeyDownUser;                /* 判断按键按下的函数 */
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
	s_Powerkey.timeout_flag = 0;
	s_Powerkey.ChildLock_flag = 0;
}

void Panakey_Init(void)
{
  PanakeyHard_Init();                /* 初始化按键变量 */
  PanakeyVar_Init();                /* 初始化按键硬件 */
}

void SavePressLog(void)
{
	//if (Fifo_canPush(&recv_fifo1)) Fifo_Push(&recv_fifo1, *mqtt_mode);
	//将按下按键后的状态和按下按键的时间记录在下面的数组中
	if(All_State == sendPM) press_len = 0;
	else if(press_len >= PRESS_SIZE) press_len = PRESS_SIZE-1;	//当数组满的时候新的数据只替换末尾的一个数据
	strcpy(press_buf[press_len], mqtt_mode);
	press_time_log[press_len] = RTC_GetCounter();
	press_C1[press_len] = Conce_PM2_5;
	press_C2[press_len] = Conce_PM10;			
	press_AQI[press_len] = AQI_Max;			
	press_len++;
	wait_send_press = 1;
}

void Pannelkey_Put(unsigned char KeyCode)
{
  // 定义一个队列 放入按键值
	if(KeyCode == KEY_DOWN_Power)
  {
    DBG("press!");
  }
  else if(KeyCode == KEY_UP_Power)
  {
		DBG("short press");
		switch(*mqtt_mode)
		{
			case '0':
				strcpy(mqtt_mode,"A");
				break;
			case 'A':
				strcpy(mqtt_mode,"1");
				break;
			case '1':
				strcpy(mqtt_mode,"2");
				break;
			case '2':
				strcpy(mqtt_mode,"3");
				break;
			case '3':
				strcpy(mqtt_mode,"4");
				break;
			case '4':
				strcpy(mqtt_mode,"A");
				break;
		}
		ModeCountrol();
		SavePressLog();
  }
  else if(KeyCode == KEY_LONG_Power)
  {
    DBG("LONG PRESS");
		strcpy(mqtt_mode, "0");
		ModeCountrol();
		SavePressLog();
  }
	else
	{
		DBG("KeyCode is error!");
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
          Pannelkey_Put(_pBtn->KeyCodeDown);// 记录按键按下标志，等待释放

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
            Pannelkey_Put(_pBtn->KeyCodeLong);
						_pBtn->IsLong = 1;
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
              Pannelkey_Put(_pBtn->KeyCodeDown);

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
					if(_pBtn->IsLong != 1) Pannelkey_Put(_pBtn->KeyCodeUp);
					_pBtn->IsLong = 0;
        }
      }
    }

    _pBtn->LongCount = 0;
    _pBtn->RepeatCount = 0;
  }
}
//功能说明: 检测所有按键。10MS 调用一次
void Pannelkey_Polling(void)
{
  Button_Detect(&s_Powerkey);                /* USER 键 */
}

