#include "hd_pca.h"

void pca_init(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    stc_pcacfg_t  PcaInitStruct;
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);
	PcaInitStruct.pca_clksrc = PcaPclkdiv32;
	PcaInitStruct.pca_cidl   = FALSE;
	PcaInitStruct.pca_ecom   = PcaEcomEnable;       //����Ƚ�������
	PcaInitStruct.pca_capp   = PcaCappDisable;      //��ֹ�����ز���
	PcaInitStruct.pca_capn   = PcaCapnDisable;      //��ֹ�½��ز���
	PcaInitStruct.pca_mat    = PcaMatEnable;        //��ֹƥ�书��
	PcaInitStruct.pca_tog    = PcaTogEnable;        //��ֹ��ת���ƹ���
	PcaInitStruct.pca_pwm    = PcaPwm8bitDisable;    //ʹ��PWM�������
	PcaInitStruct.pca_epwm   = PcaEpwmEnable;      //��ֹ16bitPWM���
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



