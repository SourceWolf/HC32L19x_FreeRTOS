#include "hd_spi.h"
void Hd_SPI0_Init(void)
{
	stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    
    ///< SPI0
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;   

    Gpio_Init(STK_SPI0_CS_PORT, STK_SPI0_CS_PIN, &GpioInitStruct);
    Gpio_SetAfMode(STK_SPI0_CS_PORT, STK_SPI0_CS_PIN, GpioAf2);             //SPI0_CS
                                                               
    Gpio_Init(STK_SPI0_SCK_PORT, STK_SPI0_SCK_PIN, &GpioInitStruct);          
    Gpio_SetAfMode(STK_SPI0_SCK_PORT, STK_SPI0_SCK_PIN, GpioAf2);           //SPI0_SCK
                                                               
    Gpio_Init(STK_SPI0_MOSI_PORT, STK_SPI0_MOSI_PIN, &GpioInitStruct);           
    Gpio_SetAfMode(STK_SPI0_MOSI_PORT, STK_SPI0_MOSI_PIN, GpioAf2);         //SPI0_MOSI
                                                               
    GpioInitStruct.enDir = GpioDirIn;                          
    Gpio_Init(STK_SPI0_MISO_PORT, STK_SPI0_MISO_PIN, &GpioInitStruct);            
    Gpio_SetAfMode(STK_SPI0_MISO_PORT, STK_SPI0_MISO_PIN, GpioAf2);         //SPI0_MISO
	
	stc_spi_cfg_t  SpiInitStruct;    
    
    //
    Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);
    
    //
    Reset_RstPeripheral0(ResetMskSpi0);
    
    //SPI0
    SpiInitStruct.enSpiMode = SpiMskMaster;     
    SpiInitStruct.enPclkDiv = SpiClkMskDiv128;  
    SpiInitStruct.enCPHA    = SpiMskCphafirst;  
    SpiInitStruct.enCPOL    = SpiMskcpollow;    
    Spi_Init(M0P_SPI0, &SpiInitStruct);
}
void SPIx_ReadWriteByte(uint8_t *Txdata,uint8_t *Rxdata,uint8_t len)
{
	Spi_SetCS(M0P_SPI0, FALSE);
    Spi_SendBuf(M0P_SPI0, (uint8_t*)Txdata, len);   
    Spi_SetCS(M0P_SPI0, TRUE);
	Spi_SetCS(M0P_SPI0, FALSE);
    Spi_ReceiveBuf(M0P_SPI0, Rxdata, len);   
    Spi_SetCS(M0P_SPI0, TRUE);
}