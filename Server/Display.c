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
#include "timer.h"
#include "display.h"

//#define Local_MutexNumber 0
#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID
#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID

#define TIMER_COUNTER_0	 0
#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define PCK_SIZE         16
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define BLUE 0x000000ff
#define WHITE 0x00ffffff
#define BLACK 0x00000000
static XTft TftInstance;
struct sched_param sched_par;
pthread_attr_t attr;
pthread_t tid1;

XMbox xmbox;
XMbox_Config *xmbox_ConfigPtr;

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

void tft_displaycontrol(XTft *TftInstance,struct team_t *teamA,struct team_t *teamB,struct ball_t ball)
{
	tft_display_team(TftInstance,teamA,BLUE);
	tft_display_team(TftInstance,teamB,RED);
	Draw_Ball(TftInstance,ball.x,ball.y,GREEN);
}

void tft_clear(XTft *TftInstance,struct team_t *teamA,struct team_t *teamB,struct ball_t ball)
{
	tft_display_team(TftInstance,teamA,BLACK);
	tft_display_team(TftInstance,teamB,BLACK);
	Draw_Ball(TftInstance,ball.x,ball.y,BLACK);
}

void	 tft_temp()
{
}

void tft_display_team(XTft *TftInstance,struct team_t *team,int col)
{
	int i;
	for (i=0;i++;i<5)
	Draw_Player(TftInstance,team->pos[i].x,team->pos[i].y,col);

}


void state(int game_state)
{
	

}
void* thread_func_1 () {
  print("\r Thread 1 running\r\n");
  struct team_t *teamA,*teamB;
  struct ball_t ball;
  int ret;
  while(1) 
  {    
  
  
	/*	Value2 = XTmrCtr_GetValue(TmrCtrInstancePtr, TmrCtrNumber);
	check timer	

	*/
	tft_clear(&TftInstance,teamA,teamB,ball);
	 print("-- Printed the first Part --\r\n");
	 sleep(200);
	 Init(&TftInstance);
	 tft_temp();
	 print("-- Printed the second part --\r\n");
	 sleep(200);
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

/* * Interrupt Vector 
   * to update the VGA (puerly xtft operation 
 */



int main (void) {
  
  //Control does not reach here
  

  
  //Start Xilkernel
  xilkernel_main ();
}


int main_prog(void) {   // This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)
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
  print("\r Success 2 \r\n");
  XTft_ClearScreen(&TftInstance);
  print("-- Screen Cleared --\r\n");
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
	/* * Start of init Timer */
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
	/*End of init timer*/
	
	
  /* * Init of RR Schedule */
  
  ret = pthread_create(&tid1, NULL, (void*)thread_func_1, NULL);
  
  if (ret != 0)  
  {
    xil_printf ("-- ERROR (%d) launching thread_func_1...\r\n", ret);
  }
  else 
  { 
    xil_printf ("Thread 1 launched with ID %d \r\n",tid1);
  }
  
  /* * End of Init RR Schedule */ 

}


