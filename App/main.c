#include <stdio.h>
#include "Periphery.h"

int main(void)
{
    /***********************System Init***********************/
    SystemInit();//96M
    systick_config();

    /***********************Driver Init***********************/
    USART0_Init(115200);
    Key_Init();
    SPI_Init();
    LED_Init();
    USBD_Init();

    /***********************Periphery Init***********************/
    //PSRAM_Init();

    /***********************App/OS Init***********************/
    printf("Application Runing!\n");

    /***********************main loop***********************/
    while (1)
    {
        USBD_Kernel();
    }
}



