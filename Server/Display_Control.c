#include "xio.h"
#include "xtft.h"
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include "xmbox.h"
#include "display.h"
#include "timer.h"
#include "controlling.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "xil_exception.h"

/*TIMER*/
#define TMRCTR_DEVICE_ID        XPAR_TMRCTR_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define TIMER_COUNTER_0	 0

/*MAILBOX*/
#define PCK_SIZE         16

/*TIMER*/
//static XGpio myGpio;
static XTmrCtr myTimer;
static XIntc myIntc;

/*MAILBOX*/
XMbox xmbox;
XMbox_Config *xmbox_ConfigPtr;
int rcvmsg[PCK_SIZE];
int rcvd;

/*GAME STATE*/
GameState local_gs;
GameState prev_gs;

void update_MailBox() {
  int ret,i,j;
  while(1) 
  {    
  
	 print("-- Display: Start Mailbox --\r\n");
	
    /* * Mailbox receiving */
		int nbytes = 0;		
		while (nbytes < PCK_SIZE) 
		{
		  (void)XMbox_Read(&xmbox, (u32*)(rcvmsg + nbytes), PCK_SIZE - nbytes, &rcvd);
		  nbytes += rcvd;
		}
		ret = XMbox_IsEmpty(&xmbox); 
		if (ret == 0) print("asdf");
		//prev_gs=local_gs;
		local_gs.ball.x_pos=rcvmsg[0];
		local_gs.ball.y_pos=rcvmsg[1];
	for (i = 0; i < 2; i++)
	      for (j = 0; j < 5; j++)
  		{
			local_gs.players[j][i].x_pos=rcvmsg[10*i+2*j+2];
			local_gs.players[j][i].y_pos=rcvmsg[10*i+2*j+3];
 		}
		
		local_gs.special=rcvmsg[22];
		print("Display: Received from Processor 1 ");
	 
		
	 /* end of mailbox */
	 
  }
  
}

volatile int TimerExpired;

void timer_handler(void * baseaddr_p) {
    Xuint32 csr;
    csr = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, 0);
    if (csr) 
    {
        TimerExpired++;
		  
		  count_down(&TftInstance,TimerExpired);
		 // tft_clear(&TftInstance,prev_gs);
	    // tft_displaycontrol(&TftInstance,local_gs);
	     print("-- Update successful --\r\n");
    }
	 XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, 0, csr);
}
	
int TimerSetup(XIntc* myIntc,
				XTmrCtr* myTimer,u16 DeviceId,u16 IntrId,u8 TmrCtrNumber)

{	
	int LastTimerExpired=0;
	//Initialize and configuring the timer
	XTmrCtr_Initialize(myTimer, DeviceId);
	XTmrCtr_SelfTest(myTimer, TmrCtrNumber);
	XTmrCtr_SetOptions(myTimer,TmrCtrNumber,XTC_INT_MODE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_AUTO_RELOAD_OPTION);
	
	//*************** Interrupt controller initialization and configuration ******************
	xil_printf("Setting up interrupt controller...\r\n");
	XIntc_Initialize(myIntc, IntrId);
	XIntc_Connect(myIntc, IntrId,(XInterruptHandler)timer_handler,
                           myTimer);					
	XTmrCtr_SetHandler(myTimer,(XTmrCtr_Handler)timer_handler,NULL); 

	/*Initialize and configure the push buttons
	XGpio_Initialize(&myGpio, XPAR_BUTTONS_3BIT_DEVICE_ID);

	XGpio_SetDataDirection(&myGpio, 1, 0x3);
	XGpio_InterruptEnable(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);
	XGpio_InterruptGlobalEnable(&myGpio);

	XIntc_Connect(&myIntc, XPAR_INTC_0_GPIO_0_VEC_ID,
                           (XInterruptHandler)pb_int_handler,
                           &myGpio);
								
	*/								
	XIntc_Start(myIntc, XIN_REAL_MODE);
	//XIntc_Enable(&myIntc, XPAR_INTC_0_GPIO_0_VEC_ID);
	XIntc_Enable(myIntc, IntrId);


	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
					(Xil_ExceptionHandler)
					XIntc_InterruptHandler,
					myIntc);
	Xil_ExceptionEnable();


	
	XTmrCtr_SetResetValue(myTimer,TmrCtrNumber,RESET_VALUE );
	XTmrCtr_Start(myTimer, TmrCtrNumber);
	
	
	xil_printf("Entering loop...\r\n");
	while(1){
	
		 			xil_printf("Loop %d  \r\n",TimerExpired);
           //count_down(&TftInstance,TimerExpired);
		while (TimerExpired == LastTimerExpired) {
		}
		LastTimerExpired = TimerExpired;
	}
}


int main (void) 
{
  int ret; 
  print("-- Entering main() --\r\n");
  

  /* * Init of XTFT Device */
  XStatus Status;
  XTft_Config *TftConfigPtr;
  
  /*
	* Get address of the XTft_Config structure for the given device id.
	*/
  print("\r Start Init of TFT \r\n");
  TftConfigPtr = XTft_LookupConfig(XPAR_XPS_TFT_0_DEVICE_ID);

  if (TftConfigPtr == (XTft_Config *)NULL) {
    print("\r Failed 1 \r\n");
	 return XST_FAILURE;
  }

  /*
	* Initialize all the TftInstance members and fills the screen with
	* default background color.
	*/
  print("\r Start to init \r\n");
  Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr, TftConfigPtr->BaseAddress);
  print("\r Init Success \r\n");
  if (Status != XST_SUCCESS) {
      print("\r Failed 2 \r\n");
		return XST_FAILURE;
  }
  /* * End of Init XTFT Device */
  
 Init(&TftInstance);
 display_score(&TftInstance, local_gs.special);
 //count_down 

  /* * Init of XMailbox */
  xmbox_ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
  if (xmbox_ConfigPtr == (XMbox_Config *)NULL){
		print ("\tLookupConfig Failed\r\n");
		return XST_FAILURE;
	}
	
	Status = XMbox_CfgInitialize(&xmbox, xmbox_ConfigPtr, xmbox_ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS){
		print ("\tXMbox_CfgInitialize Failed.\r\n");
		return Status;
	}
   /* * End of init XMbox */
	
	ret= TimerSetup(&myIntc,
				&myTimer,TMRCTR_DEVICE_ID,INTC_DEVICE_ID,TIMER_COUNTER_0);
	
	//XMbox_Flush(&xmbox); 
   update_MailBox();



}
