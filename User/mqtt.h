/**
  ******************************************************************************
  * @file    mqtt.h
  * $Author: ·ÉºèÌ¤Ñ© $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   MQTTÓ¦ÓÃ²ãº¯Êý.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, EmbedNet</center>
  *<center><a href="http:\\www.embed-net.com">http://www.embed-net.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MQTT_H
#define __MQTT_H
/* Includes ------------------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/

int mqtt_publish(char *pTopic,char *pMessage);
int mqtt_subscrib(char *pTopic,char *pMessage);



#endif /* __MAIN_H */

/*********************************END OF FILE**********************************/
