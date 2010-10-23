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

/***********Functions to recieve packets from the controllers.************/

/*	Translates packets currently in the buffer and updates gamestate.
 *	Need a way for the server to know when to call this function ?
 *	I assume that the server uses buffer1.
 *	I assume that buffer1[0-3] contains the comm.bits from [32-0]
 * 	where bit 32 = buffer1[0][MSB] and bit 0 = buffer[3][LSB].
 *	The code can "easily" be changed if either of the assumptions are wrong.
*/
void IO_SRecieveUpdate(IOmanager * io, GameState * state)
{
	//need a check here to see if the manager is ready, i.e the buffers are ready?
	unsigned char tempchar = 0;  //I guess 0 = "00000000"
	unsigned char playerid;
	//TEAM : Check which team the player is on.
	IOTeamID teamid;
	if( (io->input_buffer1[0]) & (00100000) != 0) //bit 29 = 1 => TEAM B
	{
		teamid = TEAM_B;
	}
	else //bit 29 = 0. TEAM A. 
	{
		teamid = TEAM_A;
	}
	
	//PlayerID. Bits 28-25 in [0]
	tempchar = io->input_buffer1[0] & (00011110);
	tempchar>>1;	//0000_XXXX; I assume that playerID goes from 0-4.
	playerid = tempchar;
	
	//Check packet type
	if( (io->input_buffer1[0]) & (10000000) != 0) //bit 31 = 1 => Init.pos packet.
	{
		unsigned short int tempint = 0; //16 bits.
		unsigned short int tempint2 = 0;//16 bits
		unsigned short int xposition, yposition = 0;
		
		//Xposition. Bits 24-15. Uses bits from buffer[0-2]: Lsb[0] all of [1] and msb[2].
		tempchar = io->input_buffer1[0] & (00000001); //only LSB[0].
		tempint = tempchar; //"0000_0000_0000_000X"
		tempint<<9;			//"0000_00X0_0000_0000"
		tempint2 = io->input_buffer1[1]; //bit 23-16
		tempint2<<1;		//"0000_000X_XXXX_XXX0"
		tempint = tempint & tempint2;//only need LSB now which is MSB from [2].
		tempchar = io->input_buffer1[2] & (10000000); //only MSB[2].
		tempint2 = tempchar; //0000_0000_X000_0000
		tempint2>>7;
		tempint = tempint & tempint2; //"0000_00XX_XXXX_XXXX"
		xposition = tempint;
		
		//Yposition bits 14-5. 14-8 on [2]. 7-5 on [3].
		tempchar = io->input_buffer1[2] & (01111111); //All except MSB[2].
		tempint = tempchar; //"0000_0000_0XXX_XXXX"
		tempint<<3; //"0000_00XX_XXXX_X000"
		tempchar = io->input_buffer1[3] & (11100000);//"XXX0_0000"
		tempchar>>5;//"0000_0XXX"
		tempint2=tempchar;
		tempint=tempint & tempint2;//"0000_00XX_XXXX_XXXX"
		yposition = tempint;
		
		if( teamid == (io->teamid) ) //this is our player.
		{
			state->players[playerid].x_pos = xposition;
			state->players[playerid].y_pos = yposition;
		}
		else //This is an opposing player.
		{
			state->players_others[playerid].x_pos = xposition;
			state->players_others[playerid].y_pos = yposition;
		}
			
	}
	else //bit 31 = 0 => Update packet.
	{
		//Kick/Move, Bit:30 on [0].
		if( io->input_buffer1[0] & (01000000) != 0) //Bit 30 = 1 => kick
			state->kick = 1; //enough with one kick ? should've one for each player ?
		else //Bit 30 = 0 => movement
			state->kick = 0;
		
		//Direction. Bits 24-21. 24 is LSB on [0]. 23-21 on [1].
		unsigned char tempchar2;
		tempchar = io->input_buffer1[0] & (00000001); 
		tempchar<<3; //"000_X000"
		tempchar2 = io->input_buffer1[1] & (11100000); //"XXX0_0000"
		tempchar2>>5; //"0000_0XXX"
		tempchar = tempchar & tempchar2; //tempchar = direction.

		//Speed. Bits 20-17 on [1]
		tempchar2 = io->input_buffer1[1] & (00011110);
		tempchar2>>1; //thempchar2 = speed.
		
		if( teamid == (io->teamid) ) //this is our player.
		{
			state->players[playerid].direction = tempchar;
			state->players[playerid].speed = tempchar2;
		}
		else //This is an opposing player.
		{
			state->players[playerid].direction = tempchar;
			state->players[playerid].speed = tempchar2;
		}
	}
}
