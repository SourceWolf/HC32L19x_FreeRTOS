#include "app_task.h"

TaskHandle_t Hd_Test_Task;
void Task_Test(void *param)
{    
//    Port_init();
//    Timer3_config();
//    pca_init();
//    Hd_uart1_init();
//    Hd_ADC_init();
//    Uart_SendDataIt(UART1_UNIT,0x55);
    while(1)
    {
         ///< LED����
//        Gpio_SetIO(EVB_LEDR_PORT, EVB_LEDR_PIN);
//        Gpio_SetIO(EVB_LEDY_PORT, EVB_LEDY_PIN);
//        Gpio_SetIO(EVB_LEDG_PORT, EVB_LEDG_PIN);
//        Gpio_SetIO(EVB_LEDB_PORT, EVB_LEDB_PIN);

        vTaskDelay(1000/portTICK_PERIOD_MS);

        ///< LED�ر�
//        Gpio_ClrIO(EVB_LEDR_PORT, EVB_LEDR_PIN);
//        Gpio_ClrIO(EVB_LEDY_PORT, EVB_LEDY_PIN);
//        Gpio_ClrIO(EVB_LEDG_PORT, EVB_LEDG_PIN);
//        Gpio_ClrIO(EVB_LEDB_PORT, EVB_LEDB_PIN);

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
void os_task_init(void)
{
    xTaskCreate(Task_Test,(const char *)"Task_Test", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Test_Task);
    vTaskStartScheduler();
}




