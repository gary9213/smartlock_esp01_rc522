/**
 ****************************************************************************************************
 * @file        atk_md0096_iic.c
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

#include "./BSP/ATK_MD0096/atk_md0096_iic.h"
#include "./SYSTEM/delay/delay.h"

/* ����ATK-MD0096ģ��IICͨѶ�ĵ�ַ */
#if (ATK_MD0096_IIC_ADDR_3C != 0)
#define ATK_MD0096_IIC_ADDR 0x3C
#elif (ATK_MD0096_IIC_ADDR_3D != 0)
#define ATK_MD0096_IIC_ADDR 0x3D
#else
#error "One of ATK_MD0096_IIC_ADDR_3C and ATK_MD0096_IIC_ADDR_3D must be defined!"
#endif

/* ����IICͨѶ�Ƕ���������д���� */
#define ATK_MD0096_IIC_WRITE    0
#define ATK_MD0096_IIC_READ     1

/* ����д������������������ */
#define ATK_MD0096_IIC_WRITE_CMD    0x00
#define ATK_MD0096_IIC_WRITE_DAT    0x40

/**
 * @brief       ��ʼ��ATK-MD0096 IIC�ӿ�GPIO
 * @param       Pin  : GPIO����
 *              GPIOx: GPIO�˿�
 * @retval      ��
 */
static inline void atk_md0096_iic_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʹ��GPIOʱ�� */
    ATK_MD0096_IIC_SA0_GPIO_CLK_ENABLE();
    ATK_MD0096_IIC_SCL_GPIO_CLK_ENABLE();
    ATK_MD0096_IIC_SDAIN_GPIO_CLK_ENABLE();
    ATK_MD0096_IIC_SDAOUT_GPIO_CLK_ENABLE();
    
    /* ��ʼ��SA0���� */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SA0_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_NOPULL;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SA0_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��SCL���� */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��SDAIN���� */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SDAIN_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SDAIN_GPIO_PORT, &gpio_init_struct);
    
    /* ��ʼ��SDAOUT���� */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SDAOUT_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_INPUT;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SDAOUT_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       IIC�ӿ���ʱ���������ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static inline void atk_md0096_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
static void atk_md0096_iic_start(void)
{
    ATK_MD0096_IIC_SDAIN(1);
    ATK_MD0096_IIC_SCL(1);
    atk_md0096_iic_delay();
    ATK_MD0096_IIC_SDAIN(0);
    atk_md0096_iic_delay();
    ATK_MD0096_IIC_SCL(0);
    atk_md0096_iic_delay();
}

/**
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
static void atk_md0096_iic_stop(void)
{
    ATK_MD0096_IIC_SDAIN(0);
    atk_md0096_iic_delay();
    ATK_MD0096_IIC_SCL(1);
    atk_md0096_iic_delay();
    ATK_MD0096_IIC_SDAIN(1);
    atk_md0096_iic_delay();
}

/**
 * @brief       �ȴ�IICӦ���ź�
 * @param       ��
 * @retval      0: Ӧ���źŽ��ճɹ�
 *              1: Ӧ���źŽ���ʧ��
 */
static uint8_t atk_md0096_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    ATK_MD0096_IIC_SDAIN(1);
    atk_md0096_iic_delay();
    ATK_MD0096_IIC_SCL(1);
    atk_md0096_iic_delay();
    
    while (ATK_MD0096_IIC_READ_SDAOUT())
    {
        waittime++;
        
        if (waittime > 250)
        {
            atk_md0096_iic_stop();
            rack = 1;
            break;
        }
    }
    
    ATK_MD0096_IIC_SCL(0);
    atk_md0096_iic_delay();
    
    return rack;
}

/**
 * @brief       ʹ��IIC�ӿ���ATK-MD0096д��1�ֽ�����
 * @param       dat : ��д���1�ֽ�����
 * @retval      ��
 */
static void atk_md0096_iic_write_byte(uint8_t dat)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        ATK_MD0096_IIC_SDAIN((dat & 0x80) >> 7);
        atk_md0096_iic_delay();
        ATK_MD0096_IIC_SCL(1);
        atk_md0096_iic_delay();
        ATK_MD0096_IIC_SCL(0);
        dat <<= 1;
    }
    ATK_MD0096_IIC_SDAIN(1);
}

/**
 * @brief       ATK-MD0096 IIC�ӿڳ�ʼ��
 * @param       ��
 * @retval      ��
 */
void atk_md0096_iic_init(void)
{
    atk_md0096_iic_gpio_init();
    
#if (ATK_MD0096_IIC_ADDR_3C != 0)
    ATK_MD0096_IIC_SA0(0);
#elif (ATK_MD0096_IIC_ADDR_3D != 0)
    ATK_MD0096_IIC_SA0(1);
#else
    #error "One of ATK_MD0096_IIC_ADDR_3C and ATK_MD0096_IIC_ADDR_3D must be defined!"
#endif
    
    atk_md0096_iic_stop();
}

/**
 * @brief       ʹ��IIC�ӿ���ATK-MD0096д������
 * @param       cmd: ��д������
 * @retval      ��
 */
void atk_md0096_iic_write_cmd(uint8_t cmd)
{
    atk_md0096_iic_start();
    atk_md0096_iic_write_byte((ATK_MD0096_IIC_ADDR << 1) | ATK_MD0096_IIC_WRITE);
    atk_md0096_iic_wait_ack();
    atk_md0096_iic_write_byte(ATK_MD0096_IIC_WRITE_CMD);
    atk_md0096_iic_wait_ack();
    atk_md0096_iic_write_byte(cmd);
    atk_md0096_iic_wait_ack();
    atk_md0096_iic_stop();
}

/**
 * @brief       ʹ��IIC�ӿ���ATK-MD0096д������
 * @param       dat: ��д������
 * @retval      ��
 */
void atk_md0096_iic_write_data(uint8_t dat)
{
    atk_md0096_iic_start();
    atk_md0096_iic_write_byte((ATK_MD0096_IIC_ADDR << 1) | ATK_MD0096_IIC_WRITE);
    atk_md0096_iic_wait_ack();
    atk_md0096_iic_write_byte(ATK_MD0096_IIC_WRITE_DAT);
    atk_md0096_iic_wait_ack();
    atk_md0096_iic_write_byte(dat);
    atk_md0096_iic_wait_ack();
    atk_md0096_iic_stop();
}
