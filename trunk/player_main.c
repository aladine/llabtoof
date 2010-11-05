

#include "xmk.h"

#include <stdio.h>

#include "xparameters.h"
#include "structures.h"
#include "io/io_player.h"

#include "xintc.h"
#include "xil_exception.h"

/*
struct device_parameters
{
	u16 device_id;
	XIntc * intterupt_controller;
	u16 intterupt_id;
}
*/
IOPlayermanager player;

GameState output;

XIntc InterruptController;

int main(){

	xilkernel_main();

	return 0;
}

void callback(GameState* state)
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
	
	XIntc_Initialize(&InterruptController, XPAR_INTC_0_DEVICE_ID);
	

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
	
	

	IOPlayer_init(&player, TEAM_A, callback, &InterruptController);
	
	
	XIntc_Start(&InterruptController, XIN_REAL_MODE);
	XIntc_Enable(&InterruptController, XPAR_INTC_0_UARTLITE_1_VEC_ID);
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XIntc_InterruptHandler, &InterruptController);
	Xil_ExceptionEnable();

	xil_printf("\n  Coucou   \n");

	while(1)
	{
		//asm("nop");
	}


}
