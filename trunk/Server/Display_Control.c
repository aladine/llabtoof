
#include "xio.h"
#include "xtft.h"
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include "xmbox.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "xil_exception.h"
#include "timer.h"
#include "display.h"
#include "controlling.h"

/*TIMER*/
#define TMRCTR_DEVICE_ID        XPAR_TMRCTR_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define TIMER_COUNTER_0	 0

/*MAILBOX*/
#define PCK_SIZE         24

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
volatile GameState local_gs;
volatile  GameState prev_gs;

void update_MailBox() {
  int ret,i,j;
  int tmp=0;
  while(1) 
  {    
  
	 print("-- Display: Start Mailbox Rec--\r\n");
	
    /* * Mailbox receiving */
		int nbytes = 0;		
		while (nbytes < PCK_SIZE) 
		{
		  (void)XMbox_Read(&xmbox, (u32*)(rcvmsg + nbytes), PCK_SIZE - nbytes, &rcvd);
		  nbytes += rcvd;
		}
		
		ret = XMbox_IsEmpty(&xmbox); 
		//if (ret == 0) print("asdf");
		prev_gs=local_gs;
		local_gs.ball.x_pos=rcvmsg[0];
		local_gs.ball.y_pos=rcvmsg[1];
	for (i = 0; i < 2; i++)
	      for (j = 0; j < 5; j++)
  		{
			local_gs.players[j][i].x_pos=rcvmsg[10*i+2*j+2];
			local_gs.players[j][i].y_pos=rcvmsg[10*i+2*j+3];
 		}
		
		local_gs.special=rcvmsg[22];
                // Clean the FIFO: rcvmsg[0] = 0;
		print("Display: Received from Processor 1 ");
		*rcvmsg = 0;
		//if(tmp==0) Timer_Start(&myTimer,TIMER_COUNTER_0);
		tmp=1;
		tft_displaycontrol(&TftInstance,local_gs);
		//XMbox_Flush(&xmbox); 
	 /* end of mailbox */
	 
  }
  
}



void Init_gs(int k)
{
	 int i,j;
    prev_gs.ball.x_pos= 20*k;
        prev_gs.ball.y_pos= 30*k;
        for (j = 0; j < 2; j++)
              for (i = 0; i < 5; i++)
                {
                        prev_gs.players[j][i].x_pos= k+ 100*i+20*j+20;
                        prev_gs.players[j][i].y_pos= k+ 100*i+20*j+50;
                }
               
        prev_gs.special= 1;

     local_gs.ball.x_pos= 30*k;
        local_gs.ball.y_pos= 50*k;

        for (j = 0; j < 2; j++)
              for (i = 0; i < 5; i++)

                {
                        local_gs.players[j][i].x_pos= k+ 100*i+20*j+20;
                        local_gs.players[j][i].y_pos= k+ 100*i+20*j+30;
                }
               
        local_gs.special= 1;

}
	
void display_timer_interrupt(int TimerExpired)
{
			int k;
		  if(TimerExpired<5) 
			{
				k=TimerExpired;
				count_down(&TftInstance,4-TimerExpired);
				Init_gs(k);
			//	tft_clear(&TftInstance,prev_gs);
		  UpdateScreen(&TftInstance);
	     tft_displaycontrol(&TftInstance,local_gs);
		   display_score(&TftInstance, local_gs.special);


			}
		
		  
	     print("-- Update successful --\r\n");
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
  TftConfigPtr = XTft_LookupConfig(XPAR_XPS_TFT_0_DEVICE_ID);

  if (TftConfigPtr == (XTft_Config *)NULL) {
    print("\r Failed 1 \r\n");
	 return XST_FAILURE;
  }

  /*
	* Initialize all the TftInstance members and fills the screen with
	* default background color.
	*/
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
  print("\r Display Success \r\n");

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
  // update_MailBox();
   
	ret= TimerSetup(&myIntc,
				&myTimer,TMRCTR_DEVICE_ID,INTC_DEVICE_ID,TIMER_COUNTER_0);
	Timer_Start(&myTimer,TIMER_COUNTER_0);


}
