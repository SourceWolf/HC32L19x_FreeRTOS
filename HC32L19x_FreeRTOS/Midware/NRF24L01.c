#include "NRF24L01.h"
#define NRF_CE_PORT	GpioPortE
#define NRF_CE_Pin	GpioPin11
#define NRF_IRQ_PORT	GpioPortE
#define NRF_IRQ_Pin	GpioPin10
//#define NRF_CS_PORT	PortE
//#define NRF_CS_Pin	Pin01
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
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
 	NRF_CS_Clr();          //使能SPI传输
//  	status = SPIx_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
//  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPIx_ReadWriteByte(*pBuf++); //写入数据	       
	Spi_SendBuf(M0P_SPI0,&reg,1);
	Spi_SendBuf(M0P_SPI0,pBuf,len);
	NRF_CS_Set();//关闭SPI传输
  	return status;          //返回读到的状态值
}
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,u8_ctr;	       
  	NRF_CS_Clr();          //使能SPI传输      
	Spi_SendBuf(M0P_SPI0,&reg,1);
	Spi_ReceiveBuf(M0P_SPI0,pBuf,len);
	NRF_CS_Set();//关闭SPI传输       //关闭SPI传输
  	return status;        //返回读到的状态值
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
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
	
}
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void RX_Mode(void)
{
	Gpio_ClrIO(NRF_CE_PORT,NRF_CE_Pin);	  
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin); //CE为高,进入接收模式 
}		
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void TX_Mode(void)
{														 
	Gpio_ClrIO(NRF_CE_PORT,NRF_CE_Pin);		    
  	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);//CE为高,10us后启动发送
}		  
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
// 	SPIx_SetSpeed(SPI_SPEED_8);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	Gpio_ClrIO(NRF_CE_PORT,NRF_CE_Pin);	
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);//启动发送	   
	while(Gpio_GetInputIO(NRF_IRQ_PORT,NRF_IRQ_Pin)!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);
		return TX_OK;
	}
	Gpio_SetIO(NRF_CE_PORT,NRF_CE_Pin);
	return 0xff;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
//	SPIx_SetSpeed(SPI_SPEED_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
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