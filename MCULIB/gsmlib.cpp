#include "gsmlib.h"
#include "stdio.h"
#include "string.h"

GSM_outCom GSM_send;
GSM_voidFunction GSM_delay100ms;
//GSM_printFunction GSM_dprint;
GSM_Do GSM_SetMotorLevel;
char commandStr[50]; //max length is 49
char receivedStr[50]; //with \n as the tail


int receivedCount;
#define GSM_NoConnect 0
#define GSM_Transparent 1
static int state = GSM_NoConnect;

void delay1s() {
    int i = 10;
    while (i--) GSM_delay100ms();
}

void connect() {
		int length;
    //GSM_dprint("setting as transparent TCP");
    sprintf(commandStr, "AT+CIPMODE=1\r\n");
    GSM_send(commandStr, (int)strlen(commandStr));
    delay1s();
    //GSM_dprint("connecting!");
    sprintf(commandStr, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",
            serverIP, serverPort);
    length = (int)strlen(commandStr);
    GSM_send(commandStr, length);
		delay1s();
}

void GSM_Disconnect() {
    state = GSM_NoConnect;    //first quit the transparent mode
    delay1s();delay1s();
    GSM_send("+++", 3);
    delay1s();delay1s();
    sprintf(commandStr, "\nAT+CIPSHUT\r\n");
    GSM_send(commandStr, (int)strlen(commandStr));
    delay1s();
}

void GSM_Initial(void) {
    //GSM_dprint("initializing!!");
    connect();
    //GSM_dprint("initial finished");
    //state = GSM_Transparent;
}

void GSM_input(char c) {
	  //char recstr[] = "recv: ";
		//recstr[5] = c;
		//GSM_dprint(recstr);
    if (state == GSM_NoConnect) {
        if (c == '\n') {
            //one command end
            receivedStr[receivedCount] = 0; //"\r"
            receivedCount = 0;
            //GSM_dprint(receivedStr);
            if (strcmp(receivedStr, "CONNECT\r") == 0) {
							
							
							
							GPIO_ResetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
							delay1s();delay1s();delay1s();
							GPIO_SetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
							
							  //  GSM_dprint("got a CONNECT into transparent mode");
                state = GSM_Transparent;
							
				  	
//							while()
//							{ u8 t=0;
//								
//							if(t%10==0) { USART_SendData(USART1,Tx_Data) ;}//发送PM2.5数据到串口1
//								
//								delay1s();t++;
//								if(t==20){t=0;}								
//							
//							}							
            
            }
        }
        else {
            receivedStr[receivedCount] = c;
            ++receivedCount;
            receivedCount %= 50;
        }
    } else {
        receivedCount = 0;
        receivedStr[0] = 0;
    }
    if (state == GSM_Transparent && c <= '9' && c >= '0') {
			
			if(c=='0')
				{	GPIO_SetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
					GSM_SetMotorLevel(c - '0');	}
			else if(c=='4')
				   { 	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
							GPIO_SetBits(GPIOB,GPIO_Pin_13);
							GPIO_SetBits(GPIOB,GPIO_Pin_14);
							GSM_SetMotorLevel(c - '0');	}
			else if(c=='6')
				   { 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
							GPIO_ResetBits(GPIOB,GPIO_Pin_13);
							GPIO_SetBits(GPIOB,GPIO_Pin_14);
							GSM_SetMotorLevel(c - '0');	}
		  else if(c=='3')
				   { 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
							GPIO_SetBits(GPIOB,GPIO_Pin_13);
							GPIO_ResetBits(GPIOB,GPIO_Pin_14);
							GSM_SetMotorLevel(c - '0');	}
        
    }
}

void GSM_SendString(const char* s) {
    int length = (int)strlen(s);
    //GSM_dprint("Send String!");
    while (s[length]) ++length;

}
