/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266D模块原子云连接实验
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

#include "demo.h"
//#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"


//#define DEMO_WIFI_SSID          "ALIENTEK-YF"
//#define DEMO_WIFI_PWD           "15902020353"
//#define DEMO_ATKCLD_DEV_ID      "37001517764328876459"
//#define DEMO_ATKCLD_DEV_PWD     "12345678"

///**
// * @brief       显示IP地址
// * @param       无
// * @retval      无
// */
//static void demo_show_ip(char *buf)
//{
//    printf("IP: %s\r\n", buf);
//    lcd_show_string(60, 151, 128, 16, 16, buf, BLUE);
//}

///**
// * @brief       按键0功能，功能测试
// * @param       is_atkcld: 0，未连接原子云
// *                         1，已连接原子云
// * @retval      无
// */
//static void demo_key0_fun(uint8_t is_atkcld)
//{
//    uint8_t ret;
//    
//    if (is_atkcld == 0)
//    {
//        /* 进行AT指令测试 */
//        ret = atk_mw8266d_at_test();
//        if (ret == 0)
//        {
//            printf("AT test success!\r\n");
//        }
//        else
//        {
//            printf("AT test failed!\r\n");
//        }
//    }
//    else
//    {
//        /* 发送信息至原子云服务器 */
//        atk_mw8266d_uart_printf("This ATK-MW8266D ALIENTEK Cloud Test.\r\n");
//    }
//}

///**
// * @brief       按键1功能，切换原子云连接状态
// * @param       is_atkcld: 0，未连接原子云
// *                         1，已连接原子云
// * @retval      无
// */
//static void demo_key1_fun(uint8_t *is_atkcld)
//{
//    uint8_t ret;
//    
//    if (*is_atkcld == 0)
//    {
//        /* 连接原子云 */
//        ret = atk_mw8266d_connect_atkcld(DEMO_ATKCLD_DEV_ID, DEMO_ATKCLD_DEV_PWD);
//        if (ret == 0)
//        {
//            *is_atkcld = 1;
//            printf("Connect to ALIENTEK cloud!\r\n");
//        }
//        else
//        {
//            printf("Error to connect ALIENTEK cloud!\r\n");
//        }
//    }
//    else
//    {
//        /* 断开原子云连接 */
//        atk_mw8266d_disconnect_atkcld();
//        *is_atkcld = 0;
//        printf("Disconnect to ALIENTEK cloud!\r\n");
//    }
//}

///**
// * @brief       连接原子云后，将接收自原子云的数据发送到串口调试助手
// * @param       is_atkcld: 0，未连接原子云
// *                         1，已连接原子云
// * @retval      无
// */
//static void demo_upload_data(uint8_t is_atkcld)
//{
//    uint8_t *buf;
//    
//    if (is_atkcld == 1)
//    {
//        /* 接收来自ATK-MW8266D UART的一帧数据 */
//        buf = atk_mw8266d_uart_rx_get_frame();
//        if (buf != NULL)
//        {
//            printf("%s", buf);
//            /* 重开开始接收来自ATK-MW8266D UART的数据 */
//            atk_mw8266d_uart_rx_restart();
//        }
//    }
//}

///**
// * @brief       例程演示入口函数
// * @param       无
// * @retval      无
// */
//void demo_run(void)
//{
//    uint8_t ret;
//    char ip_buf[16];
//    uint8_t key;
//    uint8_t is_atkcld = 0;
//    
//    /* 初始化ATK-MW8266D */
//    ret = atk_mw8266d_init(115200);
//    if (ret != 0)
//    {
//        printf("ATK-MW8266D init failed!\r\n");
//        while (1)
//        {
//            LED0_TOGGLE();
//            delay_ms(200);
//        }
//    }
//    
//    printf("Joining to AP...\r\n");
//    ret  = atk_mw8266d_restore();                               /* 恢复出厂设置 */
//    ret += atk_mw8266d_at_test();                               /* AT测试 */
//    ret += atk_mw8266d_set_mode(1);                             /* Station模式 */
//    ret += atk_mw8266d_sw_reset();                              /* 软件复位 */
//    ret += atk_mw8266d_ate_config(0);                           /* 关闭回显功能 */
//    ret += atk_mw8266d_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);  /* 连接WIFI */
//    ret += atk_mw8266d_get_ip(ip_buf);                          /* 获取IP地址 */
//    if (ret != 0)
//    {
//        printf("Error to join ap!\r\n");
//        while (1)
//        {
//            LED0_TOGGLE();
//            delay_ms(200);
//        }
//    }
//    demo_show_ip(ip_buf);
//    
//    /* 重新开始接收新的一帧数据 */
//    atk_mw8266d_uart_rx_restart();
//    
//    while (1)
//    {
//        key = key_scan(0);
//        
//        switch (key)
//        {
//            case KEY0_PRES:
//            {
//                /* 功能测试 */
//                demo_key0_fun(is_atkcld);
//                break;
//            }
//            case KEY1_PRES:
//            {
//                /* 切换原子云连接状态 */
//                demo_key1_fun(&is_atkcld);
//                break;
//            }
//            default:
//            {
//                break;
//            }
//        }
//        
//        /* 发送接收自原子云的数据到串口调试助手 */
//        demo_upload_data(is_atkcld);
//        
//        delay_ms(10);
//    }
//}
