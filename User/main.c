/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ��ԭ��������ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/KEYBORD/keybord.h"
#include "./BSP/RC522/rc522.h"
#include "demo.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d_uart.h"
#include "./BSP/esp8266/esp8266.h"
#include "./BSP/ONENET/onenet.h"
#include "./BSP/OLED/oled.h"
#include <string.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"

int main(void)
{    
	const char *topics[] = {"smartlock/client"};
        	unsigned char *dataPtr = NULL;
        extern   char esp8266_buf[128];
extern   short esp8266_cnt ; 
char PUB_BUF[256];
uint8_t Temp=35;
uint8_t Humidity=24;
float Light=661.2;
uint8_t lock ;
extern uint32_t tick;
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
    delay_init(72);                     /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��LED */


    ESP8266_Init();
    while(OneNet_DevLink()) 
    delay_ms(500);
    printf("���");
    	OneNet_Subscribe(topics, 1);
    ESP8266_Clear();  

    Rc522_Init();

        Key_Bord_Init();

        OLED_Init();
        OLED_ShowString(1, 6, "LOCK");
        OLED_ShowString(2, 1, "input password:");

    while(1)
    {
        tick+=1;
        if(tick>0xFFFFFFFF)
        {
            tick=0;
        }

        Rc522_Test();
        
        lock_input_char();
        
        dataPtr = ESP8266_GetIPD(5);
        if(dataPtr != NULL)
        {
            printf("eeeee\r\n");
            OneNet_RevPro(dataPtr);   
            lock =HAL_GPIO_ReadPin(LED0_GPIO_PORT,LED0_GPIO_PIN);
            sprintf(PUB_BUF,"{\"Temp\":%d,\"Humidity\":%d,\"Light\":%1f,\"lock\":%d}",
            Temp,Humidity,Light,lock);
            OneNet_Publish("smartlock/server", PUB_BUF);
            ESP8266_Clear();
            dataPtr=NULL;
            //yanshi
            
            
        }
    }
}
