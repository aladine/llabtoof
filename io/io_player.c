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
#include "io_structures.h"

void IOPlayer_send(IOmanager * io, GameState * output)
{
	if(io->type == PLAYER)
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
	else if(io->type == SERVER)
	{
	//	if(output->
	}
}


void IO_PSendInitPos(IOmanager * io, GameState* state)
{
	unsigned char i;
	IOPacketP2S_initial packet[5];
	
	for(i=0; i<5; i++)
	{
		packet[i].packet_type = INITIAL_POSITION;
		packet[i].teamid = io->team;
		packet[i].xpos = state->players[i].x_pos;
		packet[i].ypos = state->players[i].y_pos;
		
		IO_sendPacket(IOmanager * io, (void*)(&packet[i]));
	}
	
	io->started = true;
}

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
		
		packet[i].teamid = io->team;
		packet[i].playerid = i;
		packet[i].direction = state->players[i].direction;
		packet[i].speed = state->players[i].speed;
		
		IO_sendPacket(IOmanager * io, (void*)(&packet[i]));
	}
}

void IO_SSendUpdate();

