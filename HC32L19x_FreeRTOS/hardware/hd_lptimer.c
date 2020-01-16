#include "lptim.h"
/**
 ******************************************************************************
 ** \brief  LPTIMER0�жϷ�����
 **
 ** \return ��
 ******************************************************************************/
void LpTim0_IRQHandler(void)
{
    if (TRUE == Lptim_GetItStatus(M0P_LPTIMER0))
    {
        Lptim_ClrItStatus(M0P_LPTIMER0);//���LPTIMER0���жϱ�־λ
        Lptim_Cmd(M0P_LPTIMER0, FALSE);        
    }
}
void lptim_init(void)
{
    stc_lptim_cfg_t    stcLptCfg;    
    DDL_ZERO_STRUCT(stcLptCfg);
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    ///< ʹ��LPTIM0 ����ʱ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim0, TRUE);
    
    stcLptCfg.enGate   = LptimGateLow;
    stcLptCfg.enGatep  = LptimGatePLow;
    stcLptCfg.enTcksel = LptimRcl;
    stcLptCfg.enTogen  = LptimTogEnLow;
    stcLptCfg.enCt     = LptimTimerFun;         //����������
    stcLptCfg.enMd     = LptimMode1;            //����ģʽΪģʽ1�����Զ���װ��16λ������/��ʱ��
    stcLptCfg.u16Arr   = 0;                     //Ԥװ�ؼĴ���ֵ
    Lptim_Init(M0P_LPTIMER0, &stcLptCfg);
    
    Lptim_ClrItStatus(M0P_LPTIMER0);        //����жϱ�־λ
    Lptim_ConfIt(M0P_LPTIMER0, TRUE);       //����LPTIMER�ж�    
    EnableNvic(LPTIM_0_1_IRQn, IrqLevel3, TRUE); 
    Lptim_Cmd(M0P_LPTIMER0, TRUE); 
}