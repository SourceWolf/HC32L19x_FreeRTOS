#include "hd_gpt_timer.h"
#include "hd_adc.h"
void Tim3_IRQHandler(void)
{
    static uint8_t i;
    
    if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
    {
        if(0 == i)
        {
            Gpio_SetIO(EVB_LEDY_PORT, EVB_LEDY_PIN);
            Adc_SGL_Start();
            i++;
        }
        else
        {
            Gpio_ClrIO(EVB_LEDY_PORT, EVB_LEDY_PIN);
            i = 0;
        }       
        Tim3_ClearIntFlag(Tim3UevIrq);
    }
}

void Timer3_config(void)
{
    uint16_t                    u16ArrValue;
    uint16_t                    u16CntValue;
    stc_tim3_mode0_config_t     stcTim3BaseCfg;
    
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO 外设时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Base Timer外设时钟使能
    
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;              //定时器模式
    stcTim3BaseCfg.enCT       = Tim3Timer;                  //定时器功能，计数时钟为内部PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv64;              //PCLK/64
    stcTim3BaseCfg.enCntMode  = Tim316bitArrMode;           //自动重载16位计数器/定时器
    stcTim3BaseCfg.bEnTog     = FALSE;
    stcTim3BaseCfg.bEnGate    = FALSE;
    stcTim3BaseCfg.enGateP    = Tim3GatePositive;
    
    Tim3_Mode0_Init(&stcTim3BaseCfg);                       //TIM3 的模式0功能初始化
        
    u16ArrValue = 0x1000;
    
    Tim3_M0_ARRSet(u16ArrValue);                            //设置重载值(周期 = 0x10000-ARR)
    
    u16CntValue = 0x1000;
    
    Tim3_M0_Cnt16Set(u16CntValue);                          //设置计数初值
    
    Tim3_ClearIntFlag(Tim3UevIrq);                          //清中断标志
    Tim3_Mode0_EnableIrq();                                 //使能TIM3中断(模式0时只有一个中断)
    EnableNvic(TIM3_IRQn, IrqLevel3, TRUE);                 //TIM3 开中断 
//    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, FALSE);
    M0P_SYSCTRL->OVCK_CR_f.OVCK = 1;//使用双倍频
    Tim3_M0_Run();                                          //TIM3 运行。
}

