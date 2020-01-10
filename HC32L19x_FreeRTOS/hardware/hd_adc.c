#include "hd_adc.h"
uint16_t Adc_result;
 ///< ADC中断服务函数
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
    ///< 开启ADC/BGR外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    
    Bgr_BgrEnable();        ///< 开启BGR
    
    ///< ADC 初始化配置
    stcAdcCfg.enAdcMode         = AdcSglMode;              ///<采样模式-扫描
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv1;            ///<采样分频-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle8Clk;      ///<采样周期数-8
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;      ///<参考电压选择-VCC
    stcAdcCfg.enAdcOpBuf        = AdcMskBufEnable;         ///<OP BUF配置-关
    stcAdcCfg.enInRef           = AdcMskInRefDisable;       ///<内部参考电压使能-关
    stcAdcCfg.enAdcAlign        = AlignRight;            ///<转换结果对齐方式-右
    Adc_Init(&stcAdcCfg);  
    
    
    ///< 顺序扫描模式功能及通道配置
    ///< 注意：扫描模式下，当配置转换次数为n时，转换通道的配置范围必须为[SQRCH(0)MUX,SQRCH(n-1)MUX]
//    stcAdcSqrCfg.bSqrDmaTrig = FALSE;
//    stcAdcSqrCfg.enResultAcc = AdcResultAccDisable;
//    stcAdcSqrCfg.u8SqrCnt    = 16;
//    Adc_SqrModeConfig(&stcAdcSqrCfg);
    Adc_ConfigSglChannel(AdcExInputCH15);
//    ///< 配置顺序扫描转换通道
//    Adc_ConfigSqrChannel(SQRCH15MUX, AdcExInputCH15);
    
    ///< 顺序扫描触发端口选择
//    Adc_SqrExtTrigCfg(AdcMskTrigTimer3, TRUE);
    Adc_SglExtTrigCfg(AdcMskTrigTimer3,TRUE);
    
    Adc_EnableIrq();
    EnableNvic(ADC_DAC_IRQn, IrqLevel3, TRUE);
//    
}


