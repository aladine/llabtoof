/*
 -----------------------------------------------------------------------------
 -----------------------------------------------------------------------------
 -- File           : MAILBOX_PLAYERS.c
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


#define MSGSIZ  16

pthread_t tid1,tid2,rd_mbox;


XMbox mbox;
XMbox_Config *ConfigPtr;
XStatus status;


struct player_msg {
  
  int id,x,y;
};
struct ball_msg {
 int dir,speed,x,y;

};


#define PCK_SIZE 16
int p_q,b_q,nbytes,rcvd;

int rcvmsg[MSGSIZ];
key_t p_key=1,b_key = 2;


void* thread_func_1 () {
  while(1) {
/*waiting for DTE
send message queue to thread 3*/
ret = msgsnd(b_q, &ball, sizeof(ball), 0); 
  }
}


void* thread_func_2 () {
  while(1) {
/*waiting for DCE signal
send message queue to thread 3*/
ret = msgsnd(b_q, &ball, sizeof(ball), 0); 
  }
}


void* thread_func_3 () {
  struct player_msg player;
  struct ball_msg ball;
  while(1) {
/*convert data to game state, players and vall information and send to mailbox*/
	
    // wait for message from send
    
    if (msgrcv(p_q, &player, sizeof(player), 0, 1) != sizeof(player)) {
      xil_printf("\rDisplay: msgrcv returns error: %d\r\n", errno);
    }
    
    // print message
    xil_printf ("\rDisplay received : Player ID = %d, x = %d  y= %d  \r\n", player.id, player.x,player.y);
    if (msgrcv(b_q, &ball, sizeof(ball), 0, 1) != sizeof(ball)) {
      xil_printf("\rDisplay: msgrcv returns error: %d\r\n", errno);
    }
        xil_printf ("\rDisplay received : Ball Speed  = %d, Direction = %d , x = %d  y= %d  \r\n", ball.speed,ball.dir,ball.x,ball.y);
 
	/*Send to mailbox*/  
        XMbox_WriteBlocking(&mbox, (u32*)(&ball), sizeof(ball));

 //add XMox_flush()???
  }

}



int main (void) {

  //Start Xilkernel
  xilkernel_main ();
  
  //Control does not reach here
  
}

XMbox mbox;
XMbox_Config *ConfigPtr;
XStatus status;

int main_prog(void) {   // This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)

  int ret;
  
  
  print("-- Entering main() --\r\n");
  p_q = msgget (p_key, IPC_CREAT);
  b_q = msgget (b_key, IPC_CREAT);
	
  xil_printf("Player queue %d\r\n", p_q);
  xil_printf("Ball queue %d\r\n", b_q);
  
  ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
  if (ConfigPtr == (XMbox_Config *)NULL){
		xil_printf ("\tLookupConfig Failed\r\n");
		return XST_FAILURE;
	}
	
	status = XMbox_CfgInitialize(&mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (status != XST_SUCCESS){
		xil_printf ("\tXMbox_CfgInitialize Failed.\r\n");
		return status;
	}
  

  
  //start thread 1
  ret = pthread_create (&tid1, NULL, (void*)thread_func_1, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching thread_func_1...\r\n", ret);
  }
  else { 
    xil_printf ("Thread 1 launched with ID %d \r\n",tid1);
  }

  //start thread 2
  ret = pthread_create (&tid2, NULL, (void*)thread_func_2, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching thread_func_2...\r\n", ret);
  }
  else { 
    xil_printf ("Thread 2 launched with ID %d \r\n",tid2);
  }

  //start thread 3: Mbox
  ret = pthread_create (&rd_mbox, NULL, (void*)thread_func_3, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching thread_func_3...\r\n", ret);
  }
  else { 
    xil_printf ("Thread 3 launched with ID %d \r\n",tid3);
  }
    
 

}

