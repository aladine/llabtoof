/* io_server.c -- io convertion functions for server
 * 
 * Copyright 2010 Per Kristian Kjoell <a0075664@nus.edu.sg>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * 		http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"
#include "xmk.h"
#include <pthread.h>
#include "io.h"
#include "io_structures.h"
#include "io_server.h"



void IOServer_send(IOmanager * io, GameState * output)
{
	//check if initialization is finished. How does the IO get r1 & r2 ?
	// I think r1 & r2 can be set to true in the recieving function for the server.
	if(io->r1 && io->r2) //we have recieved init from both teams. Ready to proceed.
	{	//run update, either info or control.
		if (output->infocontrol == CONTROL)
			IOServer_SendControl(io, output);
		else if (output->infocontrol == INFO)
			IOServer_SendInfo(io, output);
	}
	//else //initbits have not been recieved. Should wait.
		//wait for initialization. How should this be implemented ? Wait for a signal or something.
}

//Converts GameState into control packet and sends to the player using the UART.
void IOServer_SendControl(IOmanager * io, GameState * state){
	IOPacketS2P_control packet;
	packet.packet_type = CONTROL;
	packet.teamid = io->teamid;
	if(state->special = FOUL)
		packet.foulgoal = FOUL;
	else if (state->special = GOAL)//it's a goal.
		packet.foulgoal = GOAL;
	
	IO_send(io, (void*)(&packet)); //Don't think packet should be a pointer here, its local... ?
}

//Converts GameState into info packet  and sends to the player using the UART.
//Could divide this into two functions. One for player and one for ball.
void IOServer_SendInfo(IOmanager * io, GameState * state){
	unsigned char i;
	IOPacketS2P_info_ball ball_packet;
	IOPacketS2P_info_player player_packet[5];
	//First send the players.
	for(i=0; i<5; i++)
	{
		player_packet[i].packet_type = INFO;
		player_packet[i].playerball = PLAYER;
		player_packet[i].teamid = io->teamid; //does io currently have a team member?
		player_packet[i].playerid = i; // I think this will be useful on the recieving end ? No perv :P
		player_packet[i].xpos = state->players[i].x_pos;
		player_packet[i].ypos = state->players[i].y_pos;
		//do I have to set unused to 0 ? Do the UART set zeroes there cause of the type ?
		IO_send(io, (void*)(&player_packet[i]) ); //player_packet pointer?
	}
	//Then send the ball.
	ball_packet.packet_type = INFO;
	ball_packet.playerball = BALL;
	ball_packet.direction = state->ball_state.direction;
	ball_packet.speed = state->ball_state.speed;
	ball_packet.xpos = state->ball_state.x_pos;
	ball_packet.ypos = state->ball_state.y_pos;
	IO_send(io, (void*)(&ball_packet) ); //pointer to ball_packet ?
	
}

