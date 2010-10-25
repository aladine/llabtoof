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
 
#include "io.h"
#include "io_server.h"

//structure used to differentiate the two IOmanagers in the callback function
struct io_server_callback_return
{
	IOServermanager * server;
	IOTeamID team;
};

struct io_server_manager
{
	IOmanager io[2];
	IOmanager_cb callback;
	
	GameState input;		// I/O GameStates
	GameState output;
	
	bool started;
	bool r1, r2;
	
	struct io_server_callback_return return_v[2];
};

//
// Private functions prototypes
//

void IOServer_receive(IOServermanager * server, void * input);

void IOServer_sendPacket(IOServermanager * server, void * output);
void IOServer_SendControl(IOServermanager * server, GameState * state);
void IOServer_SendInfo(IOServermanager * server, GameState * state);

//
// Functions
//

void IOServer_init(IOServermanager * server, IOmanager_cb callback)
{
	server->return_v[0].server = server;
	server->return_v[1].server = server;
	server->return_v[0].teamid = TEAM_A;
	server->return_v[1].teamid = TEAM_B;
	
	IO_init(&(server->io[0]), (IO_cb)IOServer_receive, (void*)(&(server->return_v[0])));	//init IO for team A
	IO_init(&(server->io[1]), (IO_cb)IOServer_receive, (void*)(&(server->return_v[1])));	//init IO for team B
	server->callback = callback;
	
	server->started = 0;
	server->r1 = 0; //TODO, TEAM_A initialized?
	server->r2 = 0; //TEAM_B initialized?
}

void IOServer_receive(struct io_server_callback_return * server_t, void * input)
{
	IOServermanager * server = server_t->server;
	IOTeamID team = server_t->team;
	
	//Here convert packet (input) to structure (server->input)
	//Need to check for inital packet or update packet ?
	if(input[0] & (10000000) != 0) //bit 31 = 1 => initial packet
	{
		//cast "raw" packet to the appropriate structure.
		IOPacketP2S_initial * packet = (IOPacketP2S_initial *) input;
		server->input.players[team][packet->playerid].x_pos = packet.xpos;
		server->input.players[team][packet->playerid].y_pos = packet.ypos;
		if(team = TEAM_A)
			server->r1 = 1; //TODO :init
		else if(team == TEAM_B)
			server->r2 = 1;
	}
	else //bit 31 = 0 => update packet.
		//cast "raw" packet to the appropriate structure.
		IOPacketP2S_update * packet = (IOPacketP2S_update *) input;
		if(input & (0x40000000) != 0) //bit 30 = 1 => kick
			server->input.players[team][packet->playerid].kick_speed = packet->speed;
			server->input.players[team][packet->playerid].kick_direction = packet->direction;
		else //bit 30 = 0 => movement
			server->input.players[team][packet->playerid].speed = packet->speed;
			server->input.players[team][packet->playerid].direction = packet->direction;
	if(server->r1 && server->r2)
		server->callback(server->input);
}


void IOServer_send(IOServermanager * server, GameState * output)
{
	//check if initialization is finished. How does the IO get r1 & r2 ?
	// I think r1 & r2 can be set to true in the recieving function for the server.
	if(server->r1 && server->r2) //we have recieved init from both teams. Ready to proceed.
	{	//run update, either info or control.
		if (output->infocontrol == CONTROL)
			IOServer_SendControl(io, output);
		else if (output->infocontrol == INFO)
			IOServer_SendInfo(io, output);
	}
	//else //initbits have not been recieved. Should wait.
		//wait for initialization. How should this be implemented ? Wait for a signal or something.
}

void IOServer_sendPacket(IOServermanager * server, void * output)
{
	IO_send(&(server->io[0]), output );
	IO_send(&(server->io[1]), output );
}

//Converts GameState into control packet and sends to the player using the UART.
void IOServer_SendControl(IOmanager * io, GameState * state)
{
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
void IOServer_SendInfo(IOServermanager * server, GameState * state)
{
	unsigned char i;
	IOPacketS2P_info_ball ball_packet;
	IOPacketS2P_info_player player_packet;
	
	//First send the players.
	//team A
	for(i=0; i<5; i++)
	{
		player_packet.packet_type = INFO;
		player_packet.playerball = PLAYER;
		player_packet.teamid = TEAM_A;		//does io currently have a team member?
		player_packet.playerid = i;			// I think this will be useful on the recieving end ? No perv :P
		player_packet.xpos = state->players[i].x_pos;
		player_packet.ypos = state->players[i].y_pos;
		
		IOServer_sendPacket(server, (void*)(&player_packet));
	}
	
	//team B
	//TODO : team b
	
	//Then send the ball.
	ball_packet.packet_type = INFO;
	ball_packet.playerball = BALL;
	ball_packet.direction = state->ball_state.direction;
	ball_packet.speed = state->ball_state.speed;
	ball_packet.xpos = state->ball_state.x_pos;
	ball_packet.ypos = state->ball_state.y_pos;
	
	IOServer_sendPacket(server, (void*)(&ball_packet));
}

/***********Functions to recieve packets from the controllers.************/

/*	Translates packets currently in the buffer and updates gamestate.
 *	Need a way for the server to know when to call this function ?
 *	I assume that the server uses buffer1.
 *	I assume that buffer1[0-3] contains the comm.bits from [32-0]
 * 	where bit 32 = buffer1[0][MSB] and bit 0 = buffer[3][LSB].
 *	The code can "easily" be changed if either of the assumptions are wrong.
*/
/*void IO_SRecieveUpdate(IOmanager * io, GameState * state)
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
}*/
