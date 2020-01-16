#include "ddl.h"
#include "gpio.h"
#include "lpm.h"
void App_LowPowerModeGpioSet(void)
{
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //swd as gpio
//    Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);
    
    ///< 配置为数字端口
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PCADS = 0;
    M0P_GPIO->PDADS = 0;
    M0P_GPIO->PEADS = 0;
    M0P_GPIO->PFADS = 0;

    M0P_GPIO->PADIR = 0XFFFF;
    M0P_GPIO->PBDIR = 0XFFFF;
    M0P_GPIO->PCDIR = 0XFFFF;
    M0P_GPIO->PDDIR = 0XBFFF;
    M0P_GPIO->PEDIR = 0XFFFF;
    M0P_GPIO->PFDIR = 0XFFFF;

    M0P_GPIO->PAPD = 0xFFFF;
    M0P_GPIO->PBPD = 0xFFFF;
    M0P_GPIO->PCPD = 0xFFFF;
    M0P_GPIO->PDPD = 0xFFFF;
    M0P_GPIO->PEPD = 0xFFFF;
    M0P_GPIO->PFPD = 0xFFFF;
    
}

void System_EnterDeepsleep(void)
{
    stc_gpio_cfg_t gpiocfg;
    DDL_ZERO_STRUCT(gpiocfg);
  
    App_LowPowerModeGpioSet();
    gpiocfg.enDir = GpioDirIn;
    gpiocfg.enPu = GpioPuEnable;
    gpiocfg.enPd = GpioPdDisable;
    Gpio_Init(GpioPortA,GpioPin7,&gpiocfg);
//    Gpio_ClrIO(GpioPortD,GpioPin14);
    Gpio_ClrIO(GpioPortB,GpioPin3);
    Gpio_ClrIO(GpioPortB,GpioPin4);
    Gpio_ClrIO(GpioPortB,GpioPin5);
    Gpio_ClrIO(GpioPortB,GpioPin6);
    Gpio_ClrIO(GpioPortF,GpioPin11);
    Lpm_GotoDeepSleep(TRUE);
}