#include "./BSP/KEYBORD/keybord.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/STMFLASH/stmflash.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
#include <stdio.h>
#define  KEY_DELAY  10  //�ж�ȥ�������

volatile uint8_t  key;
volatile uint32_t  key_tic;
volatile char c;
volatile uint32_t   tick;
volatile uint32_t flag_time_lock;
 char  smart_lockpassword [6]={""};
char smart_lock_input_password [6];

volatile uint8_t  Flag_Password_determination=0; 
uint8_t function_key;

static uint8_t flag_leng_unlock;
char temp_smart_lock_input [6];
static uint8_t temp_cent_one;
uint8_t Flag_password_true;
extern uint32_t tick_lock;

#define FLASH_SAVE_ADDR 0X0801FC04 /* ����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С + 0X08000000) */
uint8_t datatemp[6];




/**
 * @brief     
 * @param    
 * @retval    ��
 */
uint8_t duibishuzu(char* shuzu1,char* shuzu2,uint8_t str_len)
{
        for(uint8_t j=0;j< str_len;j++)
        {
            if (shuzu2[j] != shuzu1[j] )
            {
                return 1;
            }

        }
        return 0;
}


/**
 * @brief     ����
 * @param    
 * @retval    ��
 */
void lock(void)
{
    OLED_Clear();
    OLED_ShowString(1, 6, "LOCK");
    OLED_ShowString(2, 1, "input password:");

    printf("������\r\n");
    LED0(1);
    Flag_password_true=0;

}


/**
 * @brief     ����
 * @param    
 * @retval    ��
 */
void unlock(void)
{
    flag_time_lock =tick;
    OLED_Clear();
    OLED_ShowString(1, 1, "unlock success");

    printf("�����ɹ�\r\n");
    LED0(0);
    Flag_password_true=1;

    //delay_ms(1000);
    //lock();
}




/**
 * @brief     ��������1��2
 * @param    
 * @retval    ��
 */
uint8_t copy_array1_to2(char* array1,char* array2)
{    

        for(uint8_t j=0;j< 6;j++)
        {
               array2 [j] =array1[j];

        }
        return 0;
}




/**
 * @brief     �޸�����
 * @param    
 * @retval    ��
 */
uint8_t Flag_Change_your_password_true;
char password_temporary_1 [10];
char password_temporary_2 [10];
uint8_t cent_password_temporary;
uint8_t Flag_changeerror=0;
uint8_t Flag_queren;
uint8_t Flag_jinrucishu=1;
void Lock_Change_your_password()
{
    OLED_Clear();
    OLED_ShowString(1, 1, "change password");
    OLED_ShowString(2, 1, "old password:");
    printf("����ԭ����\r\n");
    while(Flag_Password_determination ==3 )  //�����ɹ����ҵ����3��ȷ��
    {   
                  //����ԭ����

        if(flag_leng_unlock==1) //����6λ�������
        {
            flag_leng_unlock=0;
            uint8_t flag_yuanmima=duibishuzu(smart_lock_input_password, smart_lockpassword,6);
            
            if(flag_yuanmima==0)
            {
                printf("����������\r\n");
            }else
            {
                OLED_Clear();
                OLED_ShowString(1, 1, "change password");
                OLED_ShowString(2, 1, "old password:");
                OLED_ShowString(4, 1, "error");
                printf("����ԭ�����������������\r\n");
            }
                      
            if(flag_yuanmima==0)
                {
                    OLED_Clear();
                    OLED_ShowString(1, 1, "new password:");
                    
                }
            while( flag_yuanmima==0)//������ȷ
            {
                 if(Flag_changeerror==1)
                 {
                    OLED_Clear();
                    OLED_ShowString(1, 1, "change password");
                    OLED_ShowString(2, 1, "new password:");
                    Flag_changeerror=0;
                 }

                 uint8_t flag_new_in_comp=0;
                 while(flag_leng_unlock==1 && Flag_changeerror==0 ) //����6λ�������
                 {
                     flag_new_in_comp=1;
                     flag_leng_unlock=0;
//                   flag_new_in_comp=0;
                     copy_array1_to2(smart_lock_input_password,password_temporary_1);

                     OLED_Clear();
                     OLED_ShowString(1, 1, "change password");
                     OLED_ShowString(2, 1, "second sure:");
                            

                     printf("zaici����������\r\n");
                     while( (flag_new_in_comp==1) && (Flag_changeerror==0))
                     {


                         while(flag_leng_unlock==1 && Flag_changeerror==0) //����6λ�������
                         {
                             flag_leng_unlock=0;
                             copy_array1_to2(smart_lock_input_password,password_temporary_2);
                             uint8_t flag_ercishuru=duibishuzu(password_temporary_1, password_temporary_2,6);
                             printf("flag_ercishuru%d\r\n",flag_ercishuru);
                             if(flag_ercishuru==0)
                             {
                                 OLED_Clear();
                                 OLED_ShowString(1, 1, "change password");
                                 OLED_ShowString(2, 1, "change success");
                                 printf("���������\r\n");
                                 copy_array1_to2(password_temporary_1,smart_lockpassword);
                                 stmflash_write(FLASH_SAVE_ADDR, (uint16_t *) smart_lockpassword, 6);
                                 Flag_Password_determination=0;
                                 function_key='Z';
                                 flag_yuanmima=1;
                                 flag_new_in_comp=0;
                                 delay_ms(2000);
                                 lock();
                                 
                             }else
                             {
                                  OLED_ShowString(1, 1, "change password");
                                  OLED_ShowString(2, 1, "change error");
                                  
                                  printf("�������벻һ��\r\n");
                                //Flag_Password_determination =0;
                                  function_key='Z';
                                //flag_yuanmima=1;
                                  Flag_changeerror=1;
                                  flag_new_in_comp=0;
                             }
                         
                     }
                    }
                 
                 }
            
            }
        }
    }
    

}
    




/**
 * @brief     qingkong
 * @param    
 * @retval    ��
 */
void qingkongshuzu(char * array)
{
        for (uint8_t j = 0; j <6; j++)
        {
            array[j] = '\0'   ;
        }

}



void  mode(uint8_t* function_key)
{

    switch(*function_key)
    {
        case 'A':    //printf("ȷ������");

                    Flag_Password_determination+=1;//����ȷ�ϼ�
        if(Flag_Password_determination==3&& Flag_password_true==1)
        {
            printf("�޸�����\r\n");
            Lock_Change_your_password();
        }
        if(Flag_Password_determination>3)
        {
            Flag_Password_determination=0;
        }
                    
       // printf("function_key:%s",function_key);
        
        break;//  

        case 'B':   //printf("�����˸�\r\n");
                   temp_smart_lock_input [temp_cent_one-1]='\0';
                   if((temp_cent_one-1)>=0)
                   {
                        temp_cent_one-=1;
                   }else
                    {
                        temp_cent_one=0;
                        
                    }
                    OLED_Clear();
                    OLED_ShowString(1, 6, "LOCK");
                    OLED_ShowString(2, 1, "input password:");
                   OLED_ShowString(3, 1, temp_smart_lock_input);
                  //  printf("temp_smart_lock_input:%s\r\n",temp_smart_lock_input);
                
        break;


        default:  break;
    }
    *function_key='Z';


}


/**
 * @brief     ����������������ж��Ƿ����
 * @param    
 * @retval    ��
 */  
void lock_input_char(void)
{
    if(Flag_password_true==1&&Flag_Password_determination!=3)
    {
        if((tick-flag_time_lock)>8)         
        {
            lock();
        }
        
    
    }

    mode(&function_key);
    if(flag_leng_unlock==1&&Flag_Password_determination!=3)
    {
         flag_leng_unlock=0;
        uint8_t flag_duibi_result;
        flag_duibi_result=duibishuzu(smart_lock_input_password,smart_lockpassword,6);
        if(flag_duibi_result==0)
        {

            unlock();
        }else
        {
               printf("�������\r\n");
        }
    }


}

/**
 * @brief     chushihua
 * @param    
 * @retval    ��
 */  
static void Key_EXTI_Mode(void)// A4-7exti 0-3output
{
//printf("Key_EXTI_Mode222/r/n");
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
    KEYBORD_GPIO_CLK_ENABLE();
    KEYBORD_EXTI_GPIO_CLK_ENABLE();
  //__HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(KEYBORD_GPIO_PORT, KEYBORD_GPIO_PIN, GPIO_PIN_RESET);
    
      /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = KEYBORD_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(KEYBORD_GPIO_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = KEYBORD_EXTI_GPIO_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEYBORD_EXTI_GPIO_PORT, &GPIO_InitStruct);



	  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(Column4_IRQn , 5, 0);
  HAL_NVIC_EnableIRQ(Column4_IRQn);

  HAL_NVIC_SetPriority(Column5_7_IRQn , 5, 0);
  HAL_NVIC_EnableIRQ(Column5_7_IRQn );
}

static void Key_Input_Mode(void)// A4-7output 0-3input
{
 //printf("Key_Input_Mode2/r/n");
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

 
  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static uint8_t Get_Key(uint8_t num)//����������ֵ��ӣ�����ֻ�õ���ǰ�˸�����ֻ�е� ��λ��
{ //printf("���� Get_Key/r/n");
	switch(num)
	{
		case 0x11:key='0';break;
		case 0x21:key='1';break;
		case 0x41:key='2';break;
		case 0x81:key='4';break;
		case 0x12:key='3';break;
		case 0x22:key='4';break;
		case 0x42:key='5';break;
		case 0x82:key='C';break;
		case 0x14:key='6';break;
		case 0x24:key='7';break;
		case 0x44:key='8';break;
		case 0x84:key='E';break;
		case 0x18:key='9';break;
		case 0x28:key='A';break;
		case 0x48:key='B';break;		
		case 0x88:key='F';break;	
	}
	return key;
}
//��������ʽ��ȡ����������0��û�м�ֵ����0����ü�ֵ
//�ú������ж��е���
uint8_t Get_KeyNum(uint16_t GPIO_Pin)
{ //printf("����Get_KeyNum/r/n");
	uint8_t i=0,num=0;

	num = (uint8_t)GPIO_Pin;
	
//	if(HAL_GetTick() - key_tic < KEY_DELAY) //�жϼ����С������
//	{
//		key_tic = HAL_GetTick();
//		return 0;
//	}
    delay_ms(10);
 //   printf("HAL_Delay\r\n");
//	HAL_Delay(1);
 //   printf("chuHAL_Delay\r\n");
//	key_tic = HAL_GetTick();
 //   printf("chuHAL_GetTick()\r\n");
	//��΢��ʱ�󣬶�ȡ�ܽ�״̬��ȥ�������ض���
	if( HAL_GPIO_ReadPin(GPIOA,GPIO_Pin) != GPIO_PIN_RESET ) 
		return 0;
	
	if(num)
	{
//        printf("num:%d\r\n",num);
//        printf("Key_Input_Mode--num\r\n");
		Key_Input_Mode();
		for(i=0;i<4;i++)
		{
			if(HAL_GPIO_ReadPin(GPIOA,(1<<i)) == GPIO_PIN_RESET)
				num |= (1<<i);
		}
		Key_EXTI_Mode();	
//        printf("num:%d\r\n",num);
		return Get_Key(num);
	}
	else
	{
		Key_EXTI_Mode();	
		return 0;
	}
}

void Key_Bord_Init(void)
{
   printf("Key_Bord_Init\r\n");
	Key_EXTI_Mode();
       printf("Key_EXTI_Mode()\r\n");
	key = 0;
            printf("HAL_GetTick\r\n");
	key_tic = HAL_GetTick();
           printf("chuHAL_GetTick%d\r\n",key_tic);
    stmflash_read(FLASH_SAVE_ADDR, (uint16_t *)smart_lockpassword, 6);
    printf("smart_lockpassword%s\r\n",smart_lockpassword);
    
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//ʵ��һ�»ص�����
{ //printf("����GPIO_EXTI_Callback/r/n");
	
	
	c = Get_KeyNum(GPIO_Pin);
	if(c != 0)//�жϰ����Ƿ������а�������
	{
        
        if('A'<=c && c<='F')
        {
            function_key=c;

        }else        
        {

                if(temp_cent_one<6)
                {
                    temp_smart_lock_input[temp_cent_one++]=c;
                  
                    OLED_ShowString(3, 1, temp_smart_lock_input);
                    if(temp_cent_one==6)
                    {
                        OLED_Clear();
                       OLED_ShowString(1, 6, "LOCK");
                    OLED_ShowString(2, 1, "input password:");
                        flag_leng_unlock=1;  //����6λ������ɱ�־
                        temp_cent_one=0;
                        copy_array1_to2(temp_smart_lock_input,smart_lock_input_password);//������6λ���븴��

                        qingkongshuzu(temp_smart_lock_input);                        

                    }
                }

        }
		printf("����ֵ:%c\n",c);
	}
}


/**
 * @brief       WK_UP �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void Column4_IRQHandler(void)
{  //printf("Column4_IRQHandler/r/n");
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);        /* �����жϴ����ú��� ���KEY_UP�����ж��� ���жϱ�־λ���ж��°벿��HAL_GPIO_EXTI_Callbackִ�� */
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);        /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}


/**
 * @brief       WK_UP �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void Column5_7_IRQHandler(void)
{ //printf("Column5_7_IRQHandler/r/n");
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);        /* �����жϴ����ú��� ���KEY_UP�����ж��� ���жϱ�־λ���ж��°벿��HAL_GPIO_EXTI_Callbackִ�� */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7); 


    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);        /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}



