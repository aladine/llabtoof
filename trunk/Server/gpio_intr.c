#include "xparameters.h"
#include "xtmrctr.h"
#include "xintc.h"
#include "xgpio.h"
#include "xil_exception.h"

#define TMRCTR_DEVICE_ID        XPAR_TMRCTR_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
  

#define TIMER_COUNTER_0	 0
#define RESET_VALUE	 0x11E1A300

volatile int TimerExpired;
// Driver instances
static XGpio myGpio;
static XTmrCtr myTimer;
static XIntc myIntc;


// Push Button ISR: function is called upon the depression of ANY of the 4
// buttons. Which button is determined within the ISR.
void pb_int_handler(void *baseaddr_p) {
	Xuint32 dsr;
	XStatus stat;
	char lock;
	
	//DSR contains the information of which button was depressed, so we can switch
	//on its value.
	dsr = XGpio_ReadReg(XPAR_BUTTONS_3BIT_BASEADDR, 2);
	switch(dsr) {
		
		case 0x01:
			xil_printf("Middle: Start the game \r\n");
			break;
			
		case 0x02:
			xil_printf("Right: Enabling Timer interrupts\r\n");
			XIntc_Enable(&myIntc, XPAR_INTC_0_TMRCTR_0_VEC_ID);
			//This line is duplicated in line 152
			
			/*Team B foul*/
			break;
			
		case 0x08:
			xil_printf("Up\r\n");
			/*Start the match*/
			break;
			
		case 0x04:
			xil_printf("Left: Disabling Timer interrupts\r\n");
			XIntc_Disable(&myIntc, XPAR_INTC_0_TMRCTR_0_VEC_ID);
			/*Team A foul*/
			
			
			break;
			
		default : {
		}
					
	}
	//Clear the interrupt both in the Gpio instance as well as the interrupt controller
	XGpio_InterruptClear(&myGpio, 0x3);
	XIntc_Acknowledge(&myIntc,XPAR_BUTTONS_3BIT_INTERRUPT_PRESENT);
}

// Timer ISR: function is called upon the expiration of either of two timers in a 
// single instance of the opb_timer peripheral. For the purposes of this demonstration,
// we are only using timer 0.
void timer_a_int_handler(void *CallBackRef, Xuint8 TmrCtrNumber) {
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;

	/*
	 * Check if the timer counter has expired, checking is not necessary
	 * since that's the reason this function is executed, this just shows
	 * how the callback reference can be used as a pointer to the instance
	 * of the timer counter that expired, increment a shared variable so
	 * the main thread of execution can see the timer expired
	 */
	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
		TimerExpired++;
		if(TimerExpired == 3) {
			XTmrCtr_SetOptions(InstancePtr, TmrCtrNumber, 0);
		}
	}
	XIntc_Acknowledge(&myIntc,XPAR_INTC_0_TMRCTR_0_VEC_ID);
	}
	
// Main: this is the function that is executed at the start of the program.	
int main(){
	
	/*There are three phases of set-up to get interrupts to work:
		1) Device initialization and configuration
		2) Interrupt controller initialization and configuration
		3)  Exception initialization
	*/

	
	//********************** Device initialization and configuration *************************
	int LastTimerExpired = 0;
	xil_printf("Setting up peripherals...\r\n");
	
	//Initialize and configuring the timer
	XTmrCtr_Initialize(&myTimer, TMRCTR_DEVICE_ID);
				xil_printf("Up1\r\n");

	XTmrCtr_SelfTest(&myTimer, TIMER_COUNTER_0);
			xil_printf("Up2\r\n");
	XTmrCtr_SetOptions(&myTimer,(Xuint8)0,XTC_INT_MODE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_AUTO_RELOAD_OPTION);
			xil_printf("Up3\r\n");
	//*************** Interrupt controller initialization and configuration ******************
	xil_printf("Setting up interrupt controller...\r\n");
	XIntc_Initialize(&myIntc, INTC_DEVICE_ID);
	
	XIntc_Connect(&myIntc, XPAR_INTC_0_TMRCTR_0_VEC_ID,
                           (XInterruptHandler)XTmrCtr_InterruptHandler,
                           &myTimer);			
			
	XTmrCtr_SetHandler(&myTimer,(XTmrCtr_Handler)timer_a_int_handler,NULL); 
	//XTmrCtr_SetHandler(&myTimer,(XTmrCtr_Handler)timer_a_int_handler,&myTimer); 
	//modify over here, in the example it is &myTimer
			xil_printf("Up4\r\n");
	
	
	
	
	//Initialize and configure the push buttons
	XGpio_Initialize(&myGpio, XPAR_BUTTONS_3BIT_DEVICE_ID);
					xil_printf("Up5\r\n");

	XGpio_SetDataDirection(&myGpio, 1, 0x3);
				xil_printf("Up6 \r\n");
	XGpio_InterruptEnable(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);
				xil_printf("Up7 \n");
	XGpio_InterruptGlobalEnable(&myGpio);
	//*******************************************************************************************

	
	
		
	
	//Attach the ISRs to the interrupt controller driver.
	//NOTE: The timer is weird. You have to attach "XTmrCtr_InterruptHandler," which is nested
	//deep within the timer driver. It in turn calls the callback you provide during the
	//configuration above.
	XIntc_Connect(&myIntc, XPAR_INTC_0_GPIO_0_VEC_ID,
                           (XInterruptHandler)pb_int_handler,
                           &myGpio);
								
									
	XIntc_Start(&myIntc, XIN_REAL_MODE);
	XIntc_Enable(&myIntc, XPAR_INTC_0_GPIO_0_VEC_ID);
	XIntc_Enable(&myIntc, XPAR_INTC_0_TMRCTR_0_VEC_ID);
	//*******************************************************************************************



	//************************ Exception initialization ******************************

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
					(Xil_ExceptionHandler)
					XIntc_InterruptHandler,
					&myIntc);
	Xil_ExceptionEnable();

	//*******************************************************************************************

	
	//Set the timer to expire every 6 seconds
	XTmrCtr_SetResetValue(&myTimer, (Xuint8)0,RESET_VALUE );//6 * 50000000
	XTmrCtr_Start(&myTimer, (Xuint8)0);
	
	
	xil_printf("Entering loop...\r\n");
	while(1){
	/*
		 * Wait for the first timer counter to expire as indicated by the
		 * shared variable which the handler will increment
		 */
		 			xil_printf("Loop \r\n");

		while (TimerExpired == LastTimerExpired) {
		}
		LastTimerExpired = TimerExpired;
			xil_printf("Timer expired %d \r\n",TimerExpired);


		/*
		 * If it has expired a number of times, then stop the timer counter
		 * and stop this example
		 */
		if (TimerExpired == 3) {

			XTmrCtr_Stop(&myTimer,(Xuint8) 0);
			break;
		}
	}
	
}


