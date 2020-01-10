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
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ����ʱ��ʹ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Base Timer����ʱ��ʹ��
    
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;              //��ʱ��ģʽ
    stcTim3BaseCfg.enCT       = Tim3Timer;                  //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv64;              //PCLK/64
    stcTim3BaseCfg.enCntMode  = Tim316bitArrMode;           //�Զ�����16λ������/��ʱ��
    stcTim3BaseCfg.bEnTog     = FALSE;
    stcTim3BaseCfg.bEnGate    = FALSE;
    stcTim3BaseCfg.enGateP    = Tim3GatePositive;
    
    Tim3_Mode0_Init(&stcTim3BaseCfg);                       //TIM3 ��ģʽ0���ܳ�ʼ��
        
    u16ArrValue = 0x1000;
    
    Tim3_M0_ARRSet(u16ArrValue);                            //��������ֵ(���� = 0x10000-ARR)
    
    u16CntValue = 0x1000;
    
    Tim3_M0_Cnt16Set(u16CntValue);                          //���ü�����ֵ
    
    Tim3_ClearIntFlag(Tim3UevIrq);                          //���жϱ�־
    Tim3_Mode0_EnableIrq();                                 //ʹ��TIM3�ж�(ģʽ0ʱֻ��һ���ж�)
    EnableNvic(TIM3_IRQn, IrqLevel3, TRUE);                 //TIM3 ���ж� 
//    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, FALSE);
    M0P_SYSCTRL->OVCK_CR_f.OVCK = 1;//ʹ��˫��Ƶ
    Tim3_M0_Run();                                          //TIM3 ���С�
}

