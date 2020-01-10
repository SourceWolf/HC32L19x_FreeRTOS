#ifndef HD_UART1_H
#define HD_UART1_H
#include "ddl.h"
#include "uart.h"
#include "gpio.h"

#define UART1_RX_PORT    GpioPortD
#define UART1_RX_PIN     GpioPin7
#define UART1_RX_FUN        GpioAf1

#define UART1_TX_PORT    GpioPortD
#define UART1_TX_PIN     GpioPin6
#define UART1_TX_FUN        GpioAf1

#define UART1_UNIT     M0P_UART1

#define UART1_CLK_EN    SysctrlPeripheralUart1 

#define BAUDRATE        9600

#define UART1_IRQ       UART1_3_IRQn


en_result_t Hd_uart1_init(void);
#endif


