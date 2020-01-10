#include "hd_uart1.h"
uint8_t u8RxData;
//UART1�жϺ���
void Uart1_IRQHandler(void)
{
    if(Uart_GetStatus(UART1_UNIT, UartRC))         //UART1���ݽ���
    {
        Uart_ClrStatus(UART1_UNIT, UartRC);        //���ж�״̬λ
        u8RxData = Uart_ReceiveData(M0P_UART1);   //���������ֽ�
    }
    
    if(Uart_GetStatus(UART1_UNIT, UartTC))         //UART1���ݷ���
    {
        Uart_ClrStatus(UART1_UNIT, UartTC);        //���ж�״̬λ
    }

}

en_result_t Hd_uart1_init(void)
{
    en_result_t Res;
    stc_uart_config_t    stcCfg;
    stc_gpio_config_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcCfg);    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //ʹ��GPIOģ��ʱ��
    
    ///<TX
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(UART1_RX_PORT, UART1_RX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(UART1_RX_PORT, UART1_RX_PIN, UART1_RX_FUN);          //����PA02 �˿�ΪURART1_TX
    
    ///<RX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(UART1_TX_PORT, UART1_TX_PIN, &stcGpioCfg);
    Gpio_SetAfMode(UART1_TX_PORT, UART1_TX_PIN, UART1_TX_FUN);          //����PA03 �˿�ΪURART1_RX
    
    ///< ��������ʱ��
    Sysctrl_SetPeripheralGate(UART1_CLK_EN,TRUE);///<ʹ��uart1ģ��ʱ��
    
    ///<UART Init
    stcCfg.enRunMode        = UartMskMode3;          ///<ģʽ3
    stcCfg.enStopBit        = UartMsk1bit;           ///<1bitֹͣλ
    stcCfg.enMmdorCk        = UartMskEven;           ///<ż����
    stcCfg.stcBaud.u32Baud  = BAUDRATE;                  ///<������9600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<ͨ��������Ƶ����
    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<�������ʱ�ӣ�PCLK��Ƶ��ֵ
    Res = Uart_Init(UART1_UNIT, &stcCfg);                   ///<���ڳ�ʼ��
    if(Res != Ok)
    {
        return Res;
    }
    ///<UART�ж�ʹ��
    Uart_ClrStatus(UART1_UNIT,UartRC);                ///<���������
    Uart_ClrStatus(UART1_UNIT,UartTC);                ///<���������
    Uart_EnableIrq(UART1_UNIT,UartRxIrq);             ///<ʹ�ܴ��ڽ����ж�
    Uart_EnableIrq(UART1_UNIT,UartTxIrq);             ///<ʹ�ܴ��ڽ����ж�    
    EnableNvic(UART1_IRQ, IrqLevel3, TRUE);       ///<ϵͳ�ж�ʹ��
    return Res;
}