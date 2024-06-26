/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	esp8266.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		ESP8266的简单驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件


//网络设备驱动
#include "./BSP/esp8266/esp8266.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d_uart.h"
#include "./BSP/MQTT/MqttKit.h"

//硬件驱动
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdarg.h>



#define ESP8266_WIFI_INFO		"AT+CWJAP=\"Meizu-B997\",\"\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"broker.emqx.io\",1883\r\n"

 char esp8266_buf[128];
 short esp8266_cnt = 0, esp8266_cntPre = 0;


/**
 * @brief       Usart_SendString发送字符串
 * @param       无
 * @retval      无
 */
void Usart_SendString(USART_TypeDef *USARTx, char *str, unsigned short len)
{
    esp8266_uart_send(USARTx, str, len);

}




/**
 * @brief       重写Printf
 * @param       无
 * @retval      无
 */
extern UART_HandleTypeDef g_uart1_handle;  /* UART句柄 */
static uint8_t g_uart1_tx_buf[64]; /* UART1发送缓冲 */
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
 * @brief       ATK-MW8266D硬件初始化
 * @param       无
 * @retval      无
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
 * @brief       ATK-MW8266D硬件复位
 * @param       无
 * @retval      无
 */
void atk_mw8266d_hw_reset(void)
{
    ATK_MW8266D_RST(0);
    delay_ms(100);
    ATK_MW8266D_RST(1);
    delay_ms(500);
}

//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
 atk_mw8266d_uart_rx_restart();
}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数

		return REV_OK;								//返回接收完成标志

	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
    atk_mw8266d_uart_rx_restart();//清空接收缓存
    ESP8266_Clear();								//清空接收缓存
	unsigned char timeOut = 255;

	Usart_SendString(USART2, ( char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
//				ESP8266_Clear();									//清空缓存
//                atk_mw8266d_uart_rx_restart();//清空接收缓存
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(char *data, unsigned short len)
{
	char cmdBuf[64];

    
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
//printf("这是CMD%s\r\n",cmdBuf);
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
//printf("这是dateB%s\r\n",data);
        delay_ms(500);
		Usart_SendString(USART2, data, len);		//发送设备连接请求数据      
    }

}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{

			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
//                printf("比对IPD");
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//延时等�
		timeOut--;
	} while(timeOut>0);
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Init(void)
{

    atk_mw8266d_hw_init();                          /* ATK-MW8266D硬件初始化 */
    atk_mw8266d_hw_reset();                         /* ATK-MW8266D硬件复位 */
    atk_mw8266d_uart_init(115200);                /* ATK-MW8266D UART初始化 */


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
    
    UsartPrintf(USART_DEBUG, "关闭回显\r\n");
	while(ESP8266_SendCmd("ATE0\r\n", "OK"))
		delay_ms(500);
    
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

