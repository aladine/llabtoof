#include "xmk.h"
#include "xio.h"
#include "xtft.h"
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/timer.h>
#include "xmbox.h"
#include "xtmrctr.h"
#include "xtmrctr.h"
#include "xintc.h"
#include "xil_exception.h"
#include "display.h"
#include "xintc.h"
//#define Local_MutexNumber 0
#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID

#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID	XPAR_INTC_0_TMRCTR_0_VEC_ID

#define TIMER_COUNTER_0	 0
#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define PCK_SIZE         16
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define YELLOW 0x000000ff

#define BLUE 0x000000ff
#define WHITE 0x00ffffff
#define BLACK 0x00000000
#define TIMER_CNTR_0 0

static XTft TftInstance;


XMbox xmbox;
XMbox_Config *xmbox_ConfigPtr;

/************************** Variable Definitions *****************************/
XIntc InterruptController;  /* The instance of the Interrupt Controller */
XIntc *IntcInstancePtr;
XTmrCtr TimerCounter;   /* The instance of the Timer Counter */

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */
volatile int TimerExpired;


int rcvmsg[PCK_SIZE];
int rcvd;
int game_state;
struct ball_t{
  
  int x,y;
};

//mailbox 
 struct pos_t {
	int x,y;
 };
 
 struct team_t{
	struct pos_t pos[5] ;
 };

volatile int taskrunning;





















/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/
#ifndef TESTAPP_GEN
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID	XPAR_INTC_0_TMRCTR_0_VEC_ID

/*
 * The following constant determines which timer counter of the device that is
 * used for this example, there are currently 2 timer counters in a device
 * and this example uses the first one, 0, the timer numbers are 0 based
 */
#define TIMER_CNTR_0	 0

#endif

#define RESET_VALUE	 0xF0000000


static int TmrCtrSetupIntrSystem(XIntc* IntcInstancePtr,
				XTmrCtr* InstancePtr,
				u16 DeviceId,
				u16 IntrId,
				u8 TmrCtrNumber);

void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);

void TmrCtrDisableIntr(XIntc* IntcInstancePtr, u16 IntrId);

/************************** Variable Definitions *****************************/
XIntc InterruptController;  /* The instance of the Interrupt Controller */

XTmrCtr TimerCounterInst;   /* The instance of the Timer Counter */

volatile int TimerExpired;


/*****************************************************************************/
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
   //
   control();

	
	
	//
	
	/*
	 * Check if the timer counter has expired, checking is not necessary
	 * since that's the reason this function is executed, this just shows
	 * how the callback reference can be used as a pointer to the instance
	 * of the timer counter that expired, increment a shared variable so
	 * the main thread of execution can see the timer expired
	 */
	 print("sdffs");
	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
		TimerExpired++;
		if(TimerExpired == 3) {
			XTmrCtr_SetOptions(InstancePtr, TmrCtrNumber, 0);
		}
	}
}

/*****************************************************************************/

static int TmrCtrSetupIntrSystem(XIntc* IntcInstancePtr,
				 XTmrCtr* TmrCtrInstancePtr,
				 u16 DeviceId,
				 u16 IntrId,
				 u8 TmrCtrNumber)
{
	 int Status;

#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that
	 * it's ready to use, specify the device ID that is generated in
	 * xparameters.h
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the specific
	 * interrupt processing for the device
	 */
	Status = XIntc_Connect(IntcInstancePtr, IntrId,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)TmrCtrInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#ifndef TESTAPP_GEN
	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the timer counter can cause interrupts thru the interrupt controller.
	 */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif

	/*
	 * Enable the interrupt for the timer counter
	 */
	XIntc_Enable(IntcInstancePtr, IntrId);

#ifndef TESTAPP_GEN
	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
					(Xil_ExceptionHandler)
					XIntc_InterruptHandler,
					IntcInstancePtr);

	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnable();

#endif
	return XST_SUCCESS;
}


/******************************************************************************/

void TmrCtrDisableIntr(XIntc* IntcInstancePtr, u16 IntrId)
{
	/*
	 * Disable the interrupt for the timer counter
	 */
	XIntc_Disable(IntcInstancePtr, IntrId);

	return;
}



























void tft_display_team(XTft *Tft,struct team_t *team,int col)
{
	int i;
	for (i=0;i++;i<5)
	Draw_Player(Tft,team->pos[i].x,team->pos[i].y,col);
}

void tft_displaycontrol(XTft *Tft,struct team_t *teamA,struct team_t *teamB,struct ball_t ball)
{
	tft_display_team(Tft,teamA,BLUE);
	tft_display_team(Tft,teamB,YELLOW);
	Draw_Ball(Tft,ball.x,ball.y,GREEN);
}

void tft_clear(XTft *Tft,struct team_t *teamA,struct team_t *teamB,struct ball_t ball)
{
	tft_display_team(Tft,teamA,BLACK);
	tft_display_team(Tft,teamB,BLACK);
	Draw_Ball(Tft,ball.x,ball.y,BLACK);
}



void state(int game_state)
{
	

}
void control() {
  struct team_t *teamA,*teamB;
  struct ball_t ball;
  int ret;
  while(1) 
  {    
  
  
	//	Value2 = XTmrCtr_GetValue(TmrCtrInstancePtr, TmrCtrNumber);
	
	tft_clear(&TftInstance,teamA,teamB,ball);
	 print("-- Printed the first Part --\r\n");
	
	 //XTft_ClearScreen(&TftInstance);
	 tft_displaycontrol(&TftInstance,teamA,teamB,ball);
	 sleep(200);
	 print("-- Screen Cleared --\r\n");
	 
    /* * Mailbox receiving */
		int nbytes = 0;		
		while (nbytes < PCK_SIZE) 
		{
		
		  (void)XMbox_Read(&xmbox, (u32*)(rcvmsg + nbytes), PCK_SIZE - nbytes, &rcvd);
		  nbytes += rcvd;
		}
		ret = XMbox_IsEmpty(&xmbox); 
		if (ret == 0) print("asdf");
		
		teamA->pos[0].x = rcvmsg[0];
		teamA->pos[0].y = rcvmsg[1];
		teamA->pos[1].x = rcvmsg[2];
		teamA->pos[1].y = rcvmsg[3];
		teamA->pos[2].x = rcvmsg[4];
		teamA->pos[2].y = rcvmsg[5];
		teamA->pos[3].x = rcvmsg[6];
		teamA->pos[3].y = rcvmsg[7];
		teamA->pos[4].x = rcvmsg[8];
		teamA->pos[4].y = rcvmsg[9];
		
		teamB->pos[0].x = rcvmsg[10];
		teamB->pos[0].y = rcvmsg[11];
		teamB->pos[1].x = rcvmsg[12];
		teamB->pos[1].y = rcvmsg[13];
		teamB->pos[2].x = rcvmsg[14];
		teamB->pos[2].y = rcvmsg[15];
		teamB->pos[3].x = rcvmsg[16];
		teamB->pos[3].y = rcvmsg[17];
		teamB->pos[4].x = rcvmsg[18];
		teamB->pos[4].y = rcvmsg[19];
		ball.x = rcvmsg[20];
		ball.y = rcvmsg[21];
		game_state=rcvmsg[22];

		print("sReceived from Processor 1 ");
		
		xil_printf(" BALL DATA :   X= %d  : Y = %d \n",ball.x,ball.y);
		
	 /* end of mailbox */
	 /* * database: original and new */
	 
	 
	 /* * Check the MailBox */
	 /* * If the MailBox has something , change the local database */
	 
  }
  
}




int main (void) 
{

 int ret;
  
  print("-- Entering main() --\r\n");
  
  print("--Initialized --\r\n");

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
  print("\r start to init \r\n");
  xil_printf("baddr =  %x \r\n",TftConfigPtr->BaseAddress);  
  Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr, TftConfigPtr->BaseAddress);
  print("\r Init Success \r\n");
  if (Status != XST_SUCCESS) {
      print("\r Failed 2 \r\n");
		return XST_FAILURE;
  }
  
  Init(&TftInstance);
  display_score(&TftInstance, 1, 2,  2,  3);
  
  print("\r Success 2 \r\n");
 // XTft_ClearScreen(&TftInstance);
 // print("-- Screen Cleared --\r\n");
  /* * End of Init XTFT Device */
  
  /* * Init of XMailbox */
  xmbox_ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
  if (xmbox_ConfigPtr == (XMbox_Config *)NULL){
		xil_printf ("\tLookupConfig Failed\r\n");
		return XST_FAILURE;
	}
	
	Status = XMbox_CfgInitialize(&xmbox, xmbox_ConfigPtr, xmbox_ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS){
		xil_printf ("\tXMbox_CfgInitialize Failed.\r\n");
		return Status;
	}
   /* * End of init XMbox */
	
	
	
	
	
	/* * Start of init Timer 
	XTmrCtr *TmrCtrInstancePtr = &TimerCounter;

	Status = XTmrCtr_Initialize(TmrCtrInstancePtr, TMRCTR_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XTmrCtr_SelfTest(TmrCtrInstancePtr, TIMER_COUNTER_0);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XTmrCtr_SetOptions(TmrCtrInstancePtr, TIMER_COUNTER_0,
	XTC_AUTO_RELOAD_OPTION);

	int Value1 = XTmrCtr_GetValue(TmrCtrInstancePtr, TIMER_COUNTER_0);
	XTmrCtr_Start(TmrCtrInstancePtr, TIMER_COUNTER_0);
	End of init timer*/
	

	

	/*
	 * Run the Timer Counter - Interrupt example.
	 */
  XTmrCtr *TmrCtrInstancePtr = &TimerCounter;
  XIntc* IntcInstancePtr = &InterruptController;
  print("Timer start");
  int LastTimerExpired = 0;
  print("Timer begin \n");

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(TmrCtrInstancePtr, TMRCTR_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
  print("Timer init \n");

	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly, use the 1st timer in the device (0)
	 */
	Status = XTmrCtr_SelfTest(TmrCtrInstancePtr, TIMER_COUNTER_0);
	/*if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}*/
  print("Timer self test \n");

	/*
	 * Connect the timer counter to the interrupt subsystem such that
	 * interrupts can occur.  This function is application specific.
	 */
	Status = TmrCtrSetupIntrSystem(IntcInstancePtr,
					TmrCtrInstancePtr,
					TMRCTR_DEVICE_ID,
					TMRCTR_INTERRUPT_ID,
					TIMER_COUNTER_0);
	if (Status != XST_SUCCESS) 
	{
		return XST_FAILURE;
	}
  print("Timer set up \n");

	/*
	 * Setup the handler for the timer counter that will be called from the
	 * interrupt context when the timer expires, specify a pointer to the
	 * timer counter driver instance as the callback reference so the handler
	 * is able to access the instance data
	 */
	XTmrCtr_SetHandler(TmrCtrInstancePtr, TimerCounterHandler,
					   TmrCtrInstancePtr);

	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(TmrCtrInstancePtr, TIMER_COUNTER_0,
				XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

	/*
	 * Set a reset value for the timer counter such that it will expire
	 * eariler than letting it roll over from 0, the reset value is loaded
	 * into the timer counter when it is started
	 */
	XTmrCtr_SetResetValue(TmrCtrInstancePtr, TIMER_COUNTER_0, RESET_VALUE);

	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */
	XTmrCtr_Start(TmrCtrInstancePtr, TIMER_COUNTER_0);

	while (1) {
		/*
		 * Wait for the first timer counter to expire as indicated by the
		 * shared variable which the handler will increment
		 */
		   print(" Loop");

		while (TimerExpired == LastTimerExpired) {
		  
		}
		LastTimerExpired = TimerExpired;
  print(" TimerExpired");
		/*
		 * If it has expired a number of times, then stop the timer counter
		 * and stop this example
		 */
		if (TimerExpired == 3) {

			XTmrCtr_Stop(TmrCtrInstancePtr, TIMER_COUNTER_0);
			  print("Timer stop");

			break;
		}
	}

	TmrCtrDisableIntr(IntcInstancePtr, TMRCTR_DEVICE_ID);
	

	return XST_SUCCESS;
	




}
