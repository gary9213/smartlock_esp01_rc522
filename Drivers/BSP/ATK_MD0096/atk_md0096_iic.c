/**
 ****************************************************************************************************
 * @file        atk_md0096_iic.c
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

#include "./BSP/ATK_MD0096/atk_md0096_iic.h"
#include "./SYSTEM/delay/delay.h"

/* 定义ATK-MD0096模块IIC通讯的地址 */
#if (ATK_MD0096_IIC_ADDR_3C != 0)
#define ATK_MD0096_IIC_ADDR 0x3C
#elif (ATK_MD0096_IIC_ADDR_3D != 0)
#define ATK_MD0096_IIC_ADDR 0x3D
#else
#error "One of ATK_MD0096_IIC_ADDR_3C and ATK_MD0096_IIC_ADDR_3D must be defined!"
#endif

/* 控制IIC通讯是读操作还是写操作 */
#define ATK_MD0096_IIC_WRITE    0
#define ATK_MD0096_IIC_READ     1

/* 控制写入的数据是命令或数据 */
#define ATK_MD0096_IIC_WRITE_CMD    0x00
#define ATK_MD0096_IIC_WRITE_DAT    0x40

/**
 * @brief       初始化ATK-MD0096 IIC接口GPIO
 * @param       Pin  : GPIO引脚
 *              GPIOx: GPIO端口
 * @retval      无
 */
static inline void atk_md0096_iic_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* 使能GPIO时钟 */
    ATK_MD0096_IIC_SA0_GPIO_CLK_ENABLE();
    ATK_MD0096_IIC_SCL_GPIO_CLK_ENABLE();
    ATK_MD0096_IIC_SDAIN_GPIO_CLK_ENABLE();
    ATK_MD0096_IIC_SDAOUT_GPIO_CLK_ENABLE();
    
    /* 初始化SA0引脚 */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SA0_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_NOPULL;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SA0_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化SCL引脚 */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化SDAIN引脚 */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SDAIN_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SDAIN_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化SDAOUT引脚 */
    gpio_init_struct.Pin    = ATK_MD0096_IIC_SDAOUT_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_INPUT;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0096_IIC_SDAOUT_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       IIC接口延时函数，用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static inline void atk_md0096_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
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
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
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
 * @brief       等待IIC应答信号
 * @param       无
 * @retval      0: 应答信号接收成功
 *              1: 应答信号接收失败
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
 * @brief       使用IIC接口往ATK-MD0096写入1字节数据
 * @param       dat : 待写入的1字节数据
 * @retval      无
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
 * @brief       ATK-MD0096 IIC接口初始化
 * @param       无
 * @retval      无
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
 * @brief       使用IIC接口往ATK-MD0096写入命令
 * @param       cmd: 待写入命令
 * @retval      无
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
 * @brief       使用IIC接口往ATK-MD0096写入数据
 * @param       dat: 待写入数据
 * @retval      无
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
