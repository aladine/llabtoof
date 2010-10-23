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

#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"
#include "xmk.h"
#include <pthread.h>
#include "io.h"
#include "io_structures.h"
#include "io_player.h"

/*	void IOPlayer_send(IOmanager * io, GameState * output) : Checks if the io is of type player or server. 
 *	If player : Checks if the player should send initialization package or update package to server. If initpackage it runs IO_PSendInitPos, 
 *	if updatepackage it calls IO_PSendUpdate.
 *	If server : NOT SURE LOL ? Possibly convert the other way around, that is recieve data from the server. Translate from package to structure.
*/
void IOPlayer_send(IOmanager * io, GameState * output)
{
	if(io->type == CONTROLLER)
	{
		if(!io->started)
		{
			//We are a player and we have to send initial position packets
			IO_PSendInitPos(io, output);
		}
		else
		{
			IO_PSendUpdate(io, output);
		}
	}
	else if(io->type == SERVER) //Dont see the need for this since server has its own file now. ?
	{
	//	if(output->
	}
}

//*****************Functions that send packets from the controller to server.****************//
// Sends the initial position packet from the controller.
void IO_PSendInitPos(IOmanager * io, GameState* state)
{
	unsigned char i;
	IOPacketP2S_initial packet[5];
	
	for(i=0; i<5; i++)
	{
		packet[i].packet_type = INITIAL_POSITION;
		packet[i].teamid = io->teamid;
		packet[i].xpos = state->players[i].x_pos;
		packet[i].ypos = state->players[i].y_pos;
		
		IO_send(io, (void*)(&packet[i]));
	}
	
	io->started = TRUE1; //there is no bool but true/false is occpupied so TRIIICK.
}
//Sends the update packet from the controller to the the server.
void IO_PSendUpdate(IOmanager * io, GameState* state)
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
		
		IO_send(io, (void*)(&packet[i])); //passes the address of io like this ?
	}
}
//****************Functions that recieve packets from the server***************//
void IO_PRecieveUpdate(IOmanager * io, GameState * state) //Where can this function go to read buffers and shit ?
{
	
}
void IO_SSendUpdate();

