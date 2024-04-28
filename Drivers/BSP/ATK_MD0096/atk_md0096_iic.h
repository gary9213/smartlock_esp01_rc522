/**
 ****************************************************************************************************
 * @file        atk_md0096_iic.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MD0096模块IIC接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MD0096_IIC_H
#define __ATK_MD0096_IIC_H

#include "./SYSTEM/sys/sys.h"

/* 定义ATK-MD0096模块IIC通讯的地址
 * 通过SA0引脚实现
 */
#define ATK_MD0096_IIC_ADDR_3C  1
#define ATK_MD0096_IIC_ADDR_3D  0

/* 防止IIC通讯地址多重定义 */
#if ((ATK_MD0096_IIC_ADDR_3C != 0) && (ATK_MD0096_IIC_ADDR_3D != 0))
#error "Can't define ATK_MD0096_IIC_ADDR_3C and ATK_MD0096_IIC_ADDR_3D at the same time!"
#endif

/* 默认使用0x3C作为IIC通讯地址 */
#if ((ATK_MD0096_IIC_ADDR_3C == 0) && (ATK_MD0096_IIC_ADDR_3D == 0))
#undef ATK_MD0096_IIC_ADDR_3C
#define ATK_MD0096_IIC_ADDR_3C 1
#endif

/* 引脚定义 */
#define ATK_MD0096_IIC_SA0_GPIO_PORT            GPIOD
#define ATK_MD0096_IIC_SA0_GPIO_PIN             GPIO_PIN_3
#define ATK_MD0096_IIC_SA0_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)

#define ATK_MD0096_IIC_SCL_GPIO_PORT            GPIOB
#define ATK_MD0096_IIC_SCL_GPIO_PIN             GPIO_PIN_6
#define ATK_MD0096_IIC_SCL_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define ATK_MD0096_IIC_SDAIN_GPIO_PORT          GPIOB
#define ATK_MD0096_IIC_SDAIN_GPIO_PIN           GPIO_PIN_7
#define ATK_MD0096_IIC_SDAIN_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOB_CLK_ENABLE();  \
                                                    __HAL_RCC_AFIO_CLK_ENABLE();    \
                                                    __HAL_AFIO_REMAP_SWJ_NOJTAG();  \
                                                    }while(0)

#define ATK_MD0096_IIC_SDAOUT_GPIO_PORT         GPIOC
#define ATK_MD0096_IIC_SDAOUT_GPIO_PIN          GPIO_PIN_4
#define ATK_MD0096_IIC_SDAOUT_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE();    \
                                                    __HAL_RCC_AFIO_CLK_ENABLE();    \
                                                    __HAL_AFIO_REMAP_SWJ_NOJTAG();  \
                                                  }while(0)


/* IO操作 */
#define ATK_MD0096_IIC_SA0(x)                   do{ x ?                                                                                                 \
                                                    HAL_GPIO_WritePin(ATK_MD0096_IIC_SA0_GPIO_PORT, ATK_MD0096_IIC_SA0_GPIO_PIN, GPIO_PIN_SET) :        \
                                                    HAL_GPIO_WritePin(ATK_MD0096_IIC_SA0_GPIO_PORT, ATK_MD0096_IIC_SA0_GPIO_PIN, GPIO_PIN_RESET);       \
                                                }while(0)
#define ATK_MD0096_IIC_SCL(x)                   do{ x ?                                                                                                 \
                                                    HAL_GPIO_WritePin(ATK_MD0096_IIC_SCL_GPIO_PORT, ATK_MD0096_IIC_SCL_GPIO_PIN, GPIO_PIN_SET) :        \
                                                    HAL_GPIO_WritePin(ATK_MD0096_IIC_SCL_GPIO_PORT, ATK_MD0096_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET);       \
                                                }while(0)
#define ATK_MD0096_IIC_SDAIN(x)                 do{ x ?                                                                                                 \
                                                    HAL_GPIO_WritePin(ATK_MD0096_IIC_SDAIN_GPIO_PORT, ATK_MD0096_IIC_SDAIN_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(ATK_MD0096_IIC_SDAIN_GPIO_PORT, ATK_MD0096_IIC_SDAIN_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)
#define ATK_MD0096_IIC_READ_SDAOUT()            HAL_GPIO_ReadPin(ATK_MD0096_IIC_SDAOUT_GPIO_PORT, ATK_MD0096_IIC_SDAOUT_GPIO_PIN)

/* 操作函数 */
void atk_md0096_iic_init(void);                /* ATK-MD0096 IIC接口初始化 */
void atk_md0096_iic_write_cmd(uint8_t cmd);    /* 使用IIC接口往ATK-MD0096写入命令 */
void atk_md0096_iic_write_data(uint8_t dat);   /* 使用IIC接口往ATK-MD0096写入数据 */

#endif
