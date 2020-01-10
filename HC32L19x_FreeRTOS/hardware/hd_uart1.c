#include "hd_uart1.h"
uint8_t u8RxData;
//UART1中断函数
void Uart1_IRQHandler(void)
{
    if(Uart_GetStatus(UART1_UNIT, UartRC))         //UART1数据接收
    {
        Uart_ClrStatus(UART1_UNIT, UartRC);        //清中断状态位
        u8RxData = Uart_ReceiveData(M0P_UART1);   //接收数据字节
    }
    
    if(Uart_GetStatus(UART1_UNIT, UartTC))         //UART1数据发送
    {
        Uart_ClrStatus(UART1_UNIT, UartTC);        //清中断状态位
    }

}

en_result_t Hd_uart1_init(void)
{
    en_result_t Res;
    stc_uart_config_t    stcCfg;
    stc_gpio_config_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcCfg);    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //使能GPIO模块时钟
    
    ///<TX
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(UART1_RX_PORT, UART1_RX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(UART1_RX_PORT, UART1_RX_PIN, UART1_RX_FUN);          //配置PA02 端口为URART1_TX
    
    ///<RX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(UART1_TX_PORT, UART1_TX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(UART1_TX_PORT, UART1_TX_PIN, UART1_TX_FUN);          //配置PA03 端口为URART1_RX
    
    ///< 开启外设时钟
    Sysctrl_SetPeripheralGate(UART1_CLK_EN,TRUE);///<使能uart1模块时钟
    
    ///<UART Init
    stcCfg.enRunMode        = UartMskMode3;          ///<模式3
    stcCfg.enStopBit        = UartMsk1bit;           ///<1bit停止位
    stcCfg.enMmdorCk        = UartMskEven;           ///<偶检验
    stcCfg.stcBaud.u32Baud  = BAUDRATE;                  ///<波特率9600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<通道采样分频配置
    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<获得外设时钟（PCLK）频率值
    Res = Uart_Init(UART1_UNIT, &stcCfg);                   ///<串口初始化
    if(Res != Ok)
    {
        return Res;
    }
    ///<UART中断使能
    Uart_ClrStatus(UART1_UNIT,UartRC);                ///<清接收请求
    Uart_ClrStatus(UART1_UNIT,UartTC);                ///<清接收请求
    Uart_EnableIrq(UART1_UNIT,UartRxIrq);             ///<使能串口接收中断
    Uart_EnableIrq(UART1_UNIT,UartTxIrq);             ///<使能串口接收中断    
    EnableNvic(UART1_IRQ, IrqLevel3, TRUE);       ///<系统中断使能
    return Res;
}