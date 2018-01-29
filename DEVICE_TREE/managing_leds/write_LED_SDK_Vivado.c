

/*
 * Leonardo Suriano
 */

/* Definitions for driver GPIO
#define XPAR_XGPIO_NUM_INSTANCES 2

 Definitions for peripheral AXI_GPIO_0
#define XPAR_AXI_GPIO_0_BASEADDR 0x41200000
#define XPAR_AXI_GPIO_0_HIGHADDR 0x4120FFFF
#define XPAR_AXI_GPIO_0_DEVICE_ID 0
#define XPAR_AXI_GPIO_0_INTERRUPT_PRESENT 0
#define XPAR_AXI_GPIO_0_IS_DUAL 1


 Definitions for peripheral AXI_GPIO_1
#define XPAR_AXI_GPIO_1_BASEADDR 0x41210000
#define XPAR_AXI_GPIO_1_HIGHADDR 0x4121FFFF
#define XPAR_AXI_GPIO_1_DEVICE_ID 1
#define XPAR_AXI_GPIO_1_INTERRUPT_PRESENT 1
#define XPAR_AXI_GPIO_1_IS_DUAL 1
*/



#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"

XGpio  LED_pointer;
XGpio_Config  Config_LED;


int main()
{
    init_platform();

    print("Hello Mario-World\n\r");

    int res= XGpio_CfgInitialize(&LED_pointer, &Config_LED,XPAR_AXI_GPIO_0_BASEADDR);
    if(res==XST_SUCCESS)
    	printf("OK\n");
    else
    	printf("ERROR XGpio_CfgInitialize\n");

    XGpio_SetDataDirection(&LED_pointer, 1,0);
    XGpio_DiscreteWrite(&LED_pointer, 1,6);

    cleanup_platform();
    return 0;
}
