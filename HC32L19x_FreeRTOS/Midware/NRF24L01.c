#include "NRF24L01.h"
#define NRF_CE_PORT	GpioPortE
#define NRF_CE_Pin	GpioPin11
#define NRF_IRQ_PORT	GpioPortE
#define NRF_IRQ_Pin	GpioPin10
//#define NRF_CS_PORT	PortE
//#define NRF_CS_Pin	Pin01
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
void NRF24L01_Init(void)
{
	stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
	GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut; 
	Gpio_Init(NRF_CE_PORT, NRF_CE_Pin, &GpioInitStruct);
	GpioInitStruct.enDir = GpioDirIn;                          
    Gpio_Init(NRF_IRQ_PORT, NRF_IRQ_Pin, &GpioInitStruct);  
	Hd_SPI0_Init();
}
void NRF_CS_Set(void)
{
//	PORT_SetBits(NRF_CS_PORT, NRF_CS_Pin);
	Spi_SetCS(M0P_SPI0, TRUE);
}
void NRF_CS_Clr(void)
{
//	PORT_ResetBits(NRF_CS_PORT, NRF_CS_Pin);
	Spi_SetCS(M0P_SPI0, FALSE);
}
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;	    
 	NRF_CS_Clr();          //ʹ��SPI����
//  	status = SPIx_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
//  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPIx_ReadWriteByte(*pBuf++); //д������	       
	Spi_SendBuf(M0P_SPI0,&reg,1);
	Spi_SendBuf(M0P_SPI0,pBuf,len);
	NRF_CS_Set();//�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,u8_ctr;	       
  	NRF_CS_Clr();          //ʹ��SPI����      
	Spi_SendBuf(M0P_SPI0,&reg,1);
	Spi_ReceiveBuf(M0P_SPI0,pBuf,len);
	NRF_CS_Set();//�ر�SPI����       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
void NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	NRF_CS_Clr();
	Spi_SendBuf(M0P_SPI0,&reg,1);
	Spi_SendBuf(M0P_SPI0,&value,1);
	NRF_CS_Set();
}
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	NRF_CS_Clr();
	Spi_SendBuf(M0P_SPI0,&reg,1);
	Spi_ReceiveBuf(M0P_SPI0,&reg_val,1);
	NRF_CS_Set();
	return reg_val;
}
boolean_t NRF24L01_check(void)
{
	uint8_t buf[5];
	uint8_t i;
	for(i = 0; i<5; i++)
	{
		buf[i] = 0xA5;
	}
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
	
}
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void RX_Mode(void)
{
	Gpio_ClrIO(NRF_CE_PORT,NRF_CE_Pin);	  
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin); //CEΪ��,�������ģʽ 
}		
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void TX_Mode(void)
{														 
	Gpio_ClrIO(NRF_CE_PORT,NRF_CE_Pin);		    
  	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);//CEΪ��,10us����������
}		  
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
// 	SPIx_SetSpeed(SPI_SPEED_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	Gpio_ClrIO(NRF_CE_PORT,NRF_CE_Pin);	
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);//��������	   
	while(Gpio_GetInputIO(NRF_IRQ_PORT,NRF_IRQ_Pin)!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);
		return TX_OK;
	}
	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
//	SPIx_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}
void Test_NRF24L01_TX(void)
{
	uint8_t tmp_buf[32], i;
	for(i= 0;i<32;i++)
	{
		tmp_buf[i] = i;
	}
	TX_Mode();
	NRF24L01_TxPacket(tmp_buf);
}
void Test_NRF24L01_RX(void)
{
	uint8_t tmp_buf[32], i;
	for(i= 0;i<32;i++)
	{
		tmp_buf[i] = 0;
	}
	RX_Mode();
	NRF24L01_RxPacket(tmp_buf);
}