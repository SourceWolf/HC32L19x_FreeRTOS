/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM0 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "ddl.h"
#include "hd_deepsleep.h"
uint16_t Timer0_value;
uint8_t Division,Flag_Sleep;
#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
#endif

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL		( *( ( volatile uint32_t *) 0xe000e010 ))
#define portNVIC_SYSTICK_LOAD		( *( ( volatile uint32_t *) 0xe000e014 ))
#define portNVIC_SYSTICK_CURRENT_VALUE	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_INT_CTRL			( ( volatile uint32_t *) 0xe000ed04 )
#define portNVIC_SYSPRI2			( ( volatile uint32_t *) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK		0x00000004
#define portNVIC_SYSTICK_INT		0x00000002
#define portNVIC_SYSTICK_ENABLE		0x00000001
#define portNVIC_SYSTICK_COUNT_FLAG		( 1UL << 16UL )
#define portNVIC_PENDSVSET			0x10000000
#define portMIN_INTERRUPT_PRIORITY	( 255UL )
#define portNVIC_PENDSV_PRI			( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI		( portMIN_INTERRUPT_PRIORITY << 24UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR			( 0x01000000 )

/* Constants used with memory barrier intrinsics. */
#define portSY_FULL_READ_WRITE		( 15 )

/* The systick is a 24-bit counter. */
#define portMAX_24_BIT_NUMBER				( 0xffffffUL )

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR			( 45UL )

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*
 * Setup the timer to generate the tick interrupts.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvPortStartFirstTask( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * The number of SysTick increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 1
	static unsigned long ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE == 1
	static unsigned long xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if configUSE_TICKLESS_IDLE == 1
	static unsigned long ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
	*pxTopOfStack = portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pxCode;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError;	/* LR */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = ( StackType_t ) pvParameters;	/* R0 */
	pxTopOfStack -= 8; /* R11..R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vPortSVCHandler( void )
{
	/* This function is no longer used, but retained for backward
	compatibility. */
}
/*-----------------------------------------------------------*/

__asm void prvPortStartFirstTask( void )
{
	extern pxCurrentTCB;

	PRESERVE8

	/* The MSP stack is not reset as, unlike on M3/4 parts, there is no vector
	table offset register that can be used to locate the initial stack value.
	Not all M0 parts have the application vector table at address 0. */

	ldr	r3, =pxCurrentTCB	/* Obtain location of pxCurrentTCB. */
	ldr r1, [r3]
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	adds r0, #32			/* Discard everything up to r0. */
	msr psp, r0				/* This is now the new top of stack to use in the task. */
	movs r0, #2				/* Switch to the psp stack. */
	msr CONTROL, r0
	isb
	pop {r0-r5}				/* Pop the registers that are saved automatically. */
	mov lr, r5				/* lr is now in r5. */
	pop {r3}				/* The return address is now in r3. */
	pop {r2}				/* Pop and discard the XPSR. */
	cpsie i					/* The first task has its context and interrupts can be enabled. */
	bx r3					/* Finally, jump to the user defined task code. */

	ALIGN
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	vPortSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	prvPortStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
	/* Set a PendSV to request a context switch. */
	*( portNVIC_INT_CTRL ) = portNVIC_PENDSVSET;

	/* Barriers are normally not required but do ensure the code is completely
	within the specified behaviour for the architecture. */
	__dsb( portSY_FULL_READ_WRITE );
	__isb( portSY_FULL_READ_WRITE );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
	__dsb( portSY_FULL_READ_WRITE );
	__isb( portSY_FULL_READ_WRITE );
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

__asm uint32_t ulSetInterruptMaskFromISR( void )
{
	mrs r0, PRIMASK
	cpsid i
	bx lr
}
/*-----------------------------------------------------------*/

__asm void vClearInterruptMaskFromISR( uint32_t ulMask )
{
	msr PRIMASK, r0
	bx lr
}
/*-----------------------------------------------------------*/

__asm void xPortPendSVHandler( void )
{
	extern vTaskSwitchContext
	extern pxCurrentTCB

	PRESERVE8

	mrs r0, psp

	ldr	r3, =pxCurrentTCB 	/* Get the location of the current TCB. */
	ldr	r2, [r3]

	subs r0, #32			/* Make space for the remaining low registers. */
	str r0, [r2]			/* Save the new top of stack. */
	stmia r0!, {r4-r7}		/* Store the low registers that are not saved automatically. */
	mov r4, r8				/* Store the high registers. */
	mov r5, r9
	mov r6, r10
	mov r7, r11
	stmia r0!, {r4-r7}

	push {r3, r14}
	cpsid i
	bl vTaskSwitchContext
	cpsie i
	pop {r2, r3}			/* lr goes in r3. r2 now holds tcb pointer. */

	ldr r1, [r2]
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	adds r0, #16			/* Move to the high registers. */
	ldmia r0!, {r4-r7}		/* Pop the high registers. */
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r11, r7

	msr psp, r0				/* Remember the new top of stack for the task. */

	subs r0, #32			/* Go back for the low registers that are not automatically restored. */
	ldmia r0!, {r4-r7}      /* Pop low registers.  */

	bx r3
	ALIGN
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
uint32_t ulPreviousMask;

	ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* Pend a context switch. */
			*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );
}
/*-----------------------------------------------------------*/
void HC32F19x_freertos_wakeup(TickType_t xExpectedIdleTime);
#if configUSE_TICKLESS_IDLE == 1

	__weak void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
	{
	uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulSysTickCTRL;
	TickType_t xModifiableIdleTime;

		/* Make sure the SysTick reload value does not overflow the counter. */
		if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
		{
			xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
		}

		/* Stop the SysTick momentarily.  The time the SysTick is stopped for
		is accounted for as best it can be, but using the tickless mode will
		inevitably result in some tiny drift of the time maintained by the
		kernel with respect to calendar time. */
		portNVIC_SYSTICK_CTRL &= ~portNVIC_SYSTICK_ENABLE;

		/* Calculate the reload value required to wait xExpectedIdleTime
		tick periods.  -1 is used because this code will execute part way
		through one of the tick periods. */
		ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
		if( ulReloadValue > ulStoppedTimerCompensation )
		{
			ulReloadValue -= ulStoppedTimerCompensation;
		}

		/* Enter a critical section but don't use the taskENTER_CRITICAL()
		method as that will mask interrupts that should exit sleep mode. */
		__disable_irq();

		/* If a context switch is pending or a task is waiting for the scheduler
		to be unsuspended then abandon the low power entry. */
		if( eTaskConfirmSleepModeStatus() == eAbortSleep )
		{
			/* Restart from whatever is left in the count register to complete
			this tick period. */
			portNVIC_SYSTICK_LOAD = portNVIC_SYSTICK_CURRENT_VALUE;

			/* Restart SysTick. */
			portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;

			/* Reset the reload register to the value required for normal tick
			periods. */
			portNVIC_SYSTICK_LOAD = ulTimerCountsForOneTick - 1UL;

			/* Re-enable interrupts - see comments above __disable_irq() call
			above. */
			__enable_irq();
		}
		else
		{
			/* Set the new reload value. */
			portNVIC_SYSTICK_LOAD = ulReloadValue;

			/* Clear the SysTick count flag and set the count value back to
			zero. */
			portNVIC_SYSTICK_CURRENT_VALUE = 0UL;

			/* Restart SysTick. */
			portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;

			/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
			set its parameter to 0 to indicate that its implementation contains
			its own wait for interrupt or wait for event instruction, and so wfi
			should not be executed again.  However, the original expected idle
			time variable must remain unmodified, so a copy is taken. */
			xModifiableIdleTime = xExpectedIdleTime;
			configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
			if( xModifiableIdleTime > 0 )
			{
				__dsb( portSY_FULL_READ_WRITE );
//				__wfi();
				__isb( portSY_FULL_READ_WRITE );
			}
			configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

			/* Stop SysTick.  Again, the time the SysTick is stopped for is
			accounted for as best it can be, but using the tickless mode will
			inevitably result in some tiny drift of the time maintained by the
			kernel with respect to calendar time. */
			ulSysTickCTRL = portNVIC_SYSTICK_CTRL;
			portNVIC_SYSTICK_CTRL = ( ulSysTickCTRL & ~portNVIC_SYSTICK_ENABLE );

			/* Re-enable interrupts - see comments above __disable_irq() call
			above. */
			__enable_irq();

			if( ( ulSysTickCTRL & portNVIC_SYSTICK_COUNT_FLAG ) != 0 )
			{
				uint32_t ulCalculatedLoadValue;

				/* The tick interrupt has already executed, and the SysTick
				count reloaded with ulReloadValue.  Reset the
				portNVIC_SYSTICK_LOAD with whatever remains of this tick
				period. */
				ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE );

				/* Don't allow a tiny value, or values that have somehow
				underflowed because the post sleep hook did something
				that took too long. */
				if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
				{
					ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
				}

				portNVIC_SYSTICK_LOAD = ulCalculatedLoadValue;

				/* The tick interrupt handler will already have pended the tick
				processing in the kernel.  As the pending tick will be
				processed as soon as this function exits, the tick value
				maintained by the tick is stepped forward by one less than the
				time spent waiting. */
				ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
			}
			else
			{
				/* Something other than the tick interrupt ended the sleep.
				Work out how long the sleep lasted rounded to complete tick
				periods (not the ulReload value which accounted for part
				ticks). */
				ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - portNVIC_SYSTICK_CURRENT_VALUE;

				/* How many complete tick periods passed while the processor
				was waiting? */
				ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

				/* The reload value is set to whatever fraction of a single tick
				period remains. */
				portNVIC_SYSTICK_LOAD = ( ( ulCompleteTickPeriods + 1 ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
			}

			/* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD
			again, then set portNVIC_SYSTICK_LOAD back to its standard
			value.  The critical section is used to ensure the tick interrupt
			can only execute once in the case that the reload register is near
			zero. */
			portNVIC_SYSTICK_CURRENT_VALUE = 0UL;
			portENTER_CRITICAL();
			{
				portNVIC_SYSTICK_CTRL |= portNVIC_SYSTICK_ENABLE;
				vTaskStepTick( ulCompleteTickPeriods );
				portNVIC_SYSTICK_LOAD = ulTimerCountsForOneTick - 1UL;
			}
			portEXIT_CRITICAL();
		}
	}

#endif /* #if configUSE_TICKLESS_IDLE */

/*-----------------------------------------------------------*/

/*
 * Setup the SysTick timer to generate the tick interrupts at the required
 * frequency.
 */
#if configOVERRIDE_DEFAULT_TICK_CONFIGURATION == 0

	void vPortSetupTimerInterrupt( void )
	{
			/* Calculate the constants required to configure the tick interrupt. */
			#if configUSE_TICKLESS_IDLE == 1
			{
				ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
				xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
				ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
			}
			#endif /* configUSE_TICKLESS_IDLE */
		
		/* Configure SysTick to interrupt at the requested rate. */
		portNVIC_SYSTICK_LOAD = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
		portNVIC_SYSTICK_CTRL = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
		}
#endif /* configOVERRIDE_DEFAULT_TICK_CONFIGURATION */
/*-----------------------------------------------------------*/
#if configUSE_TICKLESS_IDLE == 1
void SetSleepTime(uint32_t ms)
{
    if(ms>500000)
    {
        return;//参数错误
    }
    uint32_t temp = 0,Div_temp = 0;
    temp = (ms*32768)/1000;
    while(temp > 32768)
    {
        temp = temp/2;
        Div_temp++;
				if(Div_temp == 7)
				{
					temp = temp/2;//
				}
    }
    Division = Div_temp;
    Timer0_value = temp;
    if(Division > 7)
    {
				Division = 7;
        return;//参数错误
    }
//    Flag_Sleep = 1;//使能睡眠
}
TickType_t HC32F19x_freertos_sleep(TickType_t xExpectedIdleTime)
{
    SetSleepTime(xExpectedIdleTime);
    //添加睡眠设置
    M0P_LPTIMER0->CNT_f.CNT = 0;
    M0P_LPTIMER0->ARR_f.ARR = 65536-(Timer0_value);
    M0P_LPTIMER0->CR_f.PRS = Division;
    M0P_LPTIMER0->CR_f.TR = 1;//启动定时器
    System_EnterDeepsleep();
//        M4_TMR01->CMPAR_f.CMPA = Timer0_value;//设置周期
//        M4_TMR01->CNTAR_f.CNTA = 0;//定时器清零
//        M4_TMR01->BCONR_f.CKDIVA = Division;//分频数
//        TIMER0_Cmd(M4_TMR01, Tim0_ChannelA, Enable);//启动Timer定时。
//        System_Enter_StopMode();       
//    return 0;
}
void HC32F19x_freertos_wakeup(TickType_t xExpectedIdleTime)
{
    //恢复RTC
//    SysTick_Config(48000);//48MHZ
    return;
}
#endif /* configASSERT_DEFINED */
