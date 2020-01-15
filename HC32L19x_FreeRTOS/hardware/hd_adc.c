#include "hd_adc.h"
uint16_t Adc_result;
 ///< ADC�жϷ�����
void Adc_IRQHandler(void)
{    
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqSgl))
    {
        Adc_ClrIrqStatus(AdcMskIrqSgl);
        Adc_result = Adc_GetSglResult();
    }
}
void Hd_ADC_init(void)
{        
    stc_adc_cfg_t              stcAdcCfg;
//    stc_adc_sqr_cfg_t          stcAdcSqrCfg;
    
    Gpio_SetAnalogMode(GpioPortC, GpioPin5);        //PC05 (AIN15)
    ///< ����ADC/BGR����ʱ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< ����BGR
    
    ///< ADC ��ʼ������
    stcAdcCfg.enAdcMode         = AdcSglMode;              ///<����ģʽ-ɨ��
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv1;            ///<������Ƶ-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle8Clk;      ///<����������-8
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;      ///<�ο���ѹѡ��-VCC
    stcAdcCfg.enAdcOpBuf        = AdcMskBufEnable;         ///<OP BUF����-��
    stcAdcCfg.enInRef           = AdcMskInRefDisable;       ///<�ڲ��ο���ѹʹ��-��
    stcAdcCfg.enAdcAlign        = AdcAlignRight;            ///<ת��������뷽ʽ-��
    Adc_Init(&stcAdcCfg);  
    
    
    ///< ˳��ɨ��ģʽ���ܼ�ͨ������
    ///< ע�⣺ɨ��ģʽ�£�������ת������Ϊnʱ��ת��ͨ�������÷�Χ����Ϊ[SQRCH(0)MUX,SQRCH(n-1)MUX]
//    stcAdcSqrCfg.bSqrDmaTrig = FALSE;
//    stcAdcSqrCfg.enResultAcc = AdcResultAccDisable;
//    stcAdcSqrCfg.u8SqrCnt    = 16;
//    Adc_SqrModeConfig(&stcAdcSqrCfg);
    Adc_CfgSglChannel(AdcExInputCH15);
//    ///< ����˳��ɨ��ת��ͨ��
//    Adc_ConfigSqrChannel(SQRCH15MUX, AdcExInputCH15);
    
    ///< ˳��ɨ�败���˿�ѡ��
//    Adc_SqrExtTrigCfg(AdcMskTrigTimer3, TRUE);
    Adc_SglExtTrigCfg(AdcMskTrigTimer3,TRUE);
    
    Adc_EnableIrq();
    EnableNvic(ADC_DAC_IRQn, IrqLevel3, TRUE);
//    
}


