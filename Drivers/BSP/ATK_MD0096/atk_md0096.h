/**
 ****************************************************************************************************
 * @file        atk_md0096.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MD0096ģ����������
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

#ifndef __ATK_MD0096_H
#define __ATK_MD0096_H

#include "./SYSTEM/sys/sys.h"

/* ATK-MD0096ģ��OLED��� */
#define ATK_MD0096_OLED_WIDTH       128
#define ATK_MD0096_OLED_HEIGHT      64

/* ����ATK-MD0096�ӿ����� */
#define ATK_MD0096_INTERFACE_8080   0       /* ʹ��8080�ӿ� */
#define ATK_MD0096_INTERFACE_SPI    0       /* ʹ��4��SPI�ӿ� */
#define ATK_MD0096_INTERFACE_IIC    1       /* ʹ��IIC�ӿ� */

/* ����ATK-MD0096���õ����� */
#define ATK_MD0096_FONT_12          1       /* 12������ */
#define ATK_MD0096_FONT_16          1       /* 16������ */
#define ATK_MD0096_FONT_24          1       /* 24������ */

/* ��ֹ�ӿ����Ͷ��ض��� */
#if (   ((ATK_MD0096_INTERFACE_8080 != 0) || (ATK_MD0096_INTERFACE_SPI != 0)) && \
        ((ATK_MD0096_INTERFACE_8080 != 0) || (ATK_MD0096_INTERFACE_IIC != 0)) && \
        ((ATK_MD0096_INTERFACE_SPI != 0) || (ATK_MD0096_INTERFACE_IIC != 0)))
#error "Interface definition cannot be multiple defined."
#endif

/* Ĭ�Ͻӿ����Ͷ���Ϊ8080 */
#if ((ATK_MD0096_INTERFACE_8080 == 0) && (ATK_MD0096_INTERFACE_SPI == 0) && (ATK_MD0096_INTERFACE_IIC == 0))
#undef ATK_MD0096_INTERFACE_8080
#define ATK_MD0096_INTERFACE_8080 1
#endif

/* Ĭ������12������ */
#if ((ATK_MD0096_FONT_12 == 0) && (ATK_MD0096_FONT_16 == 0) && (ATK_MD0096_FONT_24 == 0))
#undef ATK_MD0096_FONT_12
#define ATK_MD0096_FONT_12 1
#endif

/* ���Ŷ��� */
#define ATK_MD0096_RST_GPIO_PORT            GPIOG
#define ATK_MD0096_RST_GPIO_PIN             GPIO_PIN_15
#define ATK_MD0096_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

//#define ATK_MD0096_VCC_GPIO_PORT            GPIOB
//#define ATK_MD0096_VCC_GPIO_PIN             GPIO_PIN_6
//#define ATK_MD0096_VCC_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

//#define ATK_MD0096_GND_GPIO_PORT            GPIOB
//#define ATK_MD0096_GND_GPIO_PIN             GPIO_PIN_7
//#define ATK_MD0096_GND_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/* IO���� */
#define ATK_MD0096_RST(x)                   do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(ATK_MD0096_RST_GPIO_PORT, ATK_MD0096_RST_GPIO_PIN, GPIO_PIN_SET) :    \
                                                HAL_GPIO_WritePin(ATK_MD0096_RST_GPIO_PORT, ATK_MD0096_RST_GPIO_PIN, GPIO_PIN_RESET);   \
                                            }while(0)

                                          
#define ATK_MD0096_VCC(x)                   do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(ATK_MD0096_VCC_GPIO_PORT, ATK_MD0096_VCC_GPIO_PIN, GPIO_PIN_SET) :    \
                                                HAL_GPIO_WritePin(ATK_MD0096_VCC_GPIO_PORT, ATK_MD0096_VCC_GPIO_PIN, GPIO_PIN_RESET);   \
                                            }while(0)

#define ATK_MD0096_GND(x)                   do{ x ?                                                                                     \
                                            HAL_GPIO_WritePin(ATK_MD0096_GND_GPIO_PORT, ATK_MD0096_GND_GPIO_PIN, GPIO_PIN_SET) :    \
                                            HAL_GPIO_WritePin(ATK_MD0096_GND_GPIO_PORT, ATK_MD0096_GND_GPIO_PIN, GPIO_PIN_RESET);   \
}while(0)

#define ATK_MD0096_EOK      0               /* û�д��� */
#define ATK_MD0096_ERROR    1               /* ���� */
#define ATK_MD0096_EINVAL   2               /* ���������� */

typedef enum
{
    ATK_MD0096_OLED_SHOW_MODE_NORMAL = 0x00,/* ������ʾ */
    ATK_MD0096_OLED_SHOW_MODE_REVERSE,      /* ��ɫ��ʾ */
} atk_md0096_oled_show_mode_t;              /* ATK-MD0096 OLED��ʾģʽ */

typedef enum
{
#if (ATK_MD0096_FONT_12 != 0)
    ATK_MD0096_OLED_FONT_SIZE_12,           /* 12������ */
#endif
#if (ATK_MD0096_FONT_16 != 0)
    ATK_MD0096_OLED_FONT_SIZE_16,           /* 16������ */
#endif
#if (ATK_MD0096_FONT_24 != 0)
    ATK_MD0096_OLED_FONT_SIZE_24,           /* 24������ */
#endif
} atk_md0096_oled_font_size_t;              /* ATK-MD0096 OLED��ʾ�����С*/

/* �������� */
uint8_t atk_md0096_init(void);                                                                                                                              /* ��ʼ��ATK-MD0096ģ�� */
void atk_md0096_hw_reset(void);                                                                                                                             /* ATK-MD0096ģ��Ӳ����λ */
void atk_md0096_oled_display_on(void);                                                                                                                      /* ����ATK-MD0096ģ��OLED��ʾ */
void atk_md0096_oled_display_off(void);                                                                                                                     /* �ر�ATK-MD0096ģ��OLED��ʾ */
void atk_md0096_oled_update(void);                                                                                                                          /* �����Դ���ATK-MD0096ģ�� */
void atk_md0096_oled_clear(void);                                                                                                                           /* ATK-MD0096ģ��OLED���� */
uint8_t atk_md0096_oled_draw_point(uint8_t x, uint8_t y, atk_md0096_oled_show_mode_t mode);                                                                 /* ATK-MD0096ģ��OLED���� */
uint8_t atk_md0096_oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, atk_md0096_oled_show_mode_t mode);                                        /* ATK-MD0096ģ��OLED���� */
uint8_t atk_md0096_oled_fill(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye, atk_md0096_oled_show_mode_t mode);                                             /* ATK-MD0096ģ��OLED������� */
uint8_t atk_md0096_oled_show_char(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, const char ch, atk_md0096_oled_show_mode_t mode);            /* ATK-MD0096ģ��OLED��ʾ1���ַ� */
uint8_t atk_md0096_oled_show_string(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, const char *str, atk_md0096_oled_show_mode_t mode);        /* ATK-MD0096ģ��OLED��ʾ�ַ��� */
uint8_t atk_md0096_oled_show_num(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, uint32_t num, uint8_t len, atk_md0096_oled_show_mode_t mode); /* ATK-MD0096ģ��OLED��ʾʮ�������� */

void OLED_show_mpu6050_data(void);
#endif
