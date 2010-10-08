/*
 -----------------------------------------------------------------------------
 -- Copyright (C) 2005 IMEC                                                  -
 --                                                                          -
 -- Redistribution and use in source and binary forms, with or without       -
 -- modification, are permitted provided that the following conditions       -
 -- are met:                                                                 -
 --                                                                          -
 -- 1. Redistributions of source code must retain the above copyright        -
 --    notice, this list of conditions and the following disclaimer.         -
 --                                                                          -
 -- 2. Redistributions in binary form must reproduce the above               -
 --    copyright notice, this list of conditions and the following           -
 --    disclaimer in the documentation and/or other materials provided       -
 --    with the distribution.                                                -
 --                                                                          -
 -- 3. Neither the name of the author nor the names of contributors          -
 --    may be used to endorse or promote products derived from this          -
 --    software without specific prior written permission.                   -
 --                                                                          -
 -- THIS CODE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''           -
 -- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED        -
 -- TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A          -
 -- PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR       -
 -- CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,             -
 -- SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         -
 -- LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF         -
 -- USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      -
 -- ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,       -
 -- OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT       -
 -- OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       -
 -- SUCH DAMAGE.                                                             -
 --                                                                          -
 -----------------------------------------------------------------------------
 -----------------------------------------------------------------------------
 -- File           : MAILBOX_BALL.c
 -----------------------------------------------------------------------------
 -- Description    : C code
 -- --------------------------------------------------------------------------
 -- Author         :Shakith/Manmohan
 -- Date           : 
 -- Change history : 
 -----------------------------------------------------------------------------
*/
#include "xmk.h"
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/timer.h>
#include "xmbox.h"
#include "xtmrctr.h"
#include "timer.h"

#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID



#define TIMER_COUNTER_0	 0

#define HELLO_SIZE 40;
#define MBOX_DEVICE_ID XPAR_MBOX_0_DEVICE_ID
pthread_t tid1;
static XMbox Mbox;


int TmrCtrPolledExample(u16 DeviceId, u8 TmrCtrNumber);

/************************** Variable Definitions *****************************/

XTmrCtr TimerCounter; /* The instance of the Tmrctr Device */

struct ball_msg {
  
  int dir,speed,x,y;
};


void send (int dir,int speed,int x, int y) {

	struct ball_msg ball;
	int ret;
	ball.dir = dir;
	ball.speed = speed;
	ball.x = x;
	ball.y = y;
     // print
     xil_printf("\rSend ID : %d , X = %d ,  Y = %d \r", ball.speed,ball.dir,ball.x,ball.y);
  
  //ENTER THE CODE HERE TO SEND THE BALL DATA TO THE MAILBOX
		XMbox_WriteBlocking(&Mbox, (u32*)(&ball), sizeof(ball));
}



void* thread_func_1 () {
  while(1) {
    send(10,10,10,20); // THESE ARE SAMPLE DATA ... you can modify this area to generate continuous data through the mailbox
  }
}




int main (void) {

  //Start Xilkernel
  xilkernel_main ();
  
  //Control does not reach here
  
}

int main_prog(void) {   // This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)

  int ret;
XStatus Status;
  
  
  print("-- Entering main() --\r\n");
 /*
	// CONFIGURE THE MAILBOX HERE
XMbox_Config *ConfigPtr;

	ConfigPtr = XMbox_LookupConfig(MBOX_DEVICE_ID );
	if (ConfigPtr == (XMbox_Config *)NULL) {
		return XST_FAILURE;
	}

	Status = XMbox_CfgInitialize(&Mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	
	*/
	Status = TmrCtrPolledExample(TMRCTR_DEVICE_ID, TIMER_COUNTER_0);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
   
  //start thread 1
  ret = pthread_create (&tid1, NULL, (void*)thread_func_1, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching thread_func_1...\r\n", ret);
  }
  else { 
    xil_printf ("Thread 1 launched with ID %d \r\n",tid1);
  }

 

}

