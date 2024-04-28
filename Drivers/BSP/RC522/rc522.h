#ifndef __RC522_H
#define __RC522_H
#include "./SYSTEM/sys/sys.h"

/*                  103SPI1 103SPI2  407SPI1 407SPI2
  1--SDA(CS) <-----> PA3      PB12             PC0  
  2--SCK     <-----> PA5      PB13            PB13
  3--MOSI    <-----> PA7      PB15            PC3
  4--MISO    <-----> PA6      PB14            PC2
  5--IRQ     <-----> ����     ����            ����
  6--GND     <-----> GND      GND             GND
  7--RST     <-----> PA4      PA4             PC6
  8--3.3V    <-----> 3.3V     3.3V            3.3V  */
	
////////////////////////////////////////��ֲ�޸�����///////////////////////////////////////////////////////////
//.c �ļ������κ��޸�

#define RC522_CS(x)                     MySPI_W_SS()

#define RC522_GPIO_CLK_RST_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PA��ʱ��ʹ�� */
#define RC522_GPIOx_RST                    GPIOB
#define RC522_RST_Pin                      GPIO_PIN_1
#define RC522_RST(x)                          (x?HAL_GPIO_WritePin( RC522_GPIOx_RST, RC522_RST_Pin, GPIO_PIN_SET)  \
                                            : HAL_GPIO_WritePin( RC522_GPIOx_RST,  RC522_RST_Pin, GPIO_PIN_RESET) );

#define RC522_delay_us                     delay_us


                                                                   //����оƬ�ͺ���SPI���÷�Ƶ��ֵ
                                                                   //103:SPI1-APB2=72M  SPI2-APB1=36M 
                                                                   //407:SPI1-APB2=84M  SPI2-APB1=42M
////////////////////////////////////////��ֲ�޸�����///////////////////////////////////////////////////////////

#if RC522_SPI_SELECT==1
	#include "spi1.h"
	#define RC522_SPI                        SPI1
	#define RC522_SPIx_Init                  SPI1_Init
	#define RC522_SPIx_ReadWriteByte         SPI1_ReadWriteByte
	#define RC522_SPI_SetSpeed               SPI1_SetSpeed  
	#if DMAx_SPI1==1
	#define RC522_DMA                        1 
	#define RC522_DMAx_SPIx_RX               DMAx_SPI1_RX
	#define RC522_DMAx_SPIx_TX               DMAx_SPI1_TX
	#endif
#endif
#if RC522_SPI_SELECT==2
	#include "./BSP/SPI/spi.h"
	#define RC522_SPI                        SPI2
	#define RC522_SPIx_Init                  spi2_init
	#define RC522_SPIx_ReadWriteByte         spi2_read_write_byte
	#define RC522_SPI_SetSpeed               spi2_set_speed

	#if DMAx_SPI2==1
	#define RC522_DMA                        1 
	#define RC522_DMAx_SPIx_RX               DMAx_SPI2_RX
	#define RC522_DMAx_SPIx_TX               DMAx_SPI2_TX
	#endif
#endif


extern uint8_t RFID_MoRen_KongZhiKuai[16];      //Ĭ�Ͽ��ƿ�

/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
/*
#define	SHAQU1		            0X01
#define	KUAI4			            0X04
#define	KUAI7			            0X07
#define	REGCARD		            0xa1
#define	CONSUME		            0xa2
#define READCARD	            0xa3
#define ADDMONEY	            0xa4
*/
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH         64              //FIFO size=64byte
#define MAXRLEN                 18

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define RFU00                 0x00    
#define CommandReg            0x01    
#define ComIEnReg             0x02    
#define DivlEnReg             0x03    
#define ComIrqReg             0x04    
#define DivIrqReg             0x05
#define ErrorReg              0x06    
#define Status1Reg            0x07    
#define Status2Reg            0x08    
#define FIFODataReg           0x09
#define FIFOLevelReg          0x0A
#define WaterLevelReg         0x0B
#define ControlReg            0x0C
#define BitFramingReg         0x0D
#define CollReg               0x0E
#define RFU0F                 0x0F
// PAGE 1     
#define RFU10                 0x10
#define ModeReg               0x11
#define TxModeReg             0x12
#define RxModeReg             0x13
#define TxControlReg          0x14
#define TxAutoReg             0x15
#define TxSelReg              0x16
#define RxSelReg              0x17
#define RxThresholdReg        0x18
#define DemodReg              0x19
#define RFU1A                 0x1A
#define RFU1B                 0x1B
#define MifareReg             0x1C
#define RFU1D                 0x1D
#define RFU1E                 0x1E
#define SerialSpeedReg        0x1F
// PAGE 2    
#define RFU20                 0x20  
#define CRCResultRegM         0x21
#define CRCResultRegL         0x22
#define RFU23                 0x23
#define ModWidthReg           0x24
#define RFU25                 0x25
#define RFCfgReg              0x26
#define GsNReg                0x27
#define CWGsCfgReg            0x28
#define ModGsCfgReg           0x29
#define TModeReg              0x2A
#define TPrescalerReg         0x2B
#define TReloadRegH           0x2C
#define TReloadRegL           0x2D
#define TCounterValueRegH     0x2E
#define TCounterValueRegL     0x2F
// PAGE 3      
#define RFU30                 0x30
#define TestSel1Reg           0x31
#define TestSel2Reg           0x32
#define TestPinEnReg          0x33
#define TestPinValueReg       0x34
#define TestBusReg            0x35
#define AutoTestReg           0x36
#define VersionReg            0x37
#define AnalogTestReg         0x38
#define TestDAC1Reg           0x39  
#define TestDAC2Reg           0x3A   
#define TestADCReg            0x3B   
#define RFU3C                 0x3C   
#define RFU3D                 0x3D   
#define RFU3E                 0x3E   
#define RFU3F		  		        0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_OK                  (0)
#define MI_NOTAGERR            (1)
#define MI_ERR                 (2)




//RC522������������
void Rc522_Test(void);
void Rc522_Init(void);   //RC522��Ƶ��ģ���ʼ��
void ResetRC522(void);  //��λRC522������

void ClearBitMask(uint8_t reg,uint8_t mask);     //��RC522�Ĵ���λ
void WriteRawRC(uint8_t Address,uint8_t value); //дRC632�Ĵ���
void SetBitMask(uint8_t reg,uint8_t mask);    //��RC522�Ĵ���λ
void CalulateCRC(uint8_t *pIn,uint8_t len,uint8_t *pOut );   //��MF522����CRC16����
uint8_t   ReadRawRC(uint8_t Address);      //��RC632�Ĵ���


uint8_t   PcdReset(void);          //��λRC522
uint8_t   PcdRequest(uint8_t req_code,uint8_t *pTagType);   //Ѱ��
void      PcdAntennaOn(void);     //�������� 
void      PcdAntennaOff(void);     //�ر�����
uint8_t   PcdConfigISOType(uint8_t type);    //����RC632�Ĺ�����ʽ 
uint8_t   PcdAnticoll(uint8_t *pSnr);    //����ײ
uint8_t   PcdSelect(uint8_t *pSnr);  //ѡ����Ƭ
uint8_t   PcdAuthState(uint8_t *pSnr,uint8_t addr,uint8_t auth_mode,uint8_t *pKey); //��֤��Ƭ����
uint8_t   PcdWrite(uint8_t addr,uint8_t *pData);    //д���ݵ�M1��һ��
uint8_t   PcdRead(uint8_t addr,uint8_t *pData);  //��ȡM1��һ������
uint8_t   PcdHalt(void);      //���Ƭ��������״̬
uint8_t   PcdComMF522(uint8_t Command,uint8_t *pIn ,uint8_t InLenByte,uint8_t *pOut,uint8_t *pOutLenBit);//ͨ��RC522��ISO14443��ͨѶ


uint8_t   PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue);    //�ۿ�ͳ�ֵ
uint8_t   PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr);      //����Ǯ��

#endif


