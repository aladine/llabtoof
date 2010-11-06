/* io_server.c -- io convertion functions for server
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

#include "io.h"
#include "io_server.h"

#include "xparameters.h"

//
// Private functions prototypes
//

void IOServer_receive(struct io_server_callback_return * server_t, char * input);

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
	server->return_v[0].team = TEAM_A;
	server->return_v[1].team = TEAM_B;

	char i;
	for(i=0; i<5; i++)
	{
		server->input.players[TEAM_A][i].direction = 0;
		server->input.players[TEAM_A][i].speed = 0;
		server->input.players[TEAM_A][i].kick_speed = 0;
		server->input.players[TEAM_B][i].direction = 0;
		server->input.players[TEAM_B][i].speed = 0;
		server->input.players[TEAM_B][i].kick_speed = 0;
	}

	IO_init(&(server->io[0]), XPAR_UARTLITE_1_DEVICE_ID, interrupt_controller, XPAR_INTC_0_UARTLITE_1_VEC_ID, (IO_cb)IOServer_receive, (void*)(&(server->return_v[0])));	//init IO for team A
	IO_init(&(server->io[1]), XPAR_UARTLITE_2_DEVICE_ID, interrupt_controller, XPAR_INTC_0_UARTLITE_2_VEC_ID, (IO_cb)IOServer_receive, (void*)(&(server->return_v[1])));	//init IO for team B
	server->callback = callback;

	server->started = 0;
	server->received[0] = 0;
	server->received[1] = 5;
	
	xil_printf("\n  server_init finished.    \n");

}

void IOServer_receive(struct io_server_callback_return * server_t, char * input)
{





	IOServermanager * server = server_t->server;
	TeamID team = server_t->team;

	//Here convert packet (input) to structure (server->input)
	
	if(input[0] & 0x80) //bit 31 = 1 => initial packet, 0x80 = 0b10000000
	{
		//cast "raw" packet to the appropriate structure.
		//xil_printf("Packet is an initial packet.");
		
		IOPacketP2S_initial * packet = (IOPacketP2S_initial *) input;
		Player * player = &(server->input.players[team][packet->playerid]);

		player->x_pos = packet->xpos;
		player->y_pos = packet->ypos;

		server->received[team]++;

		if(server->received[0] == 5 && server->received[1] == 5)
			server->started = 1;

	}
	else
	{	//cast "raw" packet to the appropriate structure.
		IOPacketP2S_update * packet = (IOPacketP2S_update *) input;
		Player * player = &(server->input.players[team][packet->playerid]);

		if(input[0] & 0x40) //bit 30 = 1 => kick
		{
			player->kick_speed = packet->speed;
			player->kick_direction = packet->direction;
		}
		else //bit 30 = 0 => movement
		{
			player->speed = packet->speed;
			player->direction = packet->direction;
			player->kick_speed = 0;
		}

		server->received[team]++;
	}


	if(server->started && server->received[0] == 5 && server->received[1] == 5)
	{
		server->callback(&(server->input));
		server->received[0] = 0;
		server->received[1] = 5;
	}
}


void IOServer_send(IOServermanager * server, GameState * output)
{
	//check if initialization is finished. How does the IO get r1 & r2 ?
	// I think r1 & r2 can be set to true in the recieving function for the server.
	if(!server->started) return;

	//run update, either info or control.

	if (output->special == EVENT_NONE)
		IOServer_SendControl(server, output);
	else
		IOServer_SendInfo(server, output);
}

void IOServer_sendPacket(IOServermanager * server, void * output)
{
	IO_send(&(server->io[0]), output );
	IO_send(&(server->io[1]), output );
}

//Converts GameState into control packet and sends to the player using the UART.
void IOServer_SendControl(IOServermanager * server, GameState * state)
{
	IOPacketS2P_control packet;

	//TODO : update the struture to have the team inormation
	packet.packet_type = CONTROL;
	packet.teamid = TEAM_A;
	packet.foulgoal = state->special;

	IOServer_sendPacket(server, (void*)(&packet));
}


void IOServer_sendInfoPlayers(IOServermanager * server, GameState * state, TeamID team)
{
	IOPacketS2P_info_player player_packet;
	Player * tmp_player;
	char i;

	//First send the players.
	for(i=0; i<5; i++)
	{
		tmp_player = &(state->players[team][i]);

		player_packet.packet_type = INFO;
		player_packet.playerball = PLAYER;
		player_packet.teamid = team;		//does io currently have a team member?
		player_packet.playerid = i;			// I think this will be useful on the recieving end ? No perv :P
		player_packet.xpos = tmp_player->x_pos;
		player_packet.ypos = tmp_player->y_pos;

		IOServer_sendPacket(server, (void*)(&player_packet));
	}
}

//Converts GameState into info packet  and sends to the player using the UART.
//Could divide this into two functions. One for player and one for ball.
void IOServer_SendInfo(IOServermanager * server, GameState * state)
{
	unsigned char i;
	IOPacketS2P_info_ball ball_packet;

	IOServer_sendInfoPlayers(server, state, TEAM_A);
	IOServer_sendInfoPlayers(server, state, TEAM_B);

	//Then send the ball.
	ball_packet.packet_type = INFO;
	ball_packet.playerball = BALL;
	ball_packet.direction = state->ball.direction;
	ball_packet.speed = state->ball.speed;
	ball_packet.xpos = state->ball.x_pos;
	ball_packet.ypos = state->ball.y_pos;

	IOServer_sendPacket(server, (void*)(&ball_packet));
}
