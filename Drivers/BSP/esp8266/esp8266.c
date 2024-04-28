/**
	************************************************************
	************************************************************
	************************************************************
	*	ÎÄ¼þÃû£º 	esp8266.c
	*
	*	×÷Õß£º 		ÕÅ¼ÌÈð
	*
	*	ÈÕÆÚ£º 		2017-05-08
	*
	*	°æ±¾£º 		V1.0
	*
	*	ËµÃ÷£º 		ESP8266µÄ¼òµ¥Çý¶¯
	*
	*	ÐÞ¸Ä¼ÇÂ¼£º	
	************************************************************
	************************************************************
	************************************************************
**/

//µ¥Æ¬»úÍ·ÎÄ¼þ


//ÍøÂçÉè±¸Çý¶¯
#include "./BSP/esp8266/esp8266.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d_uart.h"
#include "./BSP/MQTT/MqttKit.h"

//Ó²¼þÇý¶¯
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

//C¿â
#include <string.h>
#include <stdio.h>
#include <stdarg.h>



#define ESP8266_WIFI_INFO		"AT+CWJAP=\"Meizu-B997\",\"\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"broker.emqx.io\",1883\r\n"

 char esp8266_buf[128];
 short esp8266_cnt = 0, esp8266_cntPre = 0;


/**
 * @brief       Usart_SendString·¢ËÍ×Ö·û´®
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void Usart_SendString(USART_TypeDef *USARTx, char *str, unsigned short len)
{
    esp8266_uart_send(USARTx, str, len);

}




/**
 * @brief       ÖØÐ´Printf
 * @param       ÎÞ
 * @retval      ÎÞ
 */
extern UART_HandleTypeDef g_uart1_handle;  /* UART¾ä±ú */
static uint8_t g_uart1_tx_buf[64]; /* UART1·¢ËÍ»º³å */
void UsartPrintf(USART_TypeDef *USARTx,char *fmt, ...)
{
    va_list ap;
    uint16_t len;
    
    va_start(ap, fmt);
    vsprintf((char *)g_uart1_tx_buf, fmt, ap);
    va_end(ap);
    
    len = strlen((const char *)g_uart1_tx_buf);
    HAL_UART_Transmit(&g_uart1_handle, g_uart1_tx_buf, len, HAL_MAX_DELAY);
}

/**
 * @brief       ATK-MW8266DÓ²¼þ³õÊ¼»¯
 * @param       ÎÞ
 * @retval      ÎÞ
 */
static void atk_mw8266d_hw_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ATK_MW8266D_RST_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = ATK_MW8266D_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MW8266D_RST_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       ATK-MW8266DÓ²¼þ¸´Î»
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void atk_mw8266d_hw_reset(void)
{
    ATK_MW8266D_RST(0);
    delay_ms(100);
    ATK_MW8266D_RST(1);
    delay_ms(500);
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_Clear
//
//	º¯Êý¹¦ÄÜ£º	Çå¿Õ»º´æ
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
 atk_mw8266d_uart_rx_restart();
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_WaitRecive
//
//	º¯Êý¹¦ÄÜ£º	µÈ´ý½ÓÊÕÍê³É
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	REV_OK-½ÓÊÕÍê³É		REV_WAIT-½ÓÊÕ³¬Ê±Î´Íê³É
//
//	ËµÃ÷£º		Ñ­»·µ÷ÓÃ¼ì²âÊÇ·ñ½ÓÊÕÍê³É
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//Èç¹û½ÓÊÕ¼ÆÊýÎª0 ÔòËµÃ÷Ã»ÓÐ´¦ÓÚ½ÓÊÕÊý¾ÝÖÐ£¬ËùÒÔÖ±½ÓÌø³ö£¬½áÊøº¯Êý
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//Èç¹ûÉÏÒ»´ÎµÄÖµºÍÕâ´ÎÏàÍ¬£¬ÔòËµÃ÷½ÓÊÕÍê±Ï
	{
		esp8266_cnt = 0;							//Çå0½ÓÊÕ¼ÆÊý

		return REV_OK;								//·µ»Ø½ÓÊÕÍê³É±êÖ¾

	}
		
	esp8266_cntPre = esp8266_cnt;					//ÖÃÎªÏàÍ¬
	
	return REV_WAIT;								//·µ»Ø½ÓÊÕÎ´Íê³É±êÖ¾

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_SendCmd
//
//	º¯Êý¹¦ÄÜ£º	·¢ËÍÃüÁî
//
//	Èë¿Ú²ÎÊý£º	cmd£ºÃüÁî
//				res£ºÐèÒª¼ì²éµÄ·µ»ØÖ¸Áî
//
//	·µ»Ø²ÎÊý£º	0-³É¹¦	1-Ê§°Ü
//
//	ËµÃ÷£º		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
    atk_mw8266d_uart_rx_restart();//Çå¿Õ½ÓÊÕ»º´æ
    ESP8266_Clear();								//Çå¿Õ½ÓÊÕ»º´æ
	unsigned char timeOut = 255;

	Usart_SendString(USART2, ( char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//Èç¹ûÊÕµ½Êý¾Ý
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//Èç¹û¼ìË÷µ½¹Ø¼ü´Ê
			{
//				ESP8266_Clear();									//Çå¿Õ»º´æ
//                atk_mw8266d_uart_rx_restart();//Çå¿Õ½ÓÊÕ»º´æ
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_SendData
//
//	º¯Êý¹¦ÄÜ£º	·¢ËÍÊý¾Ý
//
//	Èë¿Ú²ÎÊý£º	data£ºÊý¾Ý
//				len£º³¤¶È
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void ESP8266_SendData(char *data, unsigned short len)
{
	char cmdBuf[64];

    
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//·¢ËÍÃüÁî
//printf("ÕâÊÇCMD%s\r\n",cmdBuf);
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//ÊÕµ½¡®>¡¯Ê±¿ÉÒÔ·¢ËÍÊý¾Ý
	{
//printf("ÕâÊÇdateB%s\r\n",data);
        delay_ms(500);
		Usart_SendString(USART2, data, len);		//·¢ËÍÉè±¸Á¬½ÓÇëÇóÊý¾Ý      
    }

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_GetIPD
//
//	º¯Êý¹¦ÄÜ£º	»ñÈ¡Æ½Ì¨·µ»ØµÄÊý¾Ý
//
//	Èë¿Ú²ÎÊý£º	µÈ´ýµÄÊ±¼ä(³ËÒÔ10ms)
//
//	·µ»Ø²ÎÊý£º	Æ½Ì¨·µ»ØµÄÔ­Ê¼Êý¾Ý
//
//	ËµÃ÷£º		²»Í¬ÍøÂçÉè±¸·µ»ØµÄ¸ñÊ½²»Í¬£¬ÐèÒªÈ¥µ÷ÊÔ
//				ÈçESP8266µÄ·µ»Ø¸ñÊ½Îª	"+IPD,x:yyy"	x´ú±íÊý¾Ý³¤¶È£¬yyyÊÇÊý¾ÝÄÚÈÝ
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//Èç¹û½ÓÊÕÍê³É
		{

			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//ËÑË÷¡°IPD¡±Í·
			if(ptrIPD == NULL)											//Èç¹ûÃ»ÕÒµ½£¬¿ÉÄÜÊÇIPDÍ·µÄÑÓ³Ù£¬»¹ÊÇÐèÒªµÈ´ýÒ»»á£¬µ«²»»á³¬¹ýÉè¶¨µÄÊ±¼ä
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
//                printf("±È¶ÔIPD");
				ptrIPD = strchr(ptrIPD, ':');							//ÕÒµ½':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//ÑÓÊ±µÈ´
		timeOut--;
	} while(timeOut>0);
	
	return NULL;														//³¬Ê±»¹Î´ÕÒµ½£¬·µ»Ø¿ÕÖ¸Õë

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_Init
//
//	º¯Êý¹¦ÄÜ£º	³õÊ¼»¯ESP8266
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void ESP8266_Init(void)
{

    atk_mw8266d_hw_init();                          /* ATK-MW8266DÓ²¼þ³õÊ¼»¯ */
    atk_mw8266d_hw_reset();                         /* ATK-MW8266DÓ²¼þ¸´Î» */
    atk_mw8266d_uart_init(115200);                /* ATK-MW8266D UART³õÊ¼»¯ */


	UsartPrintf(USART_DEBUG, "0. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "1. RST\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "");
	delay_ms(500);
	
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
	delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
    
    UsartPrintf(USART_DEBUG, "¹Ø±Õ»ØÏÔ\r\n");
	while(ESP8266_SendCmd("ATE0\r\n", "OK"))
		delay_ms(500);
    
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

