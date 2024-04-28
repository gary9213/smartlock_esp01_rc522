/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�


//�����豸����
#include "./BSP/esp8266/esp8266.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d_uart.h"
#include "./BSP/MQTT/MqttKit.h"

//Ӳ������
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

//C��
#include <string.h>
#include <stdio.h>
#include <stdarg.h>



#define ESP8266_WIFI_INFO		"AT+CWJAP=\"Meizu-B997\",\"\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"broker.emqx.io\",1883\r\n"

 char esp8266_buf[128];
 short esp8266_cnt = 0, esp8266_cntPre = 0;


/**
 * @brief       Usart_SendString�����ַ���
 * @param       ��
 * @retval      ��
 */
void Usart_SendString(USART_TypeDef *USARTx, char *str, unsigned short len)
{
    esp8266_uart_send(USARTx, str, len);

}




/**
 * @brief       ��дPrintf
 * @param       ��
 * @retval      ��
 */
extern UART_HandleTypeDef g_uart1_handle;  /* UART��� */
static uint8_t g_uart1_tx_buf[64]; /* UART1���ͻ��� */
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
 * @brief       ATK-MW8266DӲ����ʼ��
 * @param       ��
 * @retval      ��
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
 * @brief       ATK-MW8266DӲ����λ
 * @param       ��
 * @retval      ��
 */
void atk_mw8266d_hw_reset(void)
{
    ATK_MW8266D_RST(0);
    delay_ms(100);
    ATK_MW8266D_RST(1);
    delay_ms(500);
}

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
 atk_mw8266d_uart_rx_restart();
}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���

		return REV_OK;								//���ؽ�����ɱ�־

	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
    atk_mw8266d_uart_rx_restart();//��ս��ջ���
    ESP8266_Clear();								//��ս��ջ���
	unsigned char timeOut = 255;

	Usart_SendString(USART2, ( char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
//				ESP8266_Clear();									//��ջ���
//                atk_mw8266d_uart_rx_restart();//��ս��ջ���
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(char *data, unsigned short len)
{
	char cmdBuf[64];

    
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
//printf("����CMD%s\r\n",cmdBuf);
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
//printf("����dateB%s\r\n",data);
        delay_ms(500);
		Usart_SendString(USART2, data, len);		//�����豸������������      
    }

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{

			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
//                printf("�ȶ�IPD");
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//��ʱ�ȴ
		timeOut--;
	} while(timeOut>0);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{

    atk_mw8266d_hw_init();                          /* ATK-MW8266DӲ����ʼ�� */
    atk_mw8266d_hw_reset();                         /* ATK-MW8266DӲ����λ */
    atk_mw8266d_uart_init(115200);                /* ATK-MW8266D UART��ʼ�� */


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
    
    UsartPrintf(USART_DEBUG, "�رջ���\r\n");
	while(ESP8266_SendCmd("ATE0\r\n", "OK"))
		delay_ms(500);
    
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

