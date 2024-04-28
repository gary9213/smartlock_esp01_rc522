#ifndef __OLED_H
#define __OLED_H
#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* 引脚 定义 */

#define OLED_W_SCL_GPIO_PORT                  GPIOB
#define OLED_W_SCL_GPIO_PIN                   GPIO_PIN_6
#define OLED_W_SCL_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define OLED_W_SDA_GPIO_PORT                  GPIOB
#define OLED_W_SDA_GPIO_PIN                   GPIO_PIN_7
#define OLED_W_SDA_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */


/******************************************************************************************/
/* OLED端口定义 */
#define OLED_W_SCL(x)                do{ x ? \
                                        HAL_GPIO_WritePin(OLED_W_SCL_GPIO_PORT, OLED_W_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                                        HAL_GPIO_WritePin(OLED_W_SCL_GPIO_PORT, OLED_W_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                                        }while(0)     

#define OLED_W_SDA(x)                do{ x ? \
                                        HAL_GPIO_WritePin(OLED_W_SDA_GPIO_PORT, OLED_W_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                                        HAL_GPIO_WritePin(OLED_W_SDA_GPIO_PORT, OLED_W_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                                        }while(0)     

                  



void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#endif
