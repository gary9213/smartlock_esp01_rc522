#ifndef _KEYBORD_H
#define _KEYBORD_H
#include "./SYSTEM/sys/sys.h"



#include "stm32f1xx_hal.h"

#define KEYBORD_GPIO_PORT                  GPIOA
#define KEYBORD_GPIO_PIN                   GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define KEYBORD_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define KEYBORD_EXTI_GPIO_PORT           GPIOA
#define KEYBORD_EXTI_GPIO_PIN             GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define KEYBORD_EXTI_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define Column4_IRQn                             EXTI4_IRQn
#define Column4_IRQHandler                       EXTI4_IRQHandler       

#define Column5_7_IRQn                             EXTI9_5_IRQn
#define Column5_7_IRQHandler                       EXTI9_5_IRQHandler       


void unlock(void);
void lock(void);
extern volatile uint8_t key;
void lock_input_char(void);
void Key_Bord_Init(void);
//非阻塞方式获取按键，返回0，没有键值，非0，获得键值
//中断中调用
uint8_t Get_KeyNum(uint16_t GPIO_Pin);

#define ReadPin_Port  GPIOA

#endif







