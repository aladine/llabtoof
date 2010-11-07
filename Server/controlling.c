#include "controlling.h"
#include "msgqueue_api.h"
#include "timer-server.h"
#include "math.h"
#include "string.h"

#include "xgpio.h"
#include "xtmrctr.h"
#include "xil_exception.h"
#include <sys/intr.h>
#include "io/io_server.h"
#include "io/io_structures.h"

/** definition
 */
//#define CHECK_TIME_SLOT 0x1E8480
#define CHECK_TIME_SLOT 0x40E8480
#define INTC_DEVICE_ID          XPAR_INTC_1_DEVICE_ID
/** Global Var
 */
XMbox mbox;
XMbox_Config *mbox_ConfigPtr;
XStatus status;
XGpio myGpio;
XTmrCtr myTimer;
IOServermanager  server; 

pthread_mutex_t temp_gs_mutex; 
volatile GameState temp_gs;
volatile GameState local_gs;
int red_zone_left[2];
int red_zone_right[2];
pthread_t tid1;

u32 global_timer_checkpoint;
volatile int global_pb_start, global_init_packet;
volatile int testbench_check;
volatile int pb_counter;

unsigned char uart_control_signal;
/** UART Control Signal Controls what is sending through uart 
  * 1. Sending Controlling frame (start the game)
  * 2. Sending Controlling frame (scored)
  * 3. Sending Controlling frame (foul)
  * 4. Sending Controlling frame (reset)
  * 5. Sending Controlling frame (off-time)
  * FF. Sending DataUpdate frame
*/
float y_proj[] = {1, 0.924, 0.707, 0.383, 0, -0.383, -0.707,
                  -0.924, -1, -0.924, -0.707, -0.383, 0, 0.383,
                  0.707, 0.924};
float x_proj[] = {0, 0.383, 0.707, 0.924, 1, 0.924, 0.707, 
                  0.383, 0, -0.383, -0.707, -0.924, -1, -0.924, 
				  -0.707, -0.383};
/** Functions
 */
 
/** Geometrical Calculation Functions
 */

int sgn(int a)
{
	if (a >= 0) 
		return 1;
	else 
		return 0;
}
/* get a randome position in the center circle*/
int random_num(int *x, int *y)
{
	*x = rand()%100 + 1;
	*y = rand()%100 + 1;
	*x = *x + DISPLAY_COLUMNS/2-50;
    *y = *y + DISPLAY_ROWS/2-50;
	return 1;
}

/* check anything in the field*/
int in_border(int x, int y){
	if((x>=0) && (x<=600) && (y>=0) && (y<=400) && (x+y>=20) && (600-x+y>=20) && (600-x+400-y>=20) && (x+400-y>=20)) return 1;
	return 0;
  	
}
/* check the player in the field*/
int in_border_player(int x, int y){
	if((x>=7) && (x<=600-7) && (y>=7) && (y<=400-7) && (x+y>=30) && (600-x+y>=30) && (600-x+400-y>=30) && (x+400-y>=30)) return 1;
	return 0;
  	
}

/* check the ball in the field*/
int in_border_ball(int x, int y){
	if((x>=5) && (x<=600-5) && (y>=5) && (y<=400-5) && (x+y>=28) && (600-x+y>=28) && (600-x+400-y>=28) && (x+400-y>=28)) return 1;	return 0;
  	
}

/* Return the distance between two points */
float distance(int x, int y, int x1, int y1){
	return sqrt((x - x1)*(x - x1) + (y - y1)*(y - y1));
}

/* check if the player possess the ball*/
int possess_ball(Player d_player,Ball d_ball)
{
	if (distance(d_player.x_pos,d_player.y_pos,d_ball.x_pos,d_ball.y_pos)<=15)
	  return 1;
	else 
      return 0;
}


/* check if the players conflict with each other*/
int player_conflict(Player d_player1, Player d_player2)
{
	if (distance(d_player1.x_pos,d_player1.y_pos,d_player2.x_pos,d_player2.y_pos)<=17)
	  return 1;
	else 
      return 0;
}
/* Check bounce to the wall*/
int check_bounce(int x, int y)
{
	if((x<=5) || (x>=600-5)) return 1;
	if((y<=5) || (y>=400-5)) return 2;
	if((x+y<=28) || (600-x+400-y>=28)) return 3;
	if((600-x+y<=28) || (x+400-y>=28)) return 4;
    return 0;
}

/* Change the direction according to the mirror direction */
void change_dir(unsigned char * dir, unsigned char mirror_dir)
{
	int tmp = (mirror_dir * 2 ) % 16;
	*dir = (tmp + 16 - *dir) % 16;
}



/* Check the mirror direction */
unsigned char check_mirror_dir(int x, int y, int x1, int y1)
{
	// Mirror_dir is the value of :
	// 1, 0.924, 0.707, 0.383, 0, -0.383, -0.707, -0.924,float x_proj[] = {0 .. 7}
	int i;
	for (i = 0; i < 8; i++)
		if (sgn((x - x1) * y_proj[i] - (y - y1) * x_proj[i]) 
	     != sgn((x - x1) * y_proj[i + 1]  - (y - y1) * x_proj[i + 1]))
			 break;
	 return (i + 4) % 8;
}



/* Change the direction of ball/player*/ 
void boundary_bounce(unsigned char * dir, int cases)
{
	unsigned char mirror_dir = 0;
	switch (cases)
    {
	case 1:
        mirror_dir = 0;
		break; 
	case 2:
        mirror_dir = 4;
		break; 
	case 3:
        mirror_dir = 6;
		break; 
	case 4:
        mirror_dir = 2;
		break; 
    }
    change_dir(dir, mirror_dir);
}

int in_red_zone_right(Player d_player)
{
    //
	 return 0;
}
int in_red_zone_left(Player d_player)
{
    //
	 return 0;
}
/** End of Geometrical Calculation 
 */  
 
 
/* *************************************************************************** */
/** Begin definition of operational Functions
  * cal_parameter()
  * check_fouls()
  * send_uartdata()
  * read_uartdata()
  * send_Mailbox()
  */

/** Calculate the new position and ball speed of the playground
 */
void cal_parameter()
{ 
  int i, j, i1, j1;
  int d_x, d_y;
  int d_x1, d_y1;
  int bounce_cases;
  
  //Calculate the new positions of the players
  for (i = 0; i < 2; i++)
      for (j = 0; j < 5; j++)
  {
    d_x = 
	  x_proj[local_gs.players[i][j].x_pos]
        + (x_proj[local_gs.players[i][j].direction]
          * local_gs.players[i][j].speed)
            * CONST_PROP * TIME_SLOT;
    d_y = 
      local_gs.players[i][j].y_pos
        + (y_proj[local_gs.players[i][j].direction]
          * local_gs.players[i][j].speed)
            * CONST_PROP * TIME_SLOT;
            
    // Check if the new position is valid,
    // If not then change the directions
    if (bounce_cases = check_bounce(d_x, d_y) == 1)
    {
      // Change the direction  
	  local_gs.players[i][j].direction = 4;
      boundary_bounce(&(local_gs.players[i][j].direction),bounce_cases);
      // Cal the new position
      d_x = local_gs.players[i][j].x_pos
        + (x_proj[local_gs.players[i][j].direction]
          * local_gs.players[i][j].speed)
            * CONST_PROP * TIME_SLOT;        
      d_y = local_gs.players[i][j].y_pos
        + (y_proj[local_gs.players[i][j].direction]
          * local_gs.players[i][j].speed)
            * CONST_PROP * TIME_SLOT;   
    }     
    local_gs.players[i][j].y_pos = d_y;
    local_gs.players[i][j].x_pos = d_x;
  }
  // See if the positions have conflicts, if does, modify direction
  for (i = 0; i < 2; i++)
      for (j = 0; j < 5; j++)
          for (i1 = i; i1 < 2; i1++)
              for (j1 = j; j1 < 5; j1++)
                  if (!(i == i1 && j == j1))
                  {
                       // Check if two conflicts
                       if (player_conflict(local_gs.players[i][j],
                                           local_gs.players[i1][j1]))
                       {
                           // Change accordingly, Exchange speed    
						   unsigned char mirror_dir = check_mirror_dir(local_gs.players[i][j].x_pos,local_gs.players[i][j].y_pos,
							   local_gs.players[i1][j1].x_pos,local_gs.players[i1][j1].y_pos);
						   change_dir(&local_gs.players[i][j].direction, mirror_dir);
						   change_dir(&local_gs.players[i1][j1].direction, mirror_dir);
                       }
                  }
                  else
                  {
                       // Check if player process the ball
                       if (possess_ball(local_gs.players[i][j],
                                        local_gs.ball))
                       {
                           if (local_gs.players[i][j].kick_speed != 0)
                           {
                               // Change the speed and dir of the ball by kick d                                                    
                               local_gs.ball.direction = 
                                 local_gs.players[i][j].kick_direction;
							   local_gs.ball.speed = 
                                 local_gs.players[i][j].kick_speed;                                 
                               local_gs.players[i][j].kick_speed = 0;
                           }            

                           else
                           {
											// Change accordingly, Player remains, ball change
											unsigned char mirror_dir = check_mirror_dir(local_gs.players[i][j].x_pos,local_gs.players[i][j].y_pos,
											local_gs.ball.x_pos,local_gs.ball.y_pos);
											change_dir(&local_gs.ball.direction, mirror_dir);
                           }
                       }                       
                  }
  // Slow down the ball according to the friction
  // End of change status.

}




void check_fouls()
{
	int i, j;
	int red_zone_count_left[2] = {0};
	int red_zone_count_right[2] = {0};
	for (i = 0; i < 2; i++)
		for (j = 0; j < 5; j++)
		{
			if (in_red_zone_left(local_gs.players[i][j]))
				red_zone_count_left[i] ++;
			if (in_red_zone_right(local_gs.players[i][j]))
				red_zone_count_right[i] ++; 
		}
	if (red_zone_count_left[1] > 1 && red_zone_count_left[1] != red_zone_left[1])
	{
		// Foul Team A
	}
	if (red_zone_count_right[1] > 2 && red_zone_count_right[1] != red_zone_right[1])			
	{
		// Foul Team A
	}
	if (red_zone_count_left[2] > 2 && red_zone_count_left[2] != red_zone_left[2])
	{
		// Foul Team B
	}
	if (red_zone_count_right[2] > 1 && red_zone_count_right[2] != red_zone_right[2])			
	{
		// Foul Team B
	}
}
/** Sending Data to UartPort
 *//*
int send_uartdata()
{
	gs_temp = local_gs;
	//msgqueue_send(&gs_temp);
	IOServer_send(IOServermanager * server, GameState * output);

	return 1;
}
*/

/** Converting data from Uartdata
 *//*
int read_uartdata()
{
	(GameState*) asdf = msgqueue_recv(&gs_temp);
	gs_temp = asdf;
	int i, j;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
		{
			local_gs.players[i][j].direction = gs_temp.players[i][j].direction;
			local_gs.players[i][j].kick_direction = gs_temp.players[i][j].kick_direction;
			local_gs.players[i][j].speed = gs_temp.players[i][j].speed;
			local_gs.players[i][j].kick_speed = gs_temp.players[i][j].kick_speed;
		}
	return 1;
}*/

/** Send Position and display Data to Mailbox
 */
int send_Mailbox(GameState gs)
{
        int ret,i,j;
        GameMessage game_msg;
                print("Send \r\n");
			
        game_msg.pos[0]=gs.ball.x_pos;
        game_msg.pos[1]=gs.ball.y_pos;
        for (i = 0; i < 2; i++)
                for (j = 0; j < 5; j++)
                {
                        game_msg.pos[10*i+2*j+2]=gs.players[i][j].x_pos;
                        game_msg.pos[10*i+2*j+3]=gs.players[i][j].y_pos;
                }
                
        print("\rControlling: start to send message!!\r\n");
        game_msg.special=1;//gs.special;
        xil_printf(" Size of structure : %d    \r\n",sizeof(game_msg));
		  ret = XMbox_IsEmpty(&mbox);
        if (ret) 
		  {
			  XMbox_WriteBlocking(&mbox, (u32*)(&game_msg), 24);
		  }
		  else
		  {
		     xil_printf("asdfasdfasdf\r\n");
		  }
}

 
 
void* controller_update()
{
  while (1) {}
}

void controlling_update()
{
	unsigned char i, j;
	u32 ret;
	unsigned char mirror_dir;
	
	if(!pthread_mutex_trylock(&temp_gs_mutex))
	{
		//copy temp_gs to appropriate struture
		for (i = 0; i < 2; i++)
			for (j = 0; j < 5; j++)
			{
				local_gs.players[i][j].speed = temp_gs.players[i][j].speed;
				local_gs.players[i][j].direction = temp_gs.players[i][j].direction;
				local_gs.players[i][j].kick_speed = temp_gs.players[i][j].kick_speed;
				local_gs.players[i][j].kick_direction = temp_gs.players[i][j].kick_direction;
			}
	}
	
 /*while (1)
 {
 
 //	ret = Timer_Value_Poll();
	if (ret / CHECK_TIME_SLOT != global_timer_checkpoint / CHECK_TIME_SLOT)
	{*/
      global_timer_checkpoint = ret;
		send_Mailbox(local_gs);
		//k = k + 1;
		xil_printf("checkpoint+1\r\n");
		//** read from message queue 
		//ret = read_uartdata();
		//** Check if there's a foul or other status 
		//check_fouls();
		//** Calculate the new position and speed and anything else 
		//** In this part also calculate all the necessary package.
		//cal_parameter();  
		//** Send data back to uartlite portal and at the same time mailbox to TFT
		//ret = send_uartdata();
		//ret = send_Mailbox();
		//** timer interrupt --> call mailbox send data 
//	}
//	}
} 




/** The code from here on is the initialization and configuration 
  * of the hardware and software components
  */

// Push Button ISR: function is called upon the depression of ANY of the 4
// buttons. Which button is determined within the ISR.
void pb_int_handler(void *baseaddr_p) 
{
	Xuint32 dsr;
	XGpio_InterruptDisable(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);

	dsr= XGpio_DiscreteRead(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);
	pb_counter++;
	xil_printf("dsr is %X  . Press %d times \r\n",dsr,pb_counter);	

	if(pb_counter%2==1)
	switch(dsr) 
	{
		case 0x01:
			xil_printf("Middle: Start the game \r\n");
			global_pb_start = 1;
			break;		
		case 0x02:
			xil_printf("Right: Team B foul\r\n");

			break;
		case 0x08:
			xil_printf("Start the match\r\n");
			break;
		case 0x04:
			xil_printf("Left:Team A foul\r\n");
			break;
		default : {}
	}
	//Clear the interrupt both in the Gpio instance as well as the interrupt controller
   testbench_check = 1;

	XGpio_InterruptClear(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);
	XGpio_InterruptEnable(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);
	
}
int Gpio_Init()
{
       //Initialize and configure the push buttons
       XGpio_Initialize(&myGpio, XPAR_BUTTONS_3BIT_DEVICE_ID);
       XGpio_SetDataDirection(&myGpio, 1, 0x3);
       XGpio_InterruptEnable(&myGpio, XPAR_BUTTONS_3BIT_IP2INTC_IRPT_MASK);
       XGpio_InterruptGlobalEnable(&myGpio);
       //*******************************************************************************************
      
         register_int_handler(XPAR_INTC_1_GPIO_0_VEC_ID,  (XInterruptHandler)pb_int_handler, &myGpio);
         enable_interrupt (XPAR_INTC_1_GPIO_0_VEC_ID);

      // xil_printf("Entering push button loop...\r\n");
		  

}

int MailBox_Init()
{
	int ret,i,j;
	unsigned char mirror_dir;
	//** read from message queue 
  
	//**Initialize mailbox
   
	mbox_ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
	if (mbox_ConfigPtr == (XMbox_Config *)NULL)
	{
		xil_printf ("\tLookupConfig Failed.\r\n");
		return XST_FAILURE;
    }
        
    status = XMbox_CfgInitialize(&mbox, mbox_ConfigPtr, mbox_ConfigPtr->BaseAddress);
    if (status != XST_SUCCESS)
	{
		xil_printf ("\tXMbox_CfgInitialize Failed.\r\n");
		return status;
    }
} 


void io_callback(GameState * state)
{
	//xil_printf("Local gs finished");
	pthread_mutex_lock(&temp_gs_mutex);
	memcpy(&local_gs, state, sizeof(GameState));
	pthread_mutex_unlock(&temp_gs_mutex);
	xil_printf("Local gs finished");
}


//int controller_init()
int main(void)
{
	xilkernel_main ();
}

int main_prog(void)
{
	//** Function execute when interrupt of Push_Button Comes 
	int ret;
	//** initial all the hardware and software components
	//MailBox_Init();

	Gpio_Init();
	Timer_Init(&myTimer);
	Timer_Start_Count(&myTimer);

	ret = pthread_mutex_init(&temp_gs_mutex, NULL);

	//** Two Threads: 1 for IO 2 for timer checking
	
	IOServer_init(&server, io_callback);
	//**Controlling_Thread_init;
//	ret = pthread_create (&tid1, NULL, (void*)controller_update, NULL);
	
	//if (ret)
		//xil_printf ("-- ERROR (%d) launching thread_func_1...\r\n", ret);
	//else
	//	xil_printf ("Thread 1 launched with ID %d \r\n",tid1);

	//** Global Variable Init
	global_pb_start = 0;
	global_init_packet = 0;
  
	//** Wait for push button to start the program*/
	
	while (1)
	{
		if (global_pb_start == 1) break;
		//delay(1000)
	}

  //** Wait for process time
/*
  //** read from message queue 
  while (1)
  {
	  if (global_init_packet == 1) break;
  }
*/
  //** Changing the initial position of players 
  //ret = send_Mailbox(local_gs);
  //** Wait for the 3.2.1....
  
  //** Began the game (Here, the timer starts)
 // Timer_Start_Count(&myTimer);
 
}
