

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


#define XGPIO_CHAN_OFFSET  8

#define XGPIO_DATA_OFFSET	0x0   < Data register for 1st channel
#define XGPIO_TRI_OFFSET	0x4   < I/O direction reg for 1st channel
#define XGPIO_DATA2_OFFSET	0x8   < Data register for 2nd channel
#define XGPIO_TRI2_OFFSET	0xC   < I/O direction reg for 2nd channel

#define XGPIO_GIE_OFFSET	0x11C < Glogal interrupt enable register
#define XGPIO_ISR_OFFSET	0x120 < Interrupt status register
#define XGPIO_IER_OFFSET	0x128 < Interrupt enable register

 Definitions for Fabric interrupts connected to ps7_scugic_0
#define XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR 61U

*/



#include <stdio.h>
//#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xgpiops.h" //PS GPIO
#include "xscugic.h" //Interrupt Controller
#include "xil_exception.h" //exeption function for Cortex-	A9



/* Global variable declarations */

XGpio  LED_pointer; /* The Instance of the GPIO LED */
XGpio_Config  Config_LED;

XGpio  B_SW_pointer; /* The Instance of the GPIO Button and Switches */
XGpio_Config  Config_B_SW;

XScuGic INTCInst; /*The instance of the Interrupt Controller*/
XScuGic_Config *IINTCInstConfig;

#define INTC_GPIO_INTERRUPT_ID    XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR /* 61U */
#define INTC_DEVICE_ID	   XPAR_SCUGIC_SINGLE_DEVICE_ID /* 0U form xparameters_ps.h */


/* Function declarations */

s32 Setup_INTC();
void GPIO_Intr_Handler(void *data);

int main()
{

    print("Hello Mario-World\n\r");



    /*Configure LED*/
    int res = XGpio_Initialize(&LED_pointer,XPAR_GPIO_0_DEVICE_ID);
    if(res==XST_SUCCESS)
    	printf("OK\n");
    else
    	printf("ERROR XGpio_CfgInitialize\n");

    XGpio_SetDataDirection(&LED_pointer, 1,0);
    XGpio_DiscreteWrite(&LED_pointer, 1,6);


    /* configure buttons and switches */
    res = XGpio_Initialize(&B_SW_pointer,XPAR_GPIO_1_DEVICE_ID);
    if(res==XST_SUCCESS)
      	printf("OK\n");
    else
       	printf("ERROR XGpio_CfgInitialize\n");
    XGpio_SetDataDirection(&B_SW_pointer, 1,0x0F);


    Setup_INTC();


	// Enable GPIO BTN interrupts. NO SWITCHes
	XGpio_InterruptEnable(&B_SW_pointer, XGPIO_IR_CH1_MASK);/* only the second channel */
	XGpio_InterruptGlobalEnable(&B_SW_pointer);
	/************************************************/

    while(1){ /*wait forever*/ };

    printf("TEST DONE\n");
    return 0;
}



/* This function
 *
 * 1. initialize the Zynq SoC’s exceptions;
 * 2. configure and initialize the GIC;
 * 3. connect the GIC to the interrupt-handling hardware.
 *
 *  */

s32 Setup_INTC(){

	s32 status = XST_SUCCESS;

	/*  Interrupt controller initialization */
	//GIC config
	Xil_ExceptionInit();

	//initialize the GIC
	IINTCInstConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);

	status = XScuGic_CfgInitialize(&INTCInst,
			              IINTCInstConfig,
						  IINTCInstConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) printf("ERROR Interrupt controller initialization \n");


	/* Register GIC interrupt handler */
	//connect to the hardware // Register GIC interrupt handler
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			                    (Xil_ExceptionHandler)XScuGic_InterruptHandler,
								&INTCInst);
	Xil_ExceptionEnable();


	//**********************************************************

	// ********* Connect GPIO to interrupt handler ***************
	status = XScuGic_Connect(&INTCInst,
							 XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR,
					  	  	 (Xil_ExceptionHandler)GPIO_Intr_Handler,
							 (void *)&B_SW_pointer);

	//Configure Rising Edge
	XScuGic_SetPriorityTriggerType(&INTCInst,
			                       XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR,
						           XGPIOPS_IRQ_TYPE_EDGE_RISING,
								   XSCUGIC_INT_CFG_MASK);

	// Enable GPIO interrupts in the controller
	XScuGic_Enable(&INTCInst, XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR);
	//**********************************************************


	return status;

}


/*****************************************************************************
*
* Interrupt Handler for the GPIO (A led is commuted every time this function
* is launched)
*
******************************************************************************/
void GPIO_Intr_Handler(void *data)
{

	// Disable GPIO interrupts
	// first channel btn and second channel switches
	XGpio_InterruptDisable(&B_SW_pointer, XGPIO_IR_CH1_MASK);

	xil_printf(" A button was pushed\n");// %x \n", (unsigned int)  BTN_VAL);


	// Acknowledge GPIO interrupts
	XGpio_InterruptClear(&B_SW_pointer, XGPIO_IR_CH1_MASK);

	//Enable the interrupts again.
	XGpio_InterruptEnable(&B_SW_pointer, XGPIO_IR_CH1_MASK);
	XGpio_InterruptGlobalEnable(&B_SW_pointer);

}
