/**
 ****************************************************************************************************
 * @file        atk_md0096.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MD0096模块驱动代码
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

#ifndef __ATK_MD0096_H
#define __ATK_MD0096_H

#include "./SYSTEM/sys/sys.h"

/* ATK-MD0096模块OLED宽高 */
#define ATK_MD0096_OLED_WIDTH       128
#define ATK_MD0096_OLED_HEIGHT      64

/* 定义ATK-MD0096接口类型 */
#define ATK_MD0096_INTERFACE_8080   0       /* 使用8080接口 */
#define ATK_MD0096_INTERFACE_SPI    0       /* 使用4线SPI接口 */
#define ATK_MD0096_INTERFACE_IIC    1       /* 使用IIC接口 */

/* 定义ATK-MD0096启用的字体 */
#define ATK_MD0096_FONT_12          1       /* 12号字体 */
#define ATK_MD0096_FONT_16          1       /* 16号字体 */
#define ATK_MD0096_FONT_24          1       /* 24号字体 */

/* 防止接口类型多重定义 */
#if (   ((ATK_MD0096_INTERFACE_8080 != 0) || (ATK_MD0096_INTERFACE_SPI != 0)) && \
        ((ATK_MD0096_INTERFACE_8080 != 0) || (ATK_MD0096_INTERFACE_IIC != 0)) && \
        ((ATK_MD0096_INTERFACE_SPI != 0) || (ATK_MD0096_INTERFACE_IIC != 0)))
#error "Interface definition cannot be multiple defined."
#endif

/* 默认接口类型定义为8080 */
#if ((ATK_MD0096_INTERFACE_8080 == 0) && (ATK_MD0096_INTERFACE_SPI == 0) && (ATK_MD0096_INTERFACE_IIC == 0))
#undef ATK_MD0096_INTERFACE_8080
#define ATK_MD0096_INTERFACE_8080 1
#endif

/* 默认启用12号字体 */
#if ((ATK_MD0096_FONT_12 == 0) && (ATK_MD0096_FONT_16 == 0) && (ATK_MD0096_FONT_24 == 0))
#undef ATK_MD0096_FONT_12
#define ATK_MD0096_FONT_12 1
#endif

/* 引脚定义 */
#define ATK_MD0096_RST_GPIO_PORT            GPIOG
#define ATK_MD0096_RST_GPIO_PIN             GPIO_PIN_15
#define ATK_MD0096_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

//#define ATK_MD0096_VCC_GPIO_PORT            GPIOB
//#define ATK_MD0096_VCC_GPIO_PIN             GPIO_PIN_6
//#define ATK_MD0096_VCC_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

//#define ATK_MD0096_GND_GPIO_PORT            GPIOB
//#define ATK_MD0096_GND_GPIO_PIN             GPIO_PIN_7
//#define ATK_MD0096_GND_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/* IO操作 */
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

#define ATK_MD0096_EOK      0               /* 没有错误 */
#define ATK_MD0096_ERROR    1               /* 错误 */
#define ATK_MD0096_EINVAL   2               /* 错误函数参数 */

typedef enum
{
    ATK_MD0096_OLED_SHOW_MODE_NORMAL = 0x00,/* 正常显示 */
    ATK_MD0096_OLED_SHOW_MODE_REVERSE,      /* 反色显示 */
} atk_md0096_oled_show_mode_t;              /* ATK-MD0096 OLED显示模式 */

typedef enum
{
#if (ATK_MD0096_FONT_12 != 0)
    ATK_MD0096_OLED_FONT_SIZE_12,           /* 12号字体 */
#endif
#if (ATK_MD0096_FONT_16 != 0)
    ATK_MD0096_OLED_FONT_SIZE_16,           /* 16号字体 */
#endif
#if (ATK_MD0096_FONT_24 != 0)
    ATK_MD0096_OLED_FONT_SIZE_24,           /* 24号字体 */
#endif
} atk_md0096_oled_font_size_t;              /* ATK-MD0096 OLED显示字体大小*/

/* 操作函数 */
uint8_t atk_md0096_init(void);                                                                                                                              /* 初始化ATK-MD0096模块 */
void atk_md0096_hw_reset(void);                                                                                                                             /* ATK-MD0096模块硬件复位 */
void atk_md0096_oled_display_on(void);                                                                                                                      /* 开启ATK-MD0096模块OLED显示 */
void atk_md0096_oled_display_off(void);                                                                                                                     /* 关闭ATK-MD0096模块OLED显示 */
void atk_md0096_oled_update(void);                                                                                                                          /* 更新显存至ATK-MD0096模块 */
void atk_md0096_oled_clear(void);                                                                                                                           /* ATK-MD0096模块OLED清屏 */
uint8_t atk_md0096_oled_draw_point(uint8_t x, uint8_t y, atk_md0096_oled_show_mode_t mode);                                                                 /* ATK-MD0096模块OLED画点 */
uint8_t atk_md0096_oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, atk_md0096_oled_show_mode_t mode);                                        /* ATK-MD0096模块OLED画线 */
uint8_t atk_md0096_oled_fill(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye, atk_md0096_oled_show_mode_t mode);                                             /* ATK-MD0096模块OLED区域填充 */
uint8_t atk_md0096_oled_show_char(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, const char ch, atk_md0096_oled_show_mode_t mode);            /* ATK-MD0096模块OLED显示1个字符 */
uint8_t atk_md0096_oled_show_string(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, const char *str, atk_md0096_oled_show_mode_t mode);        /* ATK-MD0096模块OLED显示字符串 */
uint8_t atk_md0096_oled_show_num(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, uint32_t num, uint8_t len, atk_md0096_oled_show_mode_t mode); /* ATK-MD0096模块OLED显示十进制数字 */

void OLED_show_mpu6050_data(void);
#endif
