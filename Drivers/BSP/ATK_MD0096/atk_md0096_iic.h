/**
 ****************************************************************************************************
 * @file        atk_md0096_iic.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MD0096ģ��IIC�ӿ���������
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

#ifndef __ATK_MD0096_IIC_H
#define __ATK_MD0096_IIC_H

#include "./SYSTEM/sys/sys.h"

/* ����ATK-MD0096ģ��IICͨѶ�ĵ�ַ
 * ͨ��SA0����ʵ��
 */
#define ATK_MD0096_IIC_ADDR_3C  1
#define ATK_MD0096_IIC_ADDR_3D  0

/* ��ֹIICͨѶ��ַ���ض��� */
#if ((ATK_MD0096_IIC_ADDR_3C != 0) && (ATK_MD0096_IIC_ADDR_3D != 0))
#error "Can't define ATK_MD0096_IIC_ADDR_3C and ATK_MD0096_IIC_ADDR_3D at the same time!"
#endif

/* Ĭ��ʹ��0x3C��ΪIICͨѶ��ַ */
#if ((ATK_MD0096_IIC_ADDR_3C == 0) && (ATK_MD0096_IIC_ADDR_3D == 0))
#undef ATK_MD0096_IIC_ADDR_3C
#define ATK_MD0096_IIC_ADDR_3C 1
#endif

/* ���Ŷ��� */
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


/* IO���� */
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

/* �������� */
void atk_md0096_iic_init(void);                /* ATK-MD0096 IIC�ӿڳ�ʼ�� */
void atk_md0096_iic_write_cmd(uint8_t cmd);    /* ʹ��IIC�ӿ���ATK-MD0096д������ */
void atk_md0096_iic_write_data(uint8_t dat);   /* ʹ��IIC�ӿ���ATK-MD0096д������ */

#endif
