#include "xparameters.h"
#include "xio.h"
#include "xtft.h"
#include "display.h"
#include "stdio.h"
#include "stdlib.h"

#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID

#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define BLUE 0x000000ff

static XTft TftInstance;


//Struct

struct ball_t {
  
			unsigned char x[10];
			unsigned char y[10];      
        unsigned char dir[4];
        unsigned char speed[4];

};

struct player_t { 
 		unsigned char x[10];
			unsigned char y[10];
        
       //unsigned char kick_or_move; // 1 = kick, 0 = movement.
        unsigned char dir[4];
        unsigned char speed[4];
};

int val(unsigned char y[10])
{
return (y[0]*512 + y[1]*256 + y[2]*128 + y[3]*64 + y[4]*32 + y[5]*16 + y[6]*8 + y[7]*4 + y[8]*2 + y[9])  ;
}

//Physical rules:

//get a randome position in the center circle
int random_num(int *x, int *y){
	while(1){
	x=rand()%100 + 1;
	y=rand()%100 + 1;
	if (in_circle((int)x+DISPLAY_COLUMNS/2-50,(int)y+DISPLAY_ROWS/2-50)) return;
	}
}

//check anything in the field
int in_border(int x, int y){
	if((x>=0) && (x<=600) && (y>=0) && (y<=400) && (x+y>=20) && (600-x+y>=20) && (600-x+400-y>=20) && (x+400-y>=20)) return 1;
	return 0;
  	
}
//check the player in the field
int in_border_player(int x, int y){
	if((x>=7) && (x<=600-7) && (y>=7) && (y<=400-7) && (x+y>=30) && (600-x+y>=30) && (600-x+400-y>=30) && (x+400-y>=30)) return 1;
	return 0;
  	
}

//check the ball in the field
int in_border_ball(int x, int y){
	if((x>=5) && (x<=600-5) && (y>=5) && (y<=400-5) && (x+y>=28) && (600-x+y>=28) && (600-x+400-y>=28) && (x+400-y>=28)) return 1;	return 0;
  	
}

float distance_sq(struct player_t player,struct ball_t ball){
	return ((val(player.x)-val(ball.x))*(val(player.x)-val(ball.x))+ (val(player.y)-val(ball.y))*(val(player.y)-val(ball.y)));
}

//check if the player possess the ball
int possess_ball(struct player_t player,struct ball_t ball){
	if (distance_sq(player,ball)<=225)
	return 1;
	else return 0;
}

/**if the player possess the ball, the ball will either 
 * When a moving player comes in contact with a stationary ball, 
 * the ball gets twice the speed of the player, and follows the player's direction.
 * 
 * When the moving ball comes in contact with a stationary or moving player, 
 * the perpendicular speed of the ball is reversed
**/
int check_contact(struct player_t *player,struct ball_t *ball){
	if (possess_ball(*player,*ball)==1)
	{
	/* ERROR HERE. I NEED HELP
	if(player->speed=={0,0,0,0}){
		ball->dir[0]=1- ball->dir[0];
	}else{
		ball->speed <<= player->speed;(SHIFT OPERATOR)
		ball->dir=player->dir;
	}
	*/
	return 1;
	}
	
	return 0;
}

//Check when the ball arrive and bounce to the wall
int check_bounce(int x, int y){
	if((x==5) || (x==600-5)) return 1;
	if((y==5) || (y==400-5)) return 2;
	if((x+y==28) || (600-x+400-y==28)) return 3;
	if((600-x+y==28) || (x+400-y==28)) return 4;
return 0;
}

void bounce(struct ball_t *ball){
	switch (check_bounce(val(ball->x), val(ball->y))){
	case 1:
		//ball->speed = 8 - ball->speed;
		break;
	case 2:
		//ball->speed = 16 - ball->speed;
		break;
	case 3:
		//ball->speed = 12 - ball->speed;
		break;
	case 4:
		//ball->speed = 4 - ball->speed;
		break;
	default:	
		break;
	
			}

}


//main function

int main(){

	int Status;
	XTft_Config *TftConfigPtr;
	unsigned int *col;
	unsigned char c;

	/*
	 * Get address of the XTft_Config structure for the given device id.
	 */


	TftConfigPtr = XTft_LookupConfig(XPAR_XPS_TFT_0_DEVICE_ID);
	if (TftConfigPtr == (XTft_Config *)NULL) {
		return XST_FAILURE;
	}

	/*
	 * Initialize all the TftInstance members and fills the screen with
	 * default background color.
	 */
	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr,
				 	TftConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	
	

 
  xil_printf("  Display color ");
  xil_printf("\n");
  XTft_SetColor(&TftInstance, 0, 0);
  XTft_ClearScreen(&TftInstance);

//Draw border, center circle
Init(&TftInstance);
 
 //Waiting for data from Mailbox
//Get initialized position of players  and display
//Generate random_number posistion of ball and send back to mail box
// Change game state to "BEGINING"
//Using timer to update the screen.

  xil_printf("  TFT test completed!\r\n");
    

  
	return 0;
	
}



