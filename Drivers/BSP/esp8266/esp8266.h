#ifndef _ESP8266_H_
#define _ESP8266_H_


#include "./SYSTEM/sys/sys.h"

#define USART_DEBUG                           USART1


/* 引脚定义 */
#define ATK_MW8266D_RST_GPIO_PORT           GPIOB
#define ATK_MW8266D_RST_GPIO_PIN            GPIO_PIN_8
#define ATK_MW8266D_RST_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0) /* PA口时钟使能 */

/* IO操作 */
#define ATK_MW8266D_RST(x)                  do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(ATK_MW8266D_RST_GPIO_PORT, ATK_MW8266D_RST_GPIO_PIN, GPIO_PIN_SET) :  \
                                                HAL_GPIO_WritePin(ATK_MW8266D_RST_GPIO_PORT, ATK_MW8266D_RST_GPIO_PIN, GPIO_PIN_RESET); \
                                            }while(0)
/* 错误代码 */
#define ATK_MW8266D_EOK         0   /* 没有错误 */
#define ATK_MW8266D_ERROR       1   /* 通用错误 */
#define ATK_MW8266D_ETIMEOUT    2   /* 超时错误 */
#define ATK_MW8266D_EINVAL      3   /* 参数错误 */



#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void ESP8266_Init(void);

void ESP8266_Clear(void);


void ESP8266_SendData(char *data, unsigned short len);
unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void Usart_SendString(USART_TypeDef *USARTx, char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx,char *fmt, ...);
#endif
