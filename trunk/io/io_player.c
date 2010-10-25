/* io_player.c -- io convertion functions for server
 *
 * Copyright 2010 Per Kristian Kjoell <a0075664@nus.edu.sg>
 * Copyright 2010 Adrien Béraud <adrienberaud@gmail.com>
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

#include "io_player.h"


//
// Private functions prototypes
//

void IOPlayer_sendInitPos(IOPlayermanager * io, GameState* state);
void IOPlayer_sendUpdate(IOPlayermanager * io, GameState* state);

void IOPlayer_recieve(IOPlayermanager * player, void* input);
void IOPlayer_recieveInfoPlayer(IOPlayermanager * player, void* input);
void IOPlayer_recieveInfoBall(IOPlayermanager * player, void* input);
void IOPlayer_recieveControl(IOPlayermanager * player, void* input);

//
// Functions
//

void IOPlayer_init(IOPlayermanager * player, IOmanager_cb callback)
{
	IO_init(&(player->io), (IO_cb)IOPlayer_recieve, (void*)player);
	player->callback = callback;
	player->started = 0;
}

/*	void IOPlayer_send(IOmanager * io, GameState * output) : Checks if the io is of type player or server.
 *	If player : Checks if the player should send initialization package or update package to server. If initpackage it runs IO_PSendInitPos,
 *	if updatepackage it calls IO_PSendUpdate.
 *	If server : NOT SURE LOL ? Possibly convert the other way around, that is recieve data from the server. Translate from package to structure.
*/
void IOPlayer_send(IOPlayermanager * player, GameState * output)
{
	if(!player->started)
	{
		//We are a player and we have to send initial position packets
		IOPlayer_sendInitPos(player, output);
	}
	else
	{
		IOPlayer_sendUpdate(player, output);
	}
}

//*****************Functions that send packets from the controller to server.****************//
// Sends the initial position packet from the controller.
void IOPlayer_sendInitPos(IOPlayermanager * player, GameState* state)
{
	unsigned char i;
	IOPacketP2S_initial packet[5];

	for(i=0; i<5; i++)
	{
		packet[i].packet_type = INITIAL_POSITION;
		packet[i].teamid = io->teamid;
		packet[i].xpos = state->players[i].x_pos;
		packet[i].ypos = state->players[i].y_pos;

		IO_send(&(player->io), (void*)(&packet[i]));
	}

	player->started = 1; //there is no bool but true/false is occpupied so TRIIICK.
}

//Sends the update packet from the controller to the the server.
void IOPlayer_sendUpdate(IOPlayermanager * player, GameState* state)
{
	unsigned char i;
	IOPacketP2S_update packet[5];

	for(i=0; i<5; i++)
	{
		packet[i].packet_type = UPDATE;

		if(i == state->kick)
			packet[i].kickmove = KICK;
		else
			packet[i].kickmove = MOVEMENT;

		packet[i].teamid = io->teamid;
		packet[i].playerid = i;
		packet[i].direction = state->players[i].direction;
		packet[i].speed = state->players[i].speed;

		IO_send(&(player->io), (void*)(&packet[i])); //passes the address of io like this ?
	}
}


/****************Functions that recieve packets from the server***************/


void IOPlayer_recieve(IOPlayermanager * player, char* input)
{
	//Here convert packet (input) to sructure (player->input)

	//Determinate packet type
	char infocontrol = (input[0] & 0x80)?1:0;	// 0x80 = 0b10000000

	if(infocontrol == INFO)
	{
		char playerball = (input[0] & 0x20)?1:0;	// 0x20 = 0b00100000

		if(playerball == PLAYER)
			IOPlayer_recieveInfoPlayer(player, input);
		else
			IOPlayer_recieveInfoBall(player, input);
	}
	else
		IOPlayer_recieveControl(player, input);


	player->callback(player->input);
}

void IOPlayer_recieveInfoPlayer(IOPlayermanager * player, void* input)
{
	//cast "raw" packet to the appropriate structure
	IOPacketS2P_info_player * packet = (IOPacketS2P_info_player *) input;

	IOTeamID team = packet->teamid;
	Player * f_player = &(player->input->players[team][packet->playerid]);

	f_player->x_pos = packet->xpos;
	f_player->y_pos = packet->ypos;
}

void IOPlayer_recieveInfoBall(IOPlayermanager * player, void* input)
{
	//cast "raw" packet to the appropriate structure
	IOPacketS2P_info_ball * packet = (IOPacketS2P_info_ball *) input;

	Ball * ball = &(player->input->ball);

	ball->x_pos = packet->xpos;
	ball->y_pos = packet->ypos;
	ball->direction = packet->direction;
	ball->speed = packet->speed;
}

void IOPlayer_recieveControl(IOPlayermanager * player, void* input)
{
	//cast "raw" packet to the appropriate structure
	IOPacketS2P_control* packet = (IOPacketS2P_control *)input;

	player->input->special = packet->foulgoal;
}


/*	Translates packets currently in the buffer and updates gamestate.
 *	Need a way for the controller to know when to call this function ?
 *	I assume that the player/controller uses buffer0.
 *	I assume that buffer1[0-3] contains the comm.bits from [32-0]
 * 	where bit 32 = buffer1[0][MSB] and bit 0 = buffer[3][LSB].
 *	The code can "easily" be changed if either of the assumptions are wrong.
*/
/*
void IO_PRecieveUpdate(IOmanager * io, GameState * state)
{
	//a check here to see if the manager is ready, i.e the buffers are ready ?
	//Input buffer0[0] contains, according to my assumptions, bits 31-24 in the packet.
	//first check if the packet is info/ctrl
	if( (io->input_buffer0[0]) & (10000000) != 0) //bit 31 = 1 => control.
	{
		state->infocontrol = CONTROL;

		//if( (io->input_buffer0[0]) & (01000000) != 0) //bit 30 = 1 => TEAM B caused the event.
			//currently nowhere in structure to indicate this? needs discussion
		//else //bit 30 = 0. TEAM A caused the event.
			//currently nowhere in structure to indicate this? needs discussion ?

		if( (io->input_buffer0[0]) & (00100000) != 0) //bit 29 = 1 => FOUL
			state->special = FOUL;
		else //bit 29 = 0 => GOAL (woho)
			state->special = GOAL;
	}
	else //bit 31 = 0 => info.
	{
		unsigned char tempchar = 0;  //I guess 0 = "00000000"
		unsigned char playerid;
		unsigned short int tempint = 0; //16 bits.
		unsigned short int tempint2 = 0;//16 bits
		unsigned short int xposition, yposition = 0;
		state->infocontrol = INFO;
		//checks if packet contains info about the ball or a player.
		if( (io->input_buffer0[0]) & (00100000) != 0) //bit 29 = 1 => BALL INFO
		{
			//Direction
			tempchar = io->input_buffer0[0] & (00011110); //bits 28-25
			state->ball_state.direction = tempchar; //I assume that ball_state.direction will only use the 4 LSB ?

			//Xposition. Bits 24-15. Uses bits from buffer[0-2]: Lsb[0] all of [1] and msb[2].
			tempchar = io->input_buffer0[0] & (00000001); //only LSB[0].
			tempint = tempchar; //"0000_0000_0000_000X"
			tempint<<9;			//"0000_00X0_0000_0000"
			tempint2 = io->input_buffer0[1]; //bit 23-16
			tempint2<<1;		//"0000_000X_XXXX_XXX0"
			tempint = tempint & tempint2;//only need LSB now which is MSB from [2].
			tempchar = io->input_buffer0[2] & (10000000); //only MSB[2].
			tempint2 = tempchar; //0000_0000_X000_0000
			tempint2>>7;
			tempint = tempint & tempint2; //"0000_00XX_XXXX_XXXX"
			state->ball_state.x_pos = tempint; //Finally done.

			//Yposition bits 14-5. 14-8 on [2]. 7-5 on [3].
			tempchar = io->input_buffer0[2] & (01111111); //All except MSB[2].
			tempint = tempchar; //"0000_0000_0XXX_XXXX"
			tempint<<3; //"0000_00XX_XXXX_X000"
			tempchar = io->input_buffer0[3] & (11100000);//"XXX0_0000"
			tempchar>>5;//"0000_0XXX"
			tempint2=tempchar;
			tempint=tempint & tempint2;//"0000_00XX_XXXX_XXXX"
			state->ball_state.y_pos = tempint;


			//Speed bits 4-1 on [3].
			tempchar = io->input_buffer0[3] & (00011110);
			tempchar>>1;	//"0000_XXXX"
			state->ball_state.speed = tempchar;


		}
		else //bit 29 = 0 => PLAYER (woho)
		{
			//TEAM : Check which team the player is on.
			IOTeamID teamid;
			if( (io->input_buffer0[0]) & (01000000) != 0) //bit 30 = 1 => TEAM B
			{
				//currently nowhere in structure to indicate this? needs discussion
				teamid = TEAM_B;
			}
			else //bit 30 = 0. TEAM A.
			{
				//I guess this could be a player to be put in player_others. But how to indicate? Add a teamid to the struct ?
				//currently nowhere in structure to indicate this? needs discussion ?
				teamid = TEAM_A;
			}

			//Xposition. Bits 24-15. Uses bits from buffer[0-2]: Lsb[0] all of [1] and msb[2].
			tempchar = io->input_buffer0[0] & (00000001); //only LSB[0].
			tempint = tempchar; //"0000_0000_0000_000X"
			tempint<<9;			//"0000_00X0_0000_0000"
			tempint2 = io->input_buffer0[1]; //bit 23-16
			tempint2<<1;		//"0000_000X_XXXX_XXX0"
			tempint = tempint & tempint2;//only need LSB now which is MSB from [2].
			tempchar = io->input_buffer0[2] & (10000000); //only MSB[2].
			tempint2 = tempchar; //0000_0000_X000_0000
			tempint2>>7;
			tempint = tempint & tempint2; //"0000_00XX_XXXX_XXXX"
			xposition = tempint;

			//Yposition bits 14-5. 14-8 on [2]. 7-5 on [3].
			tempchar = io->input_buffer0[2] & (01111111); //All except MSB[2].
			tempint = tempchar; //"0000_0000_0XXX_XXXX"
			tempint<<3; //"0000_00XX_XXXX_X000"
			tempchar = io->input_buffer0[3] & (11100000);//"XXX0_0000"
			tempchar>>5;//"0000_0XXX"
			tempint2=tempchar;
			tempint=tempint & tempint2;//"0000_00XX_XXXX_XXXX"
			yposition = tempint;

			//PlayerID. Bits 28-25 in [0]
			tempchar = io->input_buffer0[0] & (00011110);
			tempchar>>1;	//0000_XXXX; I assume that playerID goes from 0-4.
			playerid = tempchar;

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
	}
}
*/

