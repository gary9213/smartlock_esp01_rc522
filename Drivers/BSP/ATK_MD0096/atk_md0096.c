/**
 ****************************************************************************************************
 * @file        atk_md0096.c
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

#include "./BSP/ATK_MD0096/atk_md0096.h"
#include "./BSP/ATK_MD0096/atk_md0096_font.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
#include "./SYSTEM/usart/usart.h"

#if (ATK_MD0096_INTERFACE_8080 != 0)
/* ATK-MD0096模块使用8080接口 */
#include "./BSP/ATK_MD0096/atk_md0096_8080.h"
#define atk_md0096_interface_init   atk_md0096_8080_init
#define atk_md0096_write_cmd        atk_md0096_8080_write_cmd
#define atk_md0096_write_data       atk_md0096_8080_write_data
#endif /* ATK_MD0096_INTERFACE_8080 != 0 */

#if (ATK_MD0096_INTERFACE_SPI != 0)
/* ATK-MD0096模块使用4线SPI接口 */
#include "./BSP/ATK_MD0096/atk_md0096_spi.h"
#define atk_md0096_interface_init   atk_md0096_spi_init
#define atk_md0096_write_cmd        atk_md0096_spi_write_cmd
#define atk_md0096_write_data       atk_md0096_spi_write_data
#endif /* ATK_MD0096_INTERFACE_SPI != 0 */

#if (ATK_MD0096_INTERFACE_IIC != 0)
/* ATK-MD0096模块使用IIC接口 */
#include "./BSP/ATK_MD0096/atk_md0096_iic.h"
#define atk_md0096_interface_init   atk_md0096_iic_init
#define atk_md0096_write_cmd        atk_md0096_iic_write_cmd
#define atk_md0096_write_data       atk_md0096_iic_write_data
#endif /* ATK_MD0096_INTERFACE_IIC != 0 */

/* ATK-MD0096模块OLED显存
 * 显存中的每一比特代表ATK-MD0096模块OLED上的一个像素
 *           col0     col1         col127
 *       ┌---------------------------------┐
 *  row0 | [0][0].0 [0][1].0 ... [0][127].0 |
 *  row1 | [0][0].1 [0][1].1 ... [0][127].1 |
 *  ...  |   ...      ...    ...     ...    |
 *  row7 | [0][0].7 [0][1].7 ... [0][127].7 |
 *  row8 | [1][0].0 [1][1].0 ... [1][127].0 |
 *  row9 | [1][0].1 [1][1].1 ... [1][127].1 |
 *  ...  |   ...      ...    ...     ...    |
 * row15 | [1][0].7 [1][1].7 ... [1][127].7 |
 *  ...  |              ......              |
 * row56 | [7][0].0 [7][1].0 ... [7][127].0 |
 * row57 | [7][0].1 [7][1].1 ... [7][127].1 |
 *  ...  |   ...      ...    ...     ...    |
 * row63 | [7][0].7 [7][1].7 ... [7][127].7 |
 *       └---------------------------------┘
 */
static uint8_t g_atk_md0096_oled_gram[ATK_MD0096_OLED_HEIGHT / 8][ATK_MD0096_OLED_WIDTH] = {0};

/**
 * @brief       平方函数，x^y
 * @param       x: 底数
 *              y: 指数
 * @retval      x^y
 */
static uint32_t atk_md0096_pow(uint8_t x, uint8_t y)
{
    uint8_t loop;
    uint32_t res = 1;
    
    for (loop=0; loop<y; loop++)
    {
        res *= x;
    }
    
    return res;
}

/**
 * @brief       ATK-MD0096模块硬件初始化
 * @param       无
 * @retval      无
 */
static void atk_md0096_hw_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    gpio_init_struct.Pin    = ATK_MD0096_RST_GPIO_PIN;  /* RST引脚 */
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* 推挽输出 */
//    gpio_init_struct.Pull   = GPIO_PULLUP;              /* 上拉 */
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_MEDIUM;   /* 中速 */
//    HAL_GPIO_Init(ATK_MD0096_RST_GPIO_PORT, &gpio_init_struct);
//    ATK_MD0096_VCC_GPIO_CLK_ENABLE();
//    ATK_MD0096_GND_GPIO_CLK_ENABLE();
//    gpio_init_struct.Pin    = ATK_MD0096_VCC_GPIO_PIN;  /* RST引脚 */
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* 推挽输出 */
//    gpio_init_struct.Pull   = GPIO_PULLUP;              /* 上拉 */
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_MEDIUM;   /* 中速 */
//    HAL_GPIO_Init(ATK_MD0096_VCC_GPIO_PORT, &gpio_init_struct);
//    ATK_MD0096_VCC(1);
//    gpio_init_struct.Pin    = ATK_MD0096_GND_GPIO_PIN;  /* RST引脚 */
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* 推挽输出 */
//    gpio_init_struct.Pull   = GPIO_PULLDOWN;              /* 上拉 */
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_MEDIUM;   /* 中速 */
//    HAL_GPIO_Init( ATK_MD0096_GND_GPIO_PORT, &gpio_init_struct);
//    ATK_MD0096_GND(0);
}

/**
 * @brief       ATK-MD0096模块寄存器初始化
 * @param       无
 * @retval      无
 */
static void atk_md0096_reg_init(void)
{
    atk_md0096_write_cmd(0xAE); /* 关闭显示 */
    atk_md0096_write_cmd(0xD5); /* 设置时钟分频因子,震荡频率 */
    atk_md0096_write_cmd(0x80); /* [3:0],分频因子;[7:4],震荡频率 */
    atk_md0096_write_cmd(0xA8); /* 设置驱动路数 */
    atk_md0096_write_cmd(0X3F); /* 默认0X3F(1/64) */
    atk_md0096_write_cmd(0xD3); /* 设置显示偏移 */
    atk_md0096_write_cmd(0X00); /* 默认为0 */
    atk_md0096_write_cmd(0x40); /* 设置显示开始行 [5:0],行数. */
    atk_md0096_write_cmd(0x8D); /* 电荷泵设置 */
    atk_md0096_write_cmd(0x14); /* bit2，开启/关闭 */
    atk_md0096_write_cmd(0x20); /* 设置内存地址模式 */
    atk_md0096_write_cmd(0x02); /* [1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10; */
    atk_md0096_write_cmd(0xA1); /* 段重定义设置,bit0:0,0->0;1,0->127; */
    atk_md0096_write_cmd(0xC8); /* 设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数 */
    atk_md0096_write_cmd(0xDA); /* 设置COM硬件引脚配置 */
    atk_md0096_write_cmd(0x12); /* [5:4]配置 */
    atk_md0096_write_cmd(0x81); /* 对比度设置 */
    atk_md0096_write_cmd(0xCF); /* 1~255;默认0X7F (亮度设置,越大越亮) */
    atk_md0096_write_cmd(0xD9); /* 设置预充电周期 */
    atk_md0096_write_cmd(0xF1); /* [3:0],PHASE 1;[7:4],PHASE 2; */
    atk_md0096_write_cmd(0xDB); /* 设置VCOMH 电压倍率 */
    atk_md0096_write_cmd(0x30); /* [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc; */
    atk_md0096_write_cmd(0xA4); /* 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏) */
    atk_md0096_write_cmd(0xA6); /* 设置显示方式;bit0:1,反相显示;0,正常显示 */
    atk_md0096_write_cmd(0xAF); /* 开启显示 */
}

/**
 * @brief       初始化ATK-MD0096模块
 * @param       无
 * @retval      ATK_MD0096_EOK  : ATK-MD0096模块初始化成功
 *              ATK_MD0096_ERROR: ATK-MD0096模块初始化失败
 */
uint8_t atk_md0096_init(void)
{
    atk_md0096_hw_init();           /* ATK-MD0096模块硬件初始化 */
    atk_md0096_interface_init();    /* ATK-MD0096接口初始化 */
    atk_md0096_hw_reset();          /* ATK-MD0096模块硬件复位 */
    atk_md0096_reg_init();          /* ATK-MD0096模块寄存器初始化 */
    atk_md0096_oled_clear();        /* ATK_MD0096模块OLED清屏 */
    
    return ATK_MD0096_EOK;
}

/**
 * @brief       ATK-MD0096模块硬件复位
 * @param       无
 * @retval      无
 */
void atk_md0096_hw_reset(void)
{
    ATK_MD0096_RST(0);
    delay_ms(100);
    ATK_MD0096_RST(1);
    delay_ms(100);
}

/**
 * @brief       开启ATK-MD0096模块OLED显示
 * @param       无
 * @retval      无
 */
void atk_md0096_oled_display_on(void)
{
    atk_md0096_write_cmd(0x8D); /* 电荷泵设置 */
    atk_md0096_write_cmd(0x14); /* bit2，开启/关闭 */
    atk_md0096_write_cmd(0xAF); /* 开启显示 */
}

/**
 * @brief       关闭ATK-MD0096模块OLED显示
 * @param       无
 * @retval      无
 */
void atk_md0096_oled_display_off(void)
{
    atk_md0096_write_cmd(0x8D); /* 电荷泵设置 */
    atk_md0096_write_cmd(0x10); /* bit2，开启/关闭 */
    atk_md0096_write_cmd(0xAE); /* 关闭显示 */
}

/**
 * @brief       显存更新至ATK-MD0096模块
 * @param       无
 * @retval      无
 */
void atk_md0096_oled_update(void)
{
    uint8_t width_index;
    uint8_t height_index;
    
    for (height_index=0; height_index<(ATK_MD0096_OLED_HEIGHT / 8); height_index++)
    {
        atk_md0096_write_cmd(0xB0 + height_index);
        atk_md0096_write_cmd(0x00);
        atk_md0096_write_cmd(0x10);
        for (width_index=0; width_index<ATK_MD0096_OLED_WIDTH; width_index++)
        {
            atk_md0096_write_data(g_atk_md0096_oled_gram[height_index][width_index]);
        }
    }
}

/**
 * @brief       ATK-MD0096模块OLED清屏
 * @param       无
 * @retval      无
 */
void atk_md0096_oled_clear(void)
{
    uint8_t width_index;
    uint8_t height_index;
    
    for (height_index=0; height_index<(ATK_MD0096_OLED_HEIGHT / 8); height_index++)
    {
        for (width_index=0; width_index<ATK_MD0096_OLED_WIDTH; width_index++)
        {
            g_atk_md0096_oled_gram[height_index][width_index] = 0;
        }
    }
    
    atk_md0096_oled_update();
}

/**
 * @brief       ATK-MD0096模块OLED画点
 * @param       x   : 点X坐标，0~ATK_MD0096_OLED_WIDTH-1
 *              y   : 点Y坐标，0~ATK_MD0096_OLED_HEIGHT-1
 *              mode: ATK_MD0096_OLED_SHOW_MODE_NORMAL:  填充点
 *                    ATK_MD0096_OLED_SHOW_MODE_REVERSE: 清空点
 * @retval      ATK_MD0096_EOK   : 函数执行成功
 *              ATK_MD0096_EINVAL: 函数参数错误
 */
uint8_t atk_md0096_oled_draw_point(uint8_t x, uint8_t y, atk_md0096_oled_show_mode_t mode)
{
    uint8_t page_index;
    uint8_t row_index;
    
    if ((x >= ATK_MD0096_OLED_WIDTH) || (y >= ATK_MD0096_OLED_HEIGHT))
    {
        return ATK_MD0096_EINVAL;
    }
    
    page_index = y >> 3;
    row_index = 1 << (y % 8);
    
    switch (mode)
    {
        case ATK_MD0096_OLED_SHOW_MODE_NORMAL:
        {
            g_atk_md0096_oled_gram[page_index][x] |= row_index;
            break;
        }
        case ATK_MD0096_OLED_SHOW_MODE_REVERSE:
        {
            g_atk_md0096_oled_gram[page_index][x] &= ~row_index;
            break;
        }
        default:
        {
            return ATK_MD0096_EINVAL;
        }
    }
    
    return ATK_MD0096_EOK;
}

uint8_t atk_md0096_oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, atk_md0096_oled_show_mode_t mode)
{
    uint8_t ret;
    uint8_t x_delta;
    uint8_t y_delta;
    int8_t x_sign;
    int8_t y_sign;
    int16_t error;
    int16_t error2;
    
    x_delta = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    y_delta = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    x_sign = (x1 < x2) ? 1 : -1;
    y_sign = (y1 < y2) ? 1 : -1;
    error = x_delta - y_delta;
    
    ret = atk_md0096_oled_draw_point(x2, y2, mode);
    if (ret != ATK_MD0096_EOK)
    {
        return ret;
    }
    
    while ((x1 != x2) || (y1 != y2))
    {
        ret = atk_md0096_oled_draw_point(x1, y1, mode);
        if (ret != ATK_MD0096_EOK)
        {
            return ret;
        }
        
        error2 = error << 1;
        if (error2 > -y_delta)
        {
            error -= y_delta;
            x1 += x_sign;
        }
        
        if (error2 < x_delta)
        {
            error += x_delta;
            y1 += y_sign;
        }
    }
    
    return ATK_MD0096_EOK;
}

/**
 * @brief       ATK-MD0096模块OLED区域填充
 * @param       xs : 区域起始X坐标，0~xe
 *              ys : 区域起始Y坐标，0~ye
 *              xe : 区域终止X坐标，xs~ATK_MD0096_OLED_WIDTH-1
 *              ye : 区域终止Y坐标，ys~ATK_MD0096_OLED_HEIGHT-1
 *              mode: ATK_MD0096_OLED_SHOW_MODE_NORMAL:  填充区域
 *                    ATK_MD0096_OLED_SHOW_MODE_REVERSE: 清空区域
 * @retval      ATK_MD0096_EOK   : 函数执行成功
 *              ATK_MD0096_EINVAL: 函数参数错误
 */
uint8_t atk_md0096_oled_fill(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye, atk_md0096_oled_show_mode_t mode)
{
    uint8_t col_index;
    uint8_t row_index;
    uint8_t ret;
    
    if ((xs > xe) || (ys > ye) || (xe >= ATK_MD0096_OLED_WIDTH) || (ye >= ATK_MD0096_OLED_HEIGHT))
    {
        return ATK_MD0096_EINVAL;
    }
    
    for (col_index=xs; col_index<=xe; col_index++)
    {
        for (row_index=ys; row_index<=ye; row_index++)
        {
            ret = atk_md0096_oled_draw_point(col_index, row_index, mode);
            if (ret != ATK_MD0096_EOK)
            {
                return ret;
            }
        }
    }
    
    return ATK_MD0096_EOK;
}

/**
 * @brief       ATK-MD0096模块OLED显示1个字符
 * @param       x        : 字符X坐标，0~ATK_MD0096_OLED_WIDTH-1
 *              y        : 字符Y坐标，0~ATK_MD0096_OLED_HEIGHT-1
 *              font_size: 字符字体大小 ATK_MD0096_OLED_FONT_SIZE_12: 12号字体
 *                                     ATK_MD0096_OLED_FONT_SIZE_16: 16号字体
 *                                     ATK_MD0096_OLED_FONT_SIZE_24: 24号字体
 *              ch       : 待显示的字符
 *              mode     : ATK_MD0096_OLED_SHOW_MODE_NORMAL:  正常显示
 *                         ATK_MD0096_OLED_SHOW_MODE_REVERSE: 反色显示
 * @retval      ATK_MD0096_EOK   : 函数执行成功
 *              ATK_MD0096_EINVAL: 函数参数错误
 */
uint8_t atk_md0096_oled_show_char(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, const char ch, atk_md0096_oled_show_mode_t mode)
{
    atk_md0096_oled_show_mode_t _mode;
    uint8_t ch_offset;
    const uint8_t *font;
    uint8_t char_width;
    uint8_t char_height;
    uint8_t char_size;
    uint8_t font_dat;
    uint8_t byte_index;
    uint8_t bit_index;
    uint8_t col_offset = 0;
    uint8_t row_offset = 0;
    
    if ((ch < ' ') || (ch > '~'))
    {
        return ATK_MD0096_EINVAL;
    }
    
    switch (mode)
    {
        case ATK_MD0096_OLED_SHOW_MODE_NORMAL:
        {
            _mode = ATK_MD0096_OLED_SHOW_MODE_REVERSE;
            break;
        }
        case ATK_MD0096_OLED_SHOW_MODE_REVERSE:
        {
            _mode = ATK_MD0096_OLED_SHOW_MODE_NORMAL;
            break;
        }
        default:
        {
            return ATK_MD0096_EINVAL;
        }
    }
    
    ch_offset = (uint8_t)(ch - ' ');
    
    switch (font_size)
    {
#if (ATK_MD0096_FONT_12 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_12:
        {
            font = atk_md0096_font_1206[ch_offset];
            char_width = ATK_MD0096_FONT_12_CHAR_WIDTH;
            char_height = ATK_MD0096_FONT_12_CHAR_HEIGHT;
            char_size = ATK_MD0096_FONT_12_CHAR_SIZE;
            break;
        }
#endif
        
#if (ATK_MD0096_FONT_16 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_16:
        {
            font = atk_md0096_font_1608[ch_offset];
            char_width = ATK_MD0096_FONT_16_CHAR_WIDTH;
            char_height = ATK_MD0096_FONT_16_CHAR_HEIGHT;
            char_size = ATK_MD0096_FONT_16_CHAR_SIZE;
            break;
        }
#endif
        
#if (ATK_MD0096_FONT_24 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_24:
        {
            font = atk_md0096_font_2412[ch_offset];
            char_width = ATK_MD0096_FONT_24_CHAR_WIDTH;
            char_height = ATK_MD0096_FONT_24_CHAR_HEIGHT;
            char_size = ATK_MD0096_FONT_24_CHAR_SIZE;
            break;
        }
#endif
        
        default:
        {
            return ATK_MD0096_EINVAL;
        }
    }
    
    if ((x + char_width > ATK_MD0096_OLED_WIDTH) || (y + char_height > ATK_MD0096_OLED_HEIGHT))
    {
        return ATK_MD0096_EINVAL;
    }
    
    for (byte_index=0; byte_index<char_size; byte_index++)
    {
        font_dat = font[byte_index];
        for (bit_index=0; bit_index<8; bit_index++)
        {
            if ((font_dat & 0x80) == 0)
            {
                atk_md0096_oled_draw_point(x + col_offset, y + row_offset, _mode);
            }
            else
            {
                atk_md0096_oled_draw_point(x + col_offset, y + row_offset, mode);
            }
            
            font_dat <<= 1;
            row_offset++;
            if (row_offset == char_height)
            {
                row_offset = 0;
                col_offset++;
                break;
            }
        }
    }
    
    return ATK_MD0096_EOK;
}

/**
 * @brief       ATK-MD0096模块OLED显示字符串
 * @param       x        : 字符串X坐标，0~ATK_MD0096_OLED_WIDTH-1
 *              y        : 字符串Y坐标，0~ATK_MD0096_OLED_HEIGHT-1
 *              font_size: 字符串字体大小 ATK_MD0096_OLED_FONT_SIZE_12: 12号字体
 *                                      ATK_MD0096_OLED_FONT_SIZE_16: 16号字体
 *                                      ATK_MD0096_OLED_FONT_SIZE_24: 24号字体
 *              str      : 待显示的字符串
 *              mode     : ATK_MD0096_OLED_SHOW_MODE_NORMAL:  正常显示
 *                         ATK_MD0096_OLED_SHOW_MODE_REVERSE: 反色显示
 * @retval      ATK_MD0096_EOK   : 函数执行成功
 *              ATK_MD0096_EINVAL: 函数参数错误
 */
uint8_t atk_md0096_oled_show_string(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, const char *str, atk_md0096_oled_show_mode_t mode)
{
    uint8_t ret;
    uint8_t char_width;
    uint8_t char_height;
    
    switch (font_size)
    {
#if (ATK_MD0096_FONT_12 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_12:
        {
            char_width = ATK_MD0096_FONT_12_CHAR_WIDTH;
            char_height = ATK_MD0096_FONT_12_CHAR_HEIGHT;
            break;
        }
#endif
        
#if (ATK_MD0096_FONT_16 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_16:
        {
            char_width = ATK_MD0096_FONT_16_CHAR_WIDTH;
            char_height = ATK_MD0096_FONT_16_CHAR_HEIGHT;
            break;
        }
#endif
        
#if (ATK_MD0096_FONT_24 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_24:
        {
            char_width = ATK_MD0096_FONT_24_CHAR_WIDTH;
            char_height = ATK_MD0096_FONT_24_CHAR_HEIGHT;
            break;
        }
#endif
        
        default:
        {
            return ATK_MD0096_EINVAL;
        }
    }
    
    while ((*str >= ' ') && (*str <= '~'))
    {
        if (x + char_width >= ATK_MD0096_OLED_WIDTH)
        {
            x = 0;
            y += char_height;
        }
        
        if (y + char_height >= ATK_MD0096_OLED_HEIGHT)
        {
            x = 0;
            y = 0;
        }
        
        ret = atk_md0096_oled_show_char(x, y, font_size, *str, mode);
        if (ret != ATK_MD0096_EOK)
        {
            return ret;
        }
        
        x += char_width;
        str++;
    }
    
    return ATK_MD0096_EOK;
}

/**
 * @brief       ATK-MD0096模块OLED显示十进制数字
 * @param       x        : 十进制数字X坐标，0~ATK_MD0096_OLED_WIDTH-1
 *              y        : 十进制数字Y坐标，0~ATK_MD0096_OLED_HEIGHT-1
 *              font_size: 十进制数字字体大小 ATK_MD0096_OLED_FONT_SIZE_12: 12号字体
 *                                          ATK_MD0096_OLED_FONT_SIZE_16: 16号字体
 *                                          ATK_MD0096_OLED_FONT_SIZE_24: 24号字体
 *              num      : 待显示的十进制数字
 *              mode     : ATK_MD0096_OLED_SHOW_MODE_NORMAL:  正常显示
 *                         ATK_MD0096_OLED_SHOW_MODE_REVERSE: 反色显示
 * @retval      ATK_MD0096_EOK   : 函数执行成功
 *              ATK_MD0096_EINVAL: 函数参数错误
 */
uint8_t atk_md0096_oled_show_num(uint8_t x, uint8_t y, atk_md0096_oled_font_size_t font_size, uint32_t num, uint8_t len, atk_md0096_oled_show_mode_t mode)
{
    uint8_t ret;
    uint8_t char_width;
    uint8_t len_index;
    uint8_t num_index;
    uint8_t first_nozero = 0;
    
    switch (font_size)
    {
#if (ATK_MD0096_FONT_12 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_12:
        {
            char_width = ATK_MD0096_FONT_12_CHAR_WIDTH;
            break;
        }
#endif
        
#if (ATK_MD0096_FONT_16 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_16:
        {
            char_width = ATK_MD0096_FONT_16_CHAR_WIDTH;
            break;
        }
#endif
        
#if (ATK_MD0096_FONT_24 != 0)
        case ATK_MD0096_OLED_FONT_SIZE_24:
        {
            char_width = ATK_MD0096_FONT_24_CHAR_WIDTH;
            break;
        }
#endif
        
        default:
        {
            return ATK_MD0096_EINVAL;
        }
    }
    
    for (len_index=0; len_index<len; len_index++)
    {
        num_index = (num / atk_md0096_pow(10, len - len_index - 1)) % 10;
        if ((first_nozero == 0) && (len_index < (len - 1)))
        {
            if (num_index == 0)
            {
                ret = atk_md0096_oled_show_char(x + char_width * len_index, y, font_size, ' ', mode);
                if (ret != ATK_MD0096_EOK)
                {
                    return ret;
                }
            }
            else
            {
                first_nozero = 1;
            }
        }
        
        ret = atk_md0096_oled_show_char(x + char_width * len_index, y, font_size, num_index + '0', mode);
        if (ret != ATK_MD0096_EOK)
        {
            return ret;
        }
    }
    
    return ATK_MD0096_EOK;
}
