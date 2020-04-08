#include "lptim.h"
#include "gpio.h"
/**
 ******************************************************************************
 ** \brief  LPTIMER0中断服务函数
 **
 ** \return 无
 ******************************************************************************/
void LpTim0_IRQHandler(void)
{
	static uint8_t status = 0;
    if (TRUE == Lptim_GetItStatus(M0P_LPTIMER0))
    {
        Lptim_ClrItStatus(M0P_LPTIMER0);//清除LPTIMER0的中断标志位
        Lptim_Cmd(M0P_LPTIMER0, FALSE);
				status = !status;
				if(status)
				{
					Gpio_ClrIO(GpioPortD,GpioPin14);
				}
				else
				{
					Gpio_SetIO(GpioPortD,GpioPin14);
				}
    }
}
void lptim_init(void)
{
    stc_lptim_cfg_t    stcLptCfg;    
    DDL_ZERO_STRUCT(stcLptCfg);
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    ///< 使能LPTIM0 外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim0, TRUE);
    
    stcLptCfg.enGate   = LptimGateLow;
    stcLptCfg.enGatep  = LptimGatePLow;
    stcLptCfg.enTcksel = LptimRcl;
    stcLptCfg.enTogen  = LptimTogEnLow;
    stcLptCfg.enCt     = LptimTimerFun;         //计数器功能
    stcLptCfg.enMd     = LptimMode2;            //工作模式为模式2：自动重装载16位计数器/定时器
    stcLptCfg.u16Arr   = 65536-32;                     //预装载寄存器值
    Lptim_Init(M0P_LPTIMER0, &stcLptCfg);
    
    Lptim_ClrItStatus(M0P_LPTIMER0);        //清除中断标志位
    Lptim_ConfIt(M0P_LPTIMER0, TRUE);       //允许LPTIMER中断    
    EnableNvic(LPTIM_0_1_IRQn, IrqLevel3, TRUE); 
    Lptim_Cmd(M0P_LPTIMER0, TRUE); 
}