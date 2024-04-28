#ifndef __MYSPI_H
#define __MYSPI_H
#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 引脚 定义 */

#define MySPI_W_SS_GPIO_PORT                  GPIOB
#define MySPI_W_SS_GPIO_PIN                   GPIO_PIN_12
#define MySPI_W_SS_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define MySPI_W_SCK_GPIO_PORT                  GPIOB
#define MySPI_W_SCK_GPIO_PIN                   GPIO_PIN_13
#define MySPI_W_SCK_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define MySPI_W_MOSI_GPIO_PORT                  GPIOB
#define MySPI_W_MOSI_GPIO_PIN                   GPIO_PIN_14
#define MySPI_W_MOSI_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define MySPI_R_MISO_GPIO_PORT                  GPIOB
#define MySPI_R_MISO_GPIO_PIN                   GPIO_PIN_15
#define MySPI_R_MISO_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */


/******************************************************************************************/
/* LED端口定义 */
#define MySPI_W_SS(x)   do{ x ? \
                      HAL_GPIO_WritePin(MySPI_W_SS_GPIO_PORT, MySPI_W_SS_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(MySPI_W_SS_GPIO_PORT, MySPI_W_SS_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      /* LED0翻转 */

#define MySPI_W_SCK(x)   do{ x ? \
                      HAL_GPIO_WritePin(MySPI_W_SCK_GPIO_PORT, MySPI_W_SCK_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(MySPI_W_SCK_GPIO_PORT, MySPI_W_SCK_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      /* LED0翻转 */

#define MySPI_W_MOSI(x)   do{ x ? \
                      HAL_GPIO_WritePin(MySPI_W_MOSI_GPIO_PORT, MySPI_W_MOSI_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(MySPI_W_MOSI_GPIO_PORT, MySPI_W_MOSI_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      /* LED0翻转 */



void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
