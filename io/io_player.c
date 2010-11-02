/* io_player.c -- io convertion functions for server
 *
 * Copyright 2010 Per Kristian Kjoell <a0075664@nus.edu.sg>
 * Copyright 2010 Adrien B�raud <adrienberaud@gmail.com>
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
#include "xparameters.h"

#define PACKET_BUFFER_SIZE 11	// 5 players from each team + 1 ball = 11 packets

//
// Private functions prototypes
//

void IOPlayer_sendInitPos(IOPlayermanager * io, GameState* state);
void IOPlayer_sendUpdate(IOPlayermanager * io, GameState* state);

void IOPlayer_receive(IOPlayermanager * player, char* input);
void IOPlayer_receiveInfoPlayer(IOPlayermanager * player, char* input);
void IOPlayer_receiveInfoBall(IOPlayermanager * player, char* input);
void IOPlayer_receiveControl(IOPlayermanager * player, char* input);

//
// Functions
//

void IOPlayer_init(IOPlayermanager * player, TeamID team, IOmanager_cb callback)
{
	IO_init(&(player->io), XPAR_UARTLITE_1_DEVICE_ID, (IO_cb)IOPlayer_receive, (void*)player);

	player->callback = callback;
	player->team = team;

	player->started = 0;
	player->received = 0;

	//Initialize the input GameState buffer
	char i;
	for(i=0; i<5; i++)
	{
		player->input.players[TEAM_A][i].x_pos = 0;
		player->input.players[TEAM_A][i].y_pos = 0;
		player->input.players[TEAM_B][i].x_pos = 0;
		player->input.players[TEAM_B][i].y_pos = 0;
	}
	player->input.ball.x_pos = 0;
	player->input.ball.y_pos = 0;
	player->input.ball.direction = 0;
	player->input.ball.speed = 0;

	/*IO_send(&(player->io), "PLIN");
	IO_send(&(player->io), "IT\r\n");*/
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
	IOPacketP2S_initial packet;
	Player * player_tmp;

	for(i=0; i<5; i++)
	{
		packet.packet_type = INITIAL_POSITION;
		packet.teamid = player->team;
		player_tmp = &(state->players[player->team][i]);
		packet.xpos = player_tmp->x_pos;
		packet.ypos = player_tmp->y_pos;

		IO_send(&(player->io), (void*)(&packet));
	}

	player->started = 1; //there is no bool but true/false is occpupied so TRIIICK.
}

//Sends the update packet from the controller to the the server.
void IOPlayer_sendUpdate(IOPlayermanager * player, GameState* state)
{
	unsigned char i;
	IOPacketP2S_update packet;
	Player * player_tmp;

	for(i=0; i<5; i++)
	{
		player_tmp = &(state->players[player->team][i]);

		packet.packet_type = UPDATE;

		packet.teamid = player->team;
		packet.playerid = i;

		if(player_tmp->kick_speed == 0)
		{
			packet.kickmove = MOVEMENT;
			packet.direction = player_tmp->direction;
			packet.speed = player_tmp->speed;
		}
		else
		{
			packet.kickmove = KICK;
			packet.direction = player_tmp->kick_direction;
			packet.speed = player_tmp->kick_speed;
		}

		IO_send(&(player->io), (void*)(&packet)); //passes the address of io like this ?
	}
}


/****************Functions that recieve packets from the server***************/


void IOPlayer_receive(IOPlayermanager * player, char* input)
{
	//Determinate packet type
	char infocontrol = (input[0] & (0x80))?1:0;	// 0x80 = 0b10000000

	if(infocontrol == INFO)
	{
		char playerball = (input[0] & (0x20))?1:0;	// 0x20 = 0b00100000

		if(playerball == PLAYER)
			IOPlayer_receiveInfoPlayer(player, input);
		else //playerball = BALL
			IOPlayer_receiveInfoBall(player, input);

		player->received++;
		if(player->received == PACKET_BUFFER_SIZE)
			player->callback(&(player->input));
	}
	else //infocontrol = CONTROL
	{
		IOPlayer_receiveControl(player, input);
		player->callback(&(player->input));
	}
}

void IOPlayer_receiveInfoPlayer(IOPlayermanager * player, char* input)
{
	//cast "raw" packet to the appropriate structure
	IOPacketS2P_info_player * packet = (IOPacketS2P_info_player *) input;

	TeamID team = packet->teamid;

	xil_printf("\r\n  Recu: teamid=%d   \r\n", team);

	Player * f_player = &(player->input.players[team][packet->playerid]);

	f_player->x_pos = packet->xpos;
	f_player->y_pos = packet->ypos;
}

void IOPlayer_receiveInfoBall(IOPlayermanager * player, char* input)
{
	//cast "raw" packet to the appropriate structure
	IOPacketS2P_info_ball * packet = (IOPacketS2P_info_ball *) input;

	Ball * ball = &(player->input.ball);

	ball->x_pos = packet->xpos;
	ball->y_pos = packet->ypos;
	ball->direction = packet->direction;
	ball->speed = packet->speed;
}

void IOPlayer_receiveControl(IOPlayermanager * player, char* input)
{
	//cast "raw" packet to the appropriate structure
	IOPacketS2P_control* packet = (IOPacketS2P_control *)input;

	player->input.special = packet->foulgoal;
}

