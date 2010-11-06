
#include "function.h"
#include <math.h>
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include "gpio.h"
#include "xtmrctr.h"
#include "io/io_player.h"


#define TMRCTR_DEVICE_ID        XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_COUNTER_0	 0
#define RESET_VALUE	 0x8F0D180

XStatus status;
IOPlayermanager io_player;


struct sched_param sched_par;
pthread_attr_t attr;
pthread_t tid1, tid2, tid3, tid4,tid5;
volatile int TimerExpired;

static XTmrCtr myTimer;

XGpio Gpio; /* The Instance of the GPIO Driver */
//XIntc myIntc; /* The Instance of the Interrupt Controller Driver */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* thread_player_0 () 
{
	xil_printf("Entering thread_player_0\n");
	while(1)
	{
		if(BC0==1)
		{
			BC0=0;
			Goalkeeper_Strategy_Defense();			
		}
	}
}
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* thread_player_1 () 
{
	xil_printf("Entering thread_player_1\n");
	while(1)
	{
		 if(BC1==1)
		 {
			 BC1=0;
			 Defender_Strategy_Defense(1);		
		 }
	}	
}
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* thread_player_2 ()
 {
	xil_printf("Entering thread_player_2\n");
	while(1)
	{
		if(BC2==1)
		{
			BC2=0;
			Defender_Strategy_Defense(2);		
		}
	}	
}
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* thread_player_3 ()
{
	xil_printf("Entering thread_player_3\n");
	while(1)
	{
		if(BC3==1)
		{
			BC3=0;
			Striker_Strategy_Attack(3);
		}
	}	
}
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* thread_player_4 ()
{
	xil_printf("Entering thread_player_4\n");
	while(1)
	{
		if(BC4==1)
		{
			BC4=0;
			Striker_Strategy_Attack(4);
		}
	}	
} 

void timer_a_int_handler(void * baseaddr_p) 
{
   Xuint32 csr;

   /* Read timer 0 CSR to see if it requested the interrupt */
   csr = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, TIMER_COUNTER_0);
	xil_printf("In timer interrupt handler...%X \r\n", csr);

   if (csr) 
   {
       TimerExpired++;
   }
	
   // XIntc_Acknowledge(&myIntc,XPAR_INTC_1_TMRCTR_1_VEC_ID);
	XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, TIMER_COUNTER_0,csr);
	csr = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, TIMER_COUNTER_0);
	xil_printf("After...%X \r\n", csr);
}
 
void _callback(GameState * state)
{
	unsigned int i = 0;
	

		xil_printf("\n\r  Player %d : xpos=%d ypos=%d   \n", i, state->players[TEAM_A][i].x_pos, state->players[TEAM_A][i].y_pos);
		
}
/***********************************************************************************************************************************************/
//////////////MAIN_PROG//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
int main (void) {

  //Start Xilkernel
	xilkernel_main ();
  //Control does not reach here
}

int main_prog(void) {   // This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)
 
	int ret;
	
 //-----------------------------------------------------------------------------------------------------------------------------
	int start=0;
	int status;
	u32 DataRead;
	IOPlayer_init(&io_player, TEAM_A,(IOmanager_cb) callback);
	//xil_printf("IOplayer_init done");
	//print(" START the PROGRAM\r\n");

	while(start==0)
	{
		//print(" Press button to Generate Interrupt\r\n");
		 
		status = GpioIntrExample( &Gpio,
		GPIO_DEVICE_ID,
		INTC_GPIO_INTERRUPT_ID,
		GPIO_CHANNEL1, &DataRead);
		 
		if (status == 0 )
		{
			if(DataRead == 0){
				//print("No button pressed. \r\n");
			               }
			else
			{
				//xil_printf("Push button Interrupt Test is %X \r\n",DataRead);
				start=1;
				Initialisation_Beginning();
			}
		}
		else
		{
			print("Gpio Interrupt Test FAILED.\r\n");
		}
	}
	
	
//	status[0] = XIntc_Initialize(&InterruptController, XPAR_INTC_0_DEVICE_ID);
	//xil_printf("\n  XIntc_Initialize   \n");
	
	//xil_printf("\n  Coucou %d  \n", status);
	/*char i;
	for(i=0; i<5; i++)
	{
		output.players[TEAM_A][i].x_pos = 0;
		output.players[TEAM_A][i].y_pos = 0;
		output.players[TEAM_B][i].x_pos = 0;
		output.players[TEAM_B][i].y_pos = 0;
	}
	output.ball.x_pos = 0;
	output.ball.y_pos = 0;
	output.ball.direction = 0;
	output.ball.speed = 0;*/

	
	
	
	/*
	//-----------------------------------------------------------------------------------------------------------------------
 // Hardware Mutex Initialization
 XMutex_Config * xMutexCfg;
 xMutexCfg = XMutex_LookupConfig(XPAR_MUTEX_0_DEVICE_ID);
 Status = XMutex_CfgInitialize(&xMutex, xMutexCfg, xMutexCfg->BaseAddress);  
 // need to check whether the mutex is created accordingly by check the value of "ret"

 // Software Mutex Initialization
 print("-- Entering main() --\r\n");
 ret = pthread_mutex_init(&mutex, NULL);
 // need to check whether the mutex is created accordingly by check the value of "ret"
 
	  */
	 pthread_attr_init (&attr);
	 print("--Initialized --\r\n");
	 // Priority 1 for thread 1
	 sched_par.sched_priority = 1;
	 //pthread_attr_setschedparam(&attr,&sched_par);
	 print("--Priority Set --\r\n");
	 //start thread 1
	 ret = pthread_create (&tid1, &attr, (void*)thread_player_0, NULL);
	 print("--returned --\r\n");
	 if (ret != 0)
	 {
	 xil_printf ("-- ERROR (%d) launching thread_player_0...\r\n", ret);
	 }
	 else
	 { 
	 xil_printf ("Thread 1 launched with ID %d \r\n",tid1);
	 }
	 
	 // Priority 2 for thread 2
	 sched_par.sched_priority = 2;
	 //pthread_attr_setschedparam(&attr,&sched_par);
	 
	 //start thread 2
	 ret = pthread_create (&tid2, &attr, (void*)thread_player_1 ,NULL);
	 if (ret != 0) 
	 {
		xil_printf ("-- ERROR (%d) launching thread_player_1...\r\n", ret);
	 }
	 else 
	 { 
		xil_printf ("Thread 2 launched with ID %d \r\n",tid2);
	 }
	 
	 // Priority 3 for thread 3
	 sched_par.sched_priority = 3;
	 //pthread_attr_setschedparam(&attr,&sched_par);
	 
	 //start thread 3
	 ret = pthread_create (&tid3, &attr, (void*)thread_player_2, NULL);
	 if (ret != 0)
	 {
		xil_printf ("-- ERROR (%d) launching thread_player_2...\r\n", ret);
	 }
	 else 
	 { 
		xil_printf ("Thread 3 launched with ID %d \r\n",tid3);
	 }
	 
	 // Priority 4 for thread 4
	 sched_par.sched_priority = 4;
	 //pthread_attr_setschedparam(&attr,&sched_par);
	 
	 //start thread 4
	 ret = pthread_create (&tid4, &attr, (void*)thread_player_3, NULL);
	 if (ret != 0) 
	 {
		xil_printf ("-- ERROR (%d) launching thread_player_3...\r\n", ret);
	 }
	 else 
	 { 
		xil_printf ("Thread 4 launched with ID %d \r\n",tid4);
	 }
	 
	 // Priority 5 for thread 5
	 sched_par.sched_priority = 5;
	 //pthread_attr_setschedparam(&attr,&sched_par);
	 
	 //start thread 4
	 ret = pthread_create (&tid5, &attr, (void*)thread_player_4, NULL);
	 if (ret != 0) 
	 {
		xil_printf ("-- ERROR (%d) launching thread_player_4...\r\n", ret);
	 }
	 else 
	 { 
		xil_printf ("Thread 5 launched with ID %d \r\n",tid5);
	 }
	
}


