#include "./BSP/RC522/rc522.h"
#include "./BSP/KEYBORD/keybord.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SPI2/MySPI.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/delay/delay.h"
#include "string.h"
uint8_t RFID_MoRen_KongZhiKuai[16]={0xff,0xff,0xff,0xff,0xff,0xff, //Ĭ������A
							   0xff,0x07,0x80,0x69,           //Ĭ�Ͽ��ƿ�
							   0xff,0xff,0xff,0xff,0xff,0xff};//Ĭ������B  ����д���ƿ�ʱ����Ĭ�ϲ��Կ��д����ֹ������������
 
void POWER_RC522(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE();                                 /* LED0ʱ��ʹ�� */
    gpio_init_struct.Pin = GPIO_PIN_0;                   /* LED0���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* ���� */
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);       /* ��ʼ��LED0���� */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    
    gpio_init_struct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);       /* ��ʼ��LED0���� */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

}
void Rc522_Test(void)
{

		uint8_t KEYA[6]={0xff,0xff,0xff,0xff,0xff,0xff};         //Ĭ������A
		//uint8_t KEYB[6]={0xff,0xff,0xff,0xff,0xff,0xff};         //Ĭ������B
 
		uint8_t RFID_Temp_Data[16]={0x00,0x01,0x02,0x03,
		                       0x04,0x05,0x06,0x07,
		                       0x08,0x09,0x0a,0x0b,
		                       0x0c,0x0d,0x0e,0x0f};        //����д����
		
		uint8_t CT[2];                                           //������
		uint8_t SN[4];                                           //����
		uint8_t RFID[16];			                            //���RFID 
		
		uint8_t status;                                          
		uint8_t t;
		
//		Rc522_Init();
		
//		while(1)
//		{

				status = PcdRequest(PICC_REQIDL,CT);//Ѱ��,���ߵĲ�Ҫ ���PcdHalt(); ��ֹ�ظ�����
                          
				if(status==MI_ERR)
				{
				//	printf("�޿��������ſ�\r\n");
				}
				else
				{
					//printf("�����ͣ�0x%04X\r\n",(CT[0]<<8)+CT[1]);
					status = PcdAnticoll(SN);//����ײ
					if(status==MI_OK)
					{
						status =PcdSelect(SN);//ѡ����Ƭ,���Ŵ���SN
						if(status==MI_OK)
						{
							if(status==MI_OK)
							{
								//printf("���ţ�");
								for(t=0;t<4;t++){
									//printf("%02X ",SN[t]);		 
                                    }
								//printf("\r\n");					
							}
							else
							{
								printf("ѡ����Ƭʧ��\r\n");
							} 
							status =PcdAuthState(SN,0x07,PICC_AUTHENT1A,KEYA);//������KEYA ��֤����SN�Ŀ��ƿ�7��0-63���� A��Կ 
							if(status==MI_OK)                                 //��֤��7���Ϳ��Զ�д4 5 6����
							{
								//printf("������KEYA ��֤����SN�Ŀ��ƿ�7��0-63���� A��Կ �ɹ�\r\n");
								status=PcdWrite(0x06,RFID_Temp_Data);
								if(status==MI_OK)
								{
										//printf("д0x06�����ݣ�");
										for(t=0;t<16;t++)
											printf("%02X ",RFID_Temp_Data[t]);		 
										//printf("\r\n");	
										status=PcdRead(0x06,RFID);
										 if(status==MI_OK)
										 {
                                            unlock();
											//printf("��0x06�����ݣ�");
											for(t=0;t<16;t++)
                                             {
												//printf("%02X ",RFID[t]);		 
											//printf("\r\n");	
                                            }
										 }
										 else
										 {
												 printf("����ʧ��\r\n");
										 } 
								 }
								 else
								 {
								     printf("д��ʧ��\r\n");
								 }
							}
							else
							{
								printf("������KEYA ��֤����SN�Ŀ��ƿ�7��0-63���� A��Կ ʧ��\r\n");
							}
						}
						else
						{
							printf("����ײʧ��\r\n");
						} 
					}
                    PcdHalt();//���Ƭ��������״̬,ֱ����̧��Ż�����ʶ��
				}
				for(t=0;t< 2;t++)CT  [t]=0;	
				for(t=0;t< 4;t++)SN  [t]=0;	
				for(t=0;t<16;t++)RFID[t]=0;
				printf("\r\n\r\n");
				delay_ms(500);
//		}
}


void Rc522_IO_Init(void)
{
    POWER_RC522();
    GPIO_InitTypeDef gpio_init_struct;    
    RC522_GPIO_CLK_RST_ENABLE();

    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Pin = RC522_RST_Pin;
    HAL_GPIO_Init(RC522_GPIOx_RST, &gpio_init_struct);

    MySPI_W_SS(1); 			//ȡ��Ƭѡ

    MySPI_Init();



		MySPI_W_SS(0); 			//ʹ��
		RC522_RST(1);			//��λ 

}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
uint8_t PcdReset(void)
{
	  RC522_RST(1);
    RC522_delay_us(10);

	  RC522_RST(0);
    RC522_delay_us(10);

	  RC522_RST(1);
    RC522_delay_us(10);

    WriteRawRC(CommandReg,PCD_RESETPHASE);
	  WriteRawRC(CommandReg,PCD_RESETPHASE);
    RC522_delay_us(10);
    
    WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
	
	  WriteRawRC(TxAutoReg,0x40);//����Ҫ
   
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    uint8_t   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
        SetBitMask(TxControlReg, 0x03);
}

//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ 
//////////////////////////////////////////////////////////////////////
uint8_t PcdConfigISOType(uint8_t   type)
{
		if (type == 'A')                     //ISO14443_A
		{ 
				ClearBitMask(Status2Reg,0x08);
				WriteRawRC(ModeReg,0x3D);//3F
				WriteRawRC(RxSelReg,0x86);//84
				WriteRawRC(RFCfgReg,0x7F);   //4F
				WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
				WriteRawRC(TReloadRegH,0);
				WriteRawRC(TModeReg,0x8D);
				WriteRawRC(TPrescalerReg,0x3E);
				RC522_delay_us(1000);
				PcdAntennaOn();
		}
		else
				return 1; 

		return MI_OK;
}

void ResetRC522(void)
{
	PcdReset();       //��λRC522
	PcdAntennaOff();  //�ر�����
	PcdAntennaOn();   //��������
}    
//RC522��Ƶ��ģ���ʼ��
void Rc522_Init(void)
{
	Rc522_IO_Init();
	PcdReset();                 //��λRC522
	PcdAntennaOff();            //�ر�����
	PcdAntennaOn();             //��������
	PcdConfigISOType( 'A' );    //ISO14443_A
    



}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
uint8_t PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
	uint8_t   status;  
	uint8_t   unLen;
	uint8_t   ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);
  
	WriteRawRC(BitFramingReg,0x07);
  
	SetBitMask(TxControlReg,0x03);

 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	   status = MI_ERR;   
   
	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
uint8_t PcdAnticoll(uint8_t *pSnr)
{
    uint8_t   status;
    uint8_t   i,snr_check=0;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	   for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
            status = MI_ERR;    
    }
    
    SetBitMask(CollReg,0x80);
    return status;
    
 
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
uint8_t PcdSelect(uint8_t *pSnr)
{
    uint8_t   status;
    uint8_t   i;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
       status = MI_OK;  
    else
       status = MI_ERR;    

    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: 
//          pSnr[IN]     ����Ƭ���кţ�4�ֽ�
//          addr[IN]     �����ַ
//          auth_mode[IN]: ������֤ģʽ PICC_AUTHENT1A=0x60 ��֤A��Կ  PICC_AUTHENT1B=0x61 ��֤B��Կ 
//          pKey[IN]     ������

//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////               
uint8_t PcdAuthState(uint8_t *pSnr,uint8_t addr,uint8_t auth_mode,uint8_t *pKey)
{

    uint8_t   status;
    uint8_t  unLen;
    uint8_t  ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6); 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
       status = MI_ERR;   
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          p [OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
uint8_t PcdRead(uint8_t   addr,uint8_t *p )
{
    uint8_t   status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
       memcpy(p , ucComMF522Buf, 16);   
    else
       status = MI_ERR;   
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          p [IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                  
uint8_t PcdWrite(uint8_t   addr,uint8_t *p )
{
    uint8_t   status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, p , 16);
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
           status = MI_ERR;   
    }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
uint8_t PcdHalt(void)
{
    uint8_t   status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return status;
}


/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(uint8_t *pIn ,uint8_t   len,uint8_t *pOut )
{
    uint8_t   i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
       WriteRawRC(FIFODataReg, *(pIn +i)); 
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);

}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
uint8_t ReadRawRC(uint8_t   Address)
{
      
		uint8_t   ucAddr;
		uint8_t   ucResult=0;

    
		MySPI_W_SS(0); 
          
		ucAddr = ((Address<<1)&0x7E)|0x80;
       

        MySPI_SwapByte(ucAddr);
          
        ucResult=MySPI_SwapByte(0);

		MySPI_W_SS(1); 
//            printf("ucResult%x\r\n",ucResult);
		return ucResult;

}


/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(uint8_t   Address, uint8_t   value)
{  
    uint8_t   ucAddr;



    MySPI_W_SS(0); 

    ucAddr = ((Address<<1)&0x7E);
    MySPI_SwapByte(ucAddr);
    MySPI_SwapByte(value);

    MySPI_W_SS(1); 
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t   reg,uint8_t   mask)  
{
    uint8_t   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t   reg,uint8_t   mask)  
{
    uint8_t   tmp = 0x0;
    tmp = ReadRawRC(reg);

    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
  
} 


/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
uint8_t PcdComMF522(uint8_t   Command, //
                 uint8_t *pIn , 
                 uint8_t   InLenByte,
                 uint8_t *pOut , 
                 uint8_t *pOutLenBit)
{
    uint8_t   status = MI_ERR;
    uint8_t   irqEn   = 0x00;
    uint8_t   waitFor = 0x00;
    uint8_t   lastBits;
    uint8_t   n;
    uint16_t  i;
    switch (Command)
    {
        case PCD_AUTHENT:
						irqEn   = 0x12;
						waitFor = 0x10;
						break;
				case PCD_TRANSCEIVE:
						irqEn   = 0x77;
						waitFor = 0x30;
						break;
				default:	break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);	//�������ж�λ
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);//��FIFO����
    
    for (i=0; i<InLenByte; i++)
       WriteRawRC(FIFODataReg, pIn [i]);    
    WriteRawRC(CommandReg, Command);	  
    
    if (Command == PCD_TRANSCEIVE)
        SetBitMask(BitFramingReg,0x80);  	 //��ʼ����
    										 
    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do 
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {    
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
               status = MI_NOTAGERR;   
            if (Command == PCD_TRANSCEIVE)
            {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                   *pOutLenBit = (n-1)*8 + lastBits;   
                else
                  *pOutLenBit = n*8;   
                if (n == 0)
                   n = 1;    
                if (n > MAXRLEN)
                   n = MAXRLEN;   
                for (i=0; i<n; i++)
                   pOut [i] = ReadRawRC(FIFODataReg);    
            }
        }
        else
           status = MI_ERR;   
    }

    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                 
uint8_t PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
    uint8_t status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
	
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
       status = MI_ERR;   
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        status = MI_OK;    
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
           status = MI_ERR;   
    }
    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
uint8_t PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr)
{
    uint8_t status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
       status = MI_ERR;   
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		    if (status != MI_ERR)
            status = MI_OK;    
    }
    
    if (status != MI_OK)
        return MI_ERR;   
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
       status = MI_ERR;   

    return status;
}



