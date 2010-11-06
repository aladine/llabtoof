
#include "xparameters.h"
#include "timer-server.h"
#include <sys/intr.h>

#define TMRCTR_DEVICE_ID        XPAR_TMRCTR_1_DEVICE_ID

#define TIMER_COUNTER_1	 0
#define RESET_VALUE	 0xE4E1C0//0x8F0D180

volatile int TimerExpired;


void timer_a_int_handler(void * baseaddr_p)
{
   Xuint32 csr;
	xil_printf("Handling func \r\n");

	csr = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_1_BASEADDR, TIMER_COUNTER_1);
	if (csr) 
	{
		TimerExpired++;
		xil_printf("Controoling timer: %d \r\n", TimerExpired);
		  //controlling_update();
	}
	//xil_printf("TimerExpired is...%d \r\n", TimerExpired);

	XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_1_BASEADDR, TIMER_COUNTER_1,csr);
	
	//disable_interrupt(XPAR_INTC_1_TMRCTR_1_VEC_ID);
}

int Timer_Init(XTmrCtr *myTimer)
{      
   u32 status; 
	int LastTimerExpired = 0;
	//print("Setting up peripherals...\r\n");
       
	//Initialize and configuring the timer
	status = XTmrCtr_Initialize(myTimer, TMRCTR_DEVICE_ID);
	//xil_printf("Up1 %X \r\n",status);

	status= XTmrCtr_SelfTest(myTimer, TIMER_COUNTER_1);
	//xil_printf("Up2 %X \r\n",status);
	XTmrCtr_SetOptions(myTimer,TIMER_COUNTER_1,XTC_INT_MODE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_AUTO_RELOAD_OPTION);

	XTmrCtr_SetHandler(myTimer,(XTmrCtr_Handler)timer_a_int_handler,NULL);
	register_int_handler(XPAR_INTC_1_TMRCTR_1_VEC_ID,(XInterruptHandler)XTmrCtr_InterruptHandler, myTimer);
	enable_interrupt(XPAR_INTC_1_TMRCTR_1_VEC_ID);
}
	
int Timer_Start_Count(XTmrCtr *myTimer)
{
	int LastTimerExpired=0;
	XTmrCtr_SetResetValue(myTimer, TIMER_COUNTER_1,RESET_VALUE );
	XTmrCtr_Start(myTimer, TIMER_COUNTER_1);
	
	Xint32 csr;
	print("Entering loop...\r\n");
	
	while(1)
	{ 
		xil_printf("Loop %d  \r\n",TimerExpired);

		while (TimerExpired == LastTimerExpired)
		{}
		
		LastTimerExpired = TimerExpired;

	}
}

