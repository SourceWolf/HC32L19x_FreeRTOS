#include "hd_gpio.h"

void Port_init(void)
{
    stc_gpio_config_t stcGpioCfg;

    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    ///< 端口方向配置->输出
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口驱动能力配置->高驱动能力
    stcGpioCfg.enDrv = GpioDrvH;
    ///< 端口上下拉配置->无上下拉
    stcGpioCfg.enPu = GpioPuDisable;
	stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;

    ///< GPIO IO LED端口初始化
    Gpio_Init(EVB_LEDR_PORT, EVB_LEDR_PIN, &stcGpioCfg);
    Gpio_Init(EVB_LEDY_PORT, EVB_LEDY_PIN, &stcGpioCfg);
    Gpio_Init(EVB_LEDG_PORT, EVB_LEDG_PIN, &stcGpioCfg);
    Gpio_Init(EVB_LEDB_PORT, EVB_LEDB_PIN, &stcGpioCfg);
    ///< GPIO IO PA01初始化
    Gpio_Init(GpioPortA, GpioPin1, &stcGpioCfg);
    ///< GPIO IO PB00初始化
    Gpio_Init(GpioPortB, GpioPin0, &stcGpioCfg);
    
    ///< 设置PLL从PB00输出
    Gpio_SetAfMode(GpioPortB, GpioPin0, GpioAf7);
    
    ///< 设置HCLK从PA01输出
    Gpio_SfHClkOutputConfig(GpioSfHclkOutEnable, GpioSfHclkOutDiv1);
    Gpio_SetAfMode(GpioPortA, GpioPin1, GpioAf6);
    
    
    



}


