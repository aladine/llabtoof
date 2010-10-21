#include "controlling.h"
#include "msgqueue_api.h"
#include "iostream"
#include "math.h"
using namespace std;
/** definition
 */


/** Global Var
 */
struct team
{
  struct player player[5];
}
d_team[2];

struct game_state d_gamestate;

float x_proj[] = {1, 0.924, 0.707, 0.383, 0, -0.383, -0.707,
                  -0.924, -1, -0.924, -0.707, -0.383, 0, 0.383,
                  0.707, 0.924};
float y_proj[] = {0, -0.383, -0.707, -0.924, -1, -0.924, 
                  -0.707, -0.383, 0, 0.383, 0.707, 0.924,
                  1, 0.924, 0.707, 0.383};
int x_bounce[] = {8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9};
int y_bounce[] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
/** Functions
 */
 
/** Geometrical Calculation Functions
 */

/*get a randome position in the center circle*/
int random_num(int *x, int *y)
{
	*x = rand()%100 + 1;
	*y = rand()%100 + 1;
	*x = *x + DISPLAY_COLUMNS/2-50;
    *y = *y + DISPLAY_ROWS/2-50;
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
int possess_ball(struct player d_player,struct ball d_ball)
{
	if (distance(d_player.x_pos,d_player.y_pos,d_ball.x_pos,d_ball.y_pos)<=15)
	  return 1;
	else 
      return 0;
}

/*check if the players conflict with each other*/
int player_conflict(struct player d_player1, struct player d_player2)
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
 
void change_dir(unsigned char * dir, int cases)
{
	switch (cases)
    {
	case 1:
        *dir = (8 - *dir) % 16;
		break; 
	case 2:
        *dir = (16 - *dir) % 16;
		break; 
	case 3:
        *dir = (12 - *dir) % 16;
		break; 
	case 4:
        *dir = (4 - *dir) % 16;
		break; 
    }
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
      d_team[i].player[j].x_pos
        + (x_proj[d_team[i].player[j].direction]
          * d_team[i].player[j].speed)
            * CONST_PROP * TIME_SLOT;
    d_y = 
      d_team[i].player[j].y_pos
        + (y_proj[d_team[i].player[j].direction]
          * d_team[i].player[j].speed)
            * CONST_PROP * TIME_SLOT;
            
    // Check if the new position is valid,
    // If not then change the directions
    if (bounce_cases = check_bounce(d_x, d_y) == 1)
    {
      // Change the direction                               
      change_dir(&(d_team[i].player[j].direction),bounce_cases);
      // Cal the new position
      d_x = d_team[i].player[j].x_pos
        + (x_proj[d_team[i].player[j].direction]
          * d_team[i].player[j].speed)
            * CONST_PROP * TIME_SLOT;        
      d_y = d_team[i].player[j].y_pos
        + (y_proj[d_team[i].player[j].direction]
          * d_team[i].player[j].speed)
            * CONST_PROP * TIME_SLOT;   
    }     
    d_team[i].player[j].y_pos = d_y;
    d_team[i].player[j].x_pos = d_x;
  }
  // See if the positions have conflicts, if does, modify direction
  for (i = 0; i < 2; i++)
      for (j = 0; j < 5; j++)
          for (i1 = 0; i1 < 2; i1++)
              for (j1 = 0; j1 < 5; j1++)
                  if (!(i == i1 && j == j1))
                  {
                       d_x =  d_team[i].player[j].x_pos;
                       d_y =  d_team[i].player[j].y_pos;
                       d_x1 = d_team[i1].player[j1].x_pos;
                       d_y1 = d_team[i1].player[j1].y_pos;
                       // Check if two conflicts
                       
                       // Change accordingly, Exchange speed
                  }
                  else
                  {
                       d_x =  d_team[i].player[j].x_pos;
                       d_y =  d_team[i].player[j].y_pos;
                       d_x1 = d_gamestate.ball_state.x_pos;
                       d_y1 = d_gamestate.ball_state.y_pos;
                       // Check if two conflicts
                       
                       // Change accordingly, Player remains, ball change
                  }
  // End of change status.

}
 
 
 
/** Sending Data to UartPort
  * Convert data from structure player/ball 
    to uartdata
  * Sall send function
 */
int send_uartdata()
{
  //Players
  struct uartdata d_uartdata;
  unsigned char d_dir,d_spd;
  unsigned d_x,d_y;
  unsigned i,j;
  for (i = 0; i < 2; i++)
      for (j = 0; j < 5; j++)
      {   
          unsigned char buf = 0;       
          d_x = d_team[i].player[j].x_pos;
          d_y = d_team[i].player[j].y_pos;    
          d_x = d_x / CONST_PROP;
          d_y = d_y / CONST_PROP;
    
          // d_uartdata[0]
          if (i) buf = buf + 0x40; 
          buf = buf + 0x20;
          buf = buf + j * 2;
          if (d_x / 512) buf = buf + 1;
          d_uartdata.data[0] = buf;
          
          // d_uartdata[1]          
          buf = 0;
          buf = (d_x % 512) / 2;
          d_uartdata.data[1] = buf;
          
          // d_uartdata[2]          
          buf = 0;
          buf = buf + (d_x % 2) * 128;
          buf = buf + (d_y / 8);
          d_uartdata.data[2] = buf;
          
          // d_uartdata[3]
          buf = 0;
          buf = buf + (d_y % 8);
          d_uartdata.data[3] = buf;
          
          msgqueue_send(&d_uartdata);
      }
  //Ball
  unsigned char buf = 0;  
      
  d_x = d_gamestate.ball_state.x_pos;
  d_y = d_gamestate.ball_state.y_pos;       
  d_x = d_x / CONST_PROP;
  d_y = d_y / CONST_PROP;
  d_spd = d_gamestate.ball_state.speed;
  
  // d_uartdata[0]
  if (d_x / 512) buf = buf + 1;
  d_uartdata.data[0] = buf;
          
  // d_uartdata[1]          
  buf = 0;
  buf = (d_x % 512) / 2;
  d_uartdata.data[1] = buf;
          
  // d_uartdata[2]          
  buf = 0;
  buf = buf + (d_x % 2) * 128;
  buf = buf + (d_y / 8);
  d_uartdata.data[2] = buf;
          
  // d_uartdata[3]
  buf = 0;
  buf = buf + (d_y % 8);
  buf = buf + (d_spd * 2);
  d_uartdata.data[3] = buf;
          
  msgqueue_send(&d_uartdata);   
  return 1;
}


/** Converting data from Uartdata
  * Convert data from uartdata to structure
    player/ball
  * Kick check needed
  * Still need kick check
 */
int read_uartdata()
{
  struct uartdata d_uartdata;       
  unsigned char d_dir,d_spd;
  unsigned d_x,d_y;
  unsigned char d_team_id,d_player_id;
  unsigned char d_kick;
  int message_count[2] = {0,0};
  int i,j,k,ret;  
  while (msgqueue_recv(&d_uartdata))
  {
    /** convert to structure of general definition */
    
    //Assume that the first bit (indicating the packet type) first at data[0]
    d_team_id = (d_uartdata.data[0] & 0x20) / 32;    
    
    //Make sure that no one team will crowded the message queue
    message_count[d_team_id] = message_count[d_team_id] + 1;
    if (message_count[d_team_id] > MAX_MESSAGE_COUNT) break;//continue;
	if ((d_uartdata.data[0] > 7) == 1)
	{
	   // Process as the Initial position packet
	   // Get from the packet
       d_player_id = (d_uartdata.data[0] & 0x1E) / 2;
       d_x = (d_uartdata.data[0] & 0x01) * 512
           + (d_uartdata.data[1]) * 2
           + (d_uartdata.data[2] & 0x80) / 128;
       d_y = (d_uartdata.data[2] & 0x7F) * 8
           + (d_uartdata.data[3] & 0xE0) / 32;
       
       d_x = d_x * CONST_PROP;
       d_y = d_y * CONST_PROP;
       
       // Put to respective data structure
       d_team[d_team_id].player[d_player_id].x_pos = d_x;
       d_team[d_team_id].player[d_player_id].y_pos = d_y;       
       d_team[d_team_id].player[d_player_id].direction = 0;      
       d_team[d_team_id].player[d_player_id].speed = 0;
    }
	else
	{
       d_kick = (d_uartdata.data[0] & 0x40) / 64;
       d_player_id = (d_uartdata.data[0] & 0x1E) / 2;
       d_dir = (d_uartdata.data[0] & 0x01) * 8
             + (d_uartdata.data[1] & 0xE0) / 32;
       d_spd = (d_uartdata.data[1] & 0x1E) / 2;
	   // Process as the update packet
       d_team[d_team_id].player[d_player_id].direction = d_dir;      
       d_team[d_team_id].player[d_player_id].speed = d_spd;	   
       d_team[d_team_id].player[d_player_id].kick = d_kick;	   
    }
  }
  return 1;
}
 
 
 
//int controller_main()
int main()
{
  struct uartdata d_uartdata;
  int ret;
  /** read from message queue */
  ret = read_uartdata();
  /** Check if there's a foul or other status */
  
  /** Calculate the new position and speed and anything else */
  cal_parameter();  
  /** Send data back to uartlite portal */
  ret = send_uartdata();

  /** timer interrupt --> call mailbox send data */
} 
