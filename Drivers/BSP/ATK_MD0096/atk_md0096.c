/**
 ****************************************************************************************************
 * @file        atk_md0096.c
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

#include "./BSP/ATK_MD0096/atk_md0096.h"
#include "./BSP/ATK_MD0096/atk_md0096_font.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
#include "./SYSTEM/usart/usart.h"

#if (ATK_MD0096_INTERFACE_8080 != 0)
/* ATK-MD0096ģ��ʹ��8080�ӿ� */
#include "./BSP/ATK_MD0096/atk_md0096_8080.h"
#define atk_md0096_interface_init   atk_md0096_8080_init
#define atk_md0096_write_cmd        atk_md0096_8080_write_cmd
#define atk_md0096_write_data       atk_md0096_8080_write_data
#endif /* ATK_MD0096_INTERFACE_8080 != 0 */

#if (ATK_MD0096_INTERFACE_SPI != 0)
/* ATK-MD0096ģ��ʹ��4��SPI�ӿ� */
#include "./BSP/ATK_MD0096/atk_md0096_spi.h"
#define atk_md0096_interface_init   atk_md0096_spi_init
#define atk_md0096_write_cmd        atk_md0096_spi_write_cmd
#define atk_md0096_write_data       atk_md0096_spi_write_data
#endif /* ATK_MD0096_INTERFACE_SPI != 0 */

#if (ATK_MD0096_INTERFACE_IIC != 0)
/* ATK-MD0096ģ��ʹ��IIC�ӿ� */
#include "./BSP/ATK_MD0096/atk_md0096_iic.h"
#define atk_md0096_interface_init   atk_md0096_iic_init
#define atk_md0096_write_cmd        atk_md0096_iic_write_cmd
#define atk_md0096_write_data       atk_md0096_iic_write_data
#endif /* ATK_MD0096_INTERFACE_IIC != 0 */

/* ATK-MD0096ģ��OLED�Դ�
 * �Դ��е�ÿһ���ش���ATK-MD0096ģ��OLED�ϵ�һ������
 *           col0     col1         col127
 *       ��---------------------------------��
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
 *       ��---------------------------------��
 */
static uint8_t g_atk_md0096_oled_gram[ATK_MD0096_OLED_HEIGHT / 8][ATK_MD0096_OLED_WIDTH] = {0};

/**
 * @brief       ƽ��������x^y
 * @param       x: ����
 *              y: ָ��
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
 * @brief       ATK-MD0096ģ��Ӳ����ʼ��
 * @param       ��
 * @retval      ��
 */
static void atk_md0096_hw_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    gpio_init_struct.Pin    = ATK_MD0096_RST_GPIO_PIN;  /* RST���� */
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* ������� */
//    gpio_init_struct.Pull   = GPIO_PULLUP;              /* ���� */
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_MEDIUM;   /* ���� */
//    HAL_GPIO_Init(ATK_MD0096_RST_GPIO_PORT, &gpio_init_struct);
//    ATK_MD0096_VCC_GPIO_CLK_ENABLE();
//    ATK_MD0096_GND_GPIO_CLK_ENABLE();
//    gpio_init_struct.Pin    = ATK_MD0096_VCC_GPIO_PIN;  /* RST���� */
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* ������� */
//    gpio_init_struct.Pull   = GPIO_PULLUP;              /* ���� */
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_MEDIUM;   /* ���� */
//    HAL_GPIO_Init(ATK_MD0096_VCC_GPIO_PORT, &gpio_init_struct);
//    ATK_MD0096_VCC(1);
//    gpio_init_struct.Pin    = ATK_MD0096_GND_GPIO_PIN;  /* RST���� */
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* ������� */
//    gpio_init_struct.Pull   = GPIO_PULLDOWN;              /* ���� */
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_MEDIUM;   /* ���� */
//    HAL_GPIO_Init( ATK_MD0096_GND_GPIO_PORT, &gpio_init_struct);
//    ATK_MD0096_GND(0);
}

/**
 * @brief       ATK-MD0096ģ��Ĵ�����ʼ��
 * @param       ��
 * @retval      ��
 */
static void atk_md0096_reg_init(void)
{
    atk_md0096_write_cmd(0xAE); /* �ر���ʾ */
    atk_md0096_write_cmd(0xD5); /* ����ʱ�ӷ�Ƶ����,��Ƶ�� */
    atk_md0096_write_cmd(0x80); /* [3:0],��Ƶ����;[7:4],��Ƶ�� */
    atk_md0096_write_cmd(0xA8); /* ��������·�� */
    atk_md0096_write_cmd(0X3F); /* Ĭ��0X3F(1/64) */
    atk_md0096_write_cmd(0xD3); /* ������ʾƫ�� */
    atk_md0096_write_cmd(0X00); /* Ĭ��Ϊ0 */
    atk_md0096_write_cmd(0x40); /* ������ʾ��ʼ�� [5:0],����. */
    atk_md0096_write_cmd(0x8D); /* ��ɱ����� */
    atk_md0096_write_cmd(0x14); /* bit2������/�ر� */
    atk_md0096_write_cmd(0x20); /* �����ڴ��ַģʽ */
    atk_md0096_write_cmd(0x02); /* [1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10; */
    atk_md0096_write_cmd(0xA1); /* ���ض�������,bit0:0,0->0;1,0->127; */
    atk_md0096_write_cmd(0xC8); /* ����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·�� */
    atk_md0096_write_cmd(0xDA); /* ����COMӲ���������� */
    atk_md0096_write_cmd(0x12); /* [5:4]���� */
    atk_md0096_write_cmd(0x81); /* �Աȶ����� */
    atk_md0096_write_cmd(0xCF); /* 1~255;Ĭ��0X7F (��������,Խ��Խ��) */
    atk_md0096_write_cmd(0xD9); /* ����Ԥ������� */
    atk_md0096_write_cmd(0xF1); /* [3:0],PHASE 1;[7:4],PHASE 2; */
    atk_md0096_write_cmd(0xDB); /* ����VCOMH ��ѹ���� */
    atk_md0096_write_cmd(0x30); /* [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc; */
    atk_md0096_write_cmd(0xA4); /* ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����) */
    atk_md0096_write_cmd(0xA6); /* ������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ */
    atk_md0096_write_cmd(0xAF); /* ������ʾ */
}

/**
 * @brief       ��ʼ��ATK-MD0096ģ��
 * @param       ��
 * @retval      ATK_MD0096_EOK  : ATK-MD0096ģ���ʼ���ɹ�
 *              ATK_MD0096_ERROR: ATK-MD0096ģ���ʼ��ʧ��
 */
uint8_t atk_md0096_init(void)
{
    atk_md0096_hw_init();           /* ATK-MD0096ģ��Ӳ����ʼ�� */
    atk_md0096_interface_init();    /* ATK-MD0096�ӿڳ�ʼ�� */
    atk_md0096_hw_reset();          /* ATK-MD0096ģ��Ӳ����λ */
    atk_md0096_reg_init();          /* ATK-MD0096ģ��Ĵ�����ʼ�� */
    atk_md0096_oled_clear();        /* ATK_MD0096ģ��OLED���� */
    
    return ATK_MD0096_EOK;
}

/**
 * @brief       ATK-MD0096ģ��Ӳ����λ
 * @param       ��
 * @retval      ��
 */
void atk_md0096_hw_reset(void)
{
    ATK_MD0096_RST(0);
    delay_ms(100);
    ATK_MD0096_RST(1);
    delay_ms(100);
}

/**
 * @brief       ����ATK-MD0096ģ��OLED��ʾ
 * @param       ��
 * @retval      ��
 */
void atk_md0096_oled_display_on(void)
{
    atk_md0096_write_cmd(0x8D); /* ��ɱ����� */
    atk_md0096_write_cmd(0x14); /* bit2������/�ر� */
    atk_md0096_write_cmd(0xAF); /* ������ʾ */
}

/**
 * @brief       �ر�ATK-MD0096ģ��OLED��ʾ
 * @param       ��
 * @retval      ��
 */
void atk_md0096_oled_display_off(void)
{
    atk_md0096_write_cmd(0x8D); /* ��ɱ����� */
    atk_md0096_write_cmd(0x10); /* bit2������/�ر� */
    atk_md0096_write_cmd(0xAE); /* �ر���ʾ */
}

/**
 * @brief       �Դ������ATK-MD0096ģ��
 * @param       ��
 * @retval      ��
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
 * @brief       ATK-MD0096ģ��OLED����
 * @param       ��
 * @retval      ��
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
 * @brief       ATK-MD0096ģ��OLED����
 * @param       x   : ��X���꣬0~ATK_MD0096_OLED_WIDTH-1
 *              y   : ��Y���꣬0~ATK_MD0096_OLED_HEIGHT-1
 *              mode: ATK_MD0096_OLED_SHOW_MODE_NORMAL:  ����
 *                    ATK_MD0096_OLED_SHOW_MODE_REVERSE: ��յ�
 * @retval      ATK_MD0096_EOK   : ����ִ�гɹ�
 *              ATK_MD0096_EINVAL: ������������
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
 * @brief       ATK-MD0096ģ��OLED�������
 * @param       xs : ������ʼX���꣬0~xe
 *              ys : ������ʼY���꣬0~ye
 *              xe : ������ֹX���꣬xs~ATK_MD0096_OLED_WIDTH-1
 *              ye : ������ֹY���꣬ys~ATK_MD0096_OLED_HEIGHT-1
 *              mode: ATK_MD0096_OLED_SHOW_MODE_NORMAL:  �������
 *                    ATK_MD0096_OLED_SHOW_MODE_REVERSE: �������
 * @retval      ATK_MD0096_EOK   : ����ִ�гɹ�
 *              ATK_MD0096_EINVAL: ������������
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
 * @brief       ATK-MD0096ģ��OLED��ʾ1���ַ�
 * @param       x        : �ַ�X���꣬0~ATK_MD0096_OLED_WIDTH-1
 *              y        : �ַ�Y���꣬0~ATK_MD0096_OLED_HEIGHT-1
 *              font_size: �ַ������С ATK_MD0096_OLED_FONT_SIZE_12: 12������
 *                                     ATK_MD0096_OLED_FONT_SIZE_16: 16������
 *                                     ATK_MD0096_OLED_FONT_SIZE_24: 24������
 *              ch       : ����ʾ���ַ�
 *              mode     : ATK_MD0096_OLED_SHOW_MODE_NORMAL:  ������ʾ
 *                         ATK_MD0096_OLED_SHOW_MODE_REVERSE: ��ɫ��ʾ
 * @retval      ATK_MD0096_EOK   : ����ִ�гɹ�
 *              ATK_MD0096_EINVAL: ������������
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
 * @brief       ATK-MD0096ģ��OLED��ʾ�ַ���
 * @param       x        : �ַ���X���꣬0~ATK_MD0096_OLED_WIDTH-1
 *              y        : �ַ���Y���꣬0~ATK_MD0096_OLED_HEIGHT-1
 *              font_size: �ַ��������С ATK_MD0096_OLED_FONT_SIZE_12: 12������
 *                                      ATK_MD0096_OLED_FONT_SIZE_16: 16������
 *                                      ATK_MD0096_OLED_FONT_SIZE_24: 24������
 *              str      : ����ʾ���ַ���
 *              mode     : ATK_MD0096_OLED_SHOW_MODE_NORMAL:  ������ʾ
 *                         ATK_MD0096_OLED_SHOW_MODE_REVERSE: ��ɫ��ʾ
 * @retval      ATK_MD0096_EOK   : ����ִ�гɹ�
 *              ATK_MD0096_EINVAL: ������������
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
 * @brief       ATK-MD0096ģ��OLED��ʾʮ��������
 * @param       x        : ʮ��������X���꣬0~ATK_MD0096_OLED_WIDTH-1
 *              y        : ʮ��������Y���꣬0~ATK_MD0096_OLED_HEIGHT-1
 *              font_size: ʮ�������������С ATK_MD0096_OLED_FONT_SIZE_12: 12������
 *                                          ATK_MD0096_OLED_FONT_SIZE_16: 16������
 *                                          ATK_MD0096_OLED_FONT_SIZE_24: 24������
 *              num      : ����ʾ��ʮ��������
 *              mode     : ATK_MD0096_OLED_SHOW_MODE_NORMAL:  ������ʾ
 *                         ATK_MD0096_OLED_SHOW_MODE_REVERSE: ��ɫ��ʾ
 * @retval      ATK_MD0096_EOK   : ����ִ�гɹ�
 *              ATK_MD0096_EINVAL: ������������
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
