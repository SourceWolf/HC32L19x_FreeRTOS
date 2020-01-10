#include "hd_pca.h"

void pca_init(void)
{
    stc_gpio_config_t stcGpioCfg;
    stc_pcaconfig_t  PcaInitStruct;
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);
	PcaInitStruct.pca_clksrc = pca_pclkdiv32;
	PcaInitStruct.pca_cidl   = FALSE;
	PcaInitStruct.pca_ecom   = Ecom_Enable;       //����Ƚ�������
	PcaInitStruct.pca_capp   = Capp_Disable;      //��ֹ�����ز���
	PcaInitStruct.pca_capn   = Capn_Disable;      //��ֹ�½��ز���
	PcaInitStruct.pca_mat    = Mat_Enable;        //��ֹƥ�书��
	PcaInitStruct.pca_tog    = Tog_Enable;        //��ֹ��ת���ƹ���
	PcaInitStruct.pca_pwm    = Pwm8bit_Disable;    //ʹ��PWM�������
	PcaInitStruct.pca_epwm   = Epwm_Enable;      //��ֹ16bitPWM���
	PcaInitStruct.pca_ccap   = 500;
	PcaInitStruct.pca_carr   = 1500;              //ռ�ձ�(1500-500)/1500*100%
	
	Pca_M0Init(&PcaInitStruct);
    Pca_M1Init(&PcaInitStruct);
    Pca_M2Init(&PcaInitStruct);
    Pca_M3Init(&PcaInitStruct);
    Pca_M4Init(&PcaInitStruct);
    
    
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



