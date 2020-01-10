#include "hd_pca.h"

void pca_init(void)
{
    stc_gpio_config_t stcGpioCfg;
    stc_pcaconfig_t  PcaInitStruct;
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);
	PcaInitStruct.pca_clksrc = pca_pclkdiv32;
	PcaInitStruct.pca_cidl   = FALSE;
	PcaInitStruct.pca_ecom   = Ecom_Enable;       //允许比较器功能
	PcaInitStruct.pca_capp   = Capp_Disable;      //禁止上升沿捕获
	PcaInitStruct.pca_capn   = Capn_Disable;      //禁止下降沿捕获
	PcaInitStruct.pca_mat    = Mat_Enable;        //禁止匹配功能
	PcaInitStruct.pca_tog    = Tog_Enable;        //禁止翻转控制功能
	PcaInitStruct.pca_pwm    = Pwm8bit_Disable;    //使能PWM控制输出
	PcaInitStruct.pca_epwm   = Epwm_Enable;      //禁止16bitPWM输出
	PcaInitStruct.pca_ccap   = 500;
	PcaInitStruct.pca_carr   = 1500;              //占空比(1500-500)/1500*100%
	
	Pca_M0Init(&PcaInitStruct);
    Pca_M1Init(&PcaInitStruct);
    Pca_M2Init(&PcaInitStruct);
    Pca_M3Init(&PcaInitStruct);
    Pca_M4Init(&PcaInitStruct);
    
    
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
    Gpio_Init(GpioPortE, GpioPin3, &stcGpioCfg);
    Gpio_Init(GpioPortE, GpioPin4, &stcGpioCfg);
    Gpio_Init(GpioPortE, GpioPin5, &stcGpioCfg);
    Gpio_Init(GpioPortE, GpioPin6, &stcGpioCfg);
    Gpio_Init(GpioPortC, GpioPin12, &stcGpioCfg);
    
    Gpio_SetAfMode(GpioPortE, GpioPin3, GpioAf1);
    Gpio_SetAfMode(GpioPortE, GpioPin4, GpioAf1);
    Gpio_SetAfMode(GpioPortE, GpioPin5, GpioAf1);
    Gpio_SetAfMode(GpioPortE, GpioPin6, GpioAf1);
    Gpio_SetAfMode(GpioPortC, GpioPin12, GpioAf3);
        
    Pca_StartPca(TRUE);
}



