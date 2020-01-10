#include "hd_gpio.h"

void Port_init(void)
{
    stc_gpio_config_t stcGpioCfg;

    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    ///< �˿ڷ�������->���
    stcGpioCfg.enDir = GpioDirOut;
    ///< �˿�������������->����������
    stcGpioCfg.enDrv = GpioDrvH;
    ///< �˿�����������->��������
    stcGpioCfg.enPu = GpioPuDisable;
	stcGpioCfg.enPd = GpioPdDisable;
    ///< �˿ڿ�©�������->��©����ر�
    stcGpioCfg.enOD = GpioOdDisable;
    ///< �˿�����/���ֵ�Ĵ������߿���ģʽ����->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;

    ///< GPIO IO LED�˿ڳ�ʼ��
    Gpio_Init(EVB_LEDR_PORT, EVB_LEDR_PIN, &stcGpioCfg);
    Gpio_Init(EVB_LEDY_PORT, EVB_LEDY_PIN, &stcGpioCfg);
    Gpio_Init(EVB_LEDG_PORT, EVB_LEDG_PIN, &stcGpioCfg);
    Gpio_Init(EVB_LEDB_PORT, EVB_LEDB_PIN, &stcGpioCfg);
    ///< GPIO IO PA01��ʼ��
    Gpio_Init(GpioPortA, GpioPin1, &stcGpioCfg);
    ///< GPIO IO PB00��ʼ��
    Gpio_Init(GpioPortB, GpioPin0, &stcGpioCfg);
    
    ///< ����PLL��PB00���
    Gpio_SetAfMode(GpioPortB, GpioPin0, GpioAf7);
    
    ///< ����HCLK��PA01���
    Gpio_SfHClkOutputConfig(GpioSfHclkOutEnable, GpioSfHclkOutDiv1);
    Gpio_SetAfMode(GpioPortA, GpioPin1, GpioAf6);
    
    
    



}


