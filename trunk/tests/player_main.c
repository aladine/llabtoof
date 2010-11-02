

#include "xmk.h"

#include <stdio.h>

//#include "xparameters.h"
#include "structures.h"
#include "io/io_player.h"

IOPlayermanager player;

GameState output;

int main(){

	xilkernel_main();

	return 0;
}

void callback(GameState* state)
//void callback(IOmanager * io, void * data)
{
	char i;
	Player * pl_in;
	Player * pl_out;
	Ball * ball;
	xil_printf("\n  Recu2   \n");


	for(i=0; i<5; i++)
	{
		pl_in = &(state->players[TEAM_A][i]);
		pl_out = &(output.players[TEAM_A][i]);

		xil_printf("\n  Joueur %d : x=%d y=%d   \n", i, pl_in->x_pos, pl_in->y_pos);
		pl_out->x_pos = pl_in->x_pos;
		pl_out->y_pos = pl_in->y_pos;
	}

	ball = &(state->ball);
	xil_printf("\n  Ball : x=%d y=%d direction=%d speed=%d \n", ball->x_pos, ball->y_pos, ball->direction, ball->speed);
}

void main_prog()
{

	//	xil_printf("\r\n  XUartLite_Send : status is %d   \r\n", status3);

	char i;
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
	output.ball.speed = 0;

	IOPlayer_init(&player, TEAM_A, callback);

	xil_printf("\n  Coucou   \n");

	while(1)
	{
		//asm("nop");
	}


}
