#include "controlling.h"
#include "msgqueue_api.h"
#include "math.h"
#include "xmbox.h"
#include <xparameters.h>

/** definition
 */


/** Global Var
 */
XMbox mbox;
XMbox_Config *ConfigPtr;
XStatus status;


GameState local_gs;

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
/*get a randome position in the center circle*/
int random_num(int *x, int *y)
{
	*x = rand()%100 + 1;
	*y = rand()%100 + 1;
	*x = *x + DISPLAY_COLUMNS/2-50;
    *y = *y + DISPLAY_ROWS/2-50;
	return 1;
}

/*check anything in the field*/
int in_border(int x, int y){
	if((x>=0) && (x<=600) && (y>=0) && (y<=400) && (x+y>=20) && (600-x+y>=20) && (600-x+400-y>=20) && (x+400-y>=20)) return 1;
	return 0;
  	
}
/*check the player in the field*/
int in_border_player(int x, int y){
	if((x>=7) && (x<=600-7) && (y>=7) && (y<=400-7) && (x+y>=30) && (600-x+y>=30) && (600-x+400-y>=30) && (x+400-y>=30)) return 1;
	return 0;
  	
}

/*check the ball in the field*/
int in_border_ball(int x, int y){
	if((x>=5) && (x<=600-5) && (y>=5) && (y<=400-5) && (x+y>=28) && (600-x+y>=28) && (600-x+400-y>=28) && (x+400-y>=28)) return 1;	return 0;
  	
}

float distance(int x, int y, int x1, int y1){
	return sqrt((x - x1)*(x - x1) + (y - y1)*(y - y1));
}

/*check if the player possess the ball*/

int possess_ball(Player d_player,Ball d_ball)
{
	if (distance(d_player.x_pos,d_player.y_pos,d_ball.x_pos,d_ball.y_pos)<=15)
	  return 1;
	else 
      return 0;
}

/*check if the players conflict with each other*/

int player_conflict(Player d_player1, Player d_player2)
{
	if (distance(d_player1.x_pos,d_player1.y_pos,d_player2.x_pos,d_player2.y_pos)<=17)
	  return 1;
	else 
      return 0;
}

/**if the player possess the ball, the ball will either 
 * When a moving player comes in contact with a stationary ball, 
 * the ball gets twice the speed of the player, and follows the player's direction.
 * 
 * When the moving ball comes in contact with a stationary or moving player, 
 * the perpendicular speed of the ball is reversed
**/
/*
int check_contact(struct player *d_player,struct ball *d_ball)
{
	if (possess_ball(*d_player,*d_ball) == 1)
	{
	  if(d_player->speed == 0)
      {
		d_ball->direction = (d_ball->direction+8) % 16; 
	  }
      else
      {
		d_ball->speed = 2*d_player->speed;
		d_ball->direction = player->direction;
	  }
	  return 1;
	}
	return 0;
}
*/
/*Check bounce to the wall*/
int check_bounce(int x, int y)
{
	if((x<=5) || (x>=600-5)) return 1;
	if((y<=5) || (y>=400-5)) return 2;
	if((x+y<=28) || (600-x+400-y>=28)) return 3;
	if((600-x+y<=28) || (x+400-y>=28)) return 4;
    return 0;
}


void change_dir(unsigned char * dir, unsigned char mirror_dir)
{
	int tmp = (mirror_dir * 2 ) % 16;
	*dir = (tmp + 16 - *dir) % 16;
}

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


/*Change the direction of ball/player*/ 
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

/** End of Geometrical Calculation 
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
          for (i1 = 0; i1 < 2; i1++)
              for (j1 = 0; j1 < 5; j1++)
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
 


 
/** Sending Data to UartPort
 */
int send_uartdata()
{
	return 1;
}


/** Converting data from Uartdata
 */
int read_uartdata()
{
	return 1;

}

/** Send Position and display Data to Mailbox
 */
int send_Mailbox(GameState gs)
{
	int ret,i,j;
	GameMessage game_msg;

	game_msg.pos[0]=gs.ball.x_pos;
	game_msg.pos[1]=gs.ball.y_pos;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 5; j++)
  		{
			game_msg.pos[10*i+2*j+2]=gs.players[i][j].x_pos;
			game_msg.pos[10*i+2*j+3]=gs.players[i][j].y_pos;
 		}
		
	print("\rControlling: start to send message!!\r\n");
	game_msg.special=gs.special;
	xil_printf(" Size of structure : %d    \r\n",sizeof(game_msg.special));
	ret = XMbox_IsEmpty(&mbox);
		
	if (ret == 0) XMbox_WriteBlocking(&mbox, (u32*)(&game_msg.special), sizeof(game_msg.special));
   if(ret!=0)
	{	
		print("\rERROR!! mailbox not empty \r\n");
	}
	else
	{
		print("\rSend Message successfully !!\r\n");
	}
	return 1;
}
 
 
 
//int controller_update()
int main()
{
  int ret,i,j;
  unsigned char mirror_dir;
  //** read from message queue 
  
  /*Initialize mailbox*/
   
 ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
  if (ConfigPtr == (XMbox_Config *)NULL){
		xil_printf ("\tLookupConfig Failed.\r\n");
		return XST_FAILURE;
	}
	
	status = XMbox_CfgInitialize(&mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (status != XST_SUCCESS){
		xil_printf ("\tXMbox_CfgInitialize Failed.\r\n");
		return status;
	}
  
  //ret = read_uartdata();
  //** Check if there's a foul or other status 
  //check_fouls();
  //** Calculate the new position and speed and anything else 
  //** In this part also calculate all the necessary package.
  //cal_parameter();  
  //** Send data back to uartlite portal and at the same time mailbox to TFT
  
  		local_gs.ball.x_pos=100;
		local_gs.ball.y_pos=100;
		

	for (i = 0; i < 2; i++)
	      for (j = 0; j < 5; j++)
  		{
			local_gs.players[i][j].x_pos=200;
			local_gs.players[i][j].y_pos=300;
 		}
		
		local_gs.special=1;

  //ret = send_uartdata();
  ret = send_Mailbox(local_gs);
  //** timer interrupt --> call mailbox send data 
} 


int controller_init()
{
  //** Function execute when interrupt of Push_Button Comes 
  int ret;
  
  //** Send uartdata control frame */
  ret = send_uartdata(uart_control_signal);

  //** read from message queue 
  ret = read_uartdata();
  
  //** Changing the initial position of players 
  ret = send_Mailbox(local_gs);
  //** Wait for Push_Button from Player_Side
  while (1)
  {
    ret = read_uartdata();  
    //** Check if there is a Push_Button Issue from Player Side 
  } 
  //** Began the game (Here, the timer starts)
}
