/* io.c -- io handler using UART
 * 
 * Copyright 2010 Adrien Beraud <adrienberaud@gmail.com>
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

#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"

typedef enum
{
	TEAM_A = 0;
	TEAM_B = 1;
}
IOTeamID;

//
// I/O data structures definitions
//

// Player to Server Communication

typedef enum 
{
	UPDATE = 0;
	INITIAL_POSITION = 1;
}
IOPacketP2SType;

typedef enum
{
	MOVEMENT = 0;
	KICK = 1;
}
IOPacketP2SKickMove;

typedef struct
{
	IOPacketP2SType packet_type : 1;
	unsigned unused : 1;
	IOTeamID teamid : 1;
	unsigned playerid : 4;
	unsigned xpos : 10;
	unsigned ypos : 10;
	unsigned unused : 5;
}
IOPacketP2S_initial;

typedef struct
{
	IOPacketP2SType packet_type : 1;
	IOPacketP2SKickMove kickmove : 1;
	IOTeamID teamid : 1;
	unsigned playerid : 4;
	unsigned direction : 4;
	unsigned speed : 4;
	unsigned unused : 17;
}
IOPacketP2S_update;

// Server to Player Communication

typedef enum 
{
	INFO = 0;
	CONTROL = 1;
}
IOPacketS2PType;

typedef enum 
{
	PLAYER = 0;
	BALL = 1;
}
IOPacketS2PInfoType;

typedef enum 
{
	FOUL = 1;
	GOAL = 0;
}
IOPacketS2PControlType;

struct IOPacketS2P_info_player
{
	IOPacketS2PType packet_type : 1;
	IOTeamID teamid : 1;
	IOPacketS2PInfoType playerball : 1;
	
	unsigned playerid : 4;
	unsigned direction : 4;
	unsigned speed : 4;
	unsigned unused : 17;
}

struct IOPacketS2P_info_ball
{
	IOPacketS2PType packet_type : 1;
	IOTeamID teamid : 1;
	IOPacketS2PInfoType playerball : 1;

	unsigned direction : 4;
	unsigned xpos : 10;
	unsigned ypos : 10;
	unsigned speed : 4;
	unsigned unused : 1;
}

struct IOPacketS2P_control
{
	IOPacketS2PType packet_type : 1;
	IOTeamID teamid : 1;
	IOPacketS2PControlType foulgoal : 1;
	
	unsigned unused : 29;
}


struct IOmanager_s
{
	IOType type;				//Are we a player or the server ??
	XUartLite uartlite[2];	//UARTLite instances (just one is used if we're a player)
	IOmanager_cb callback;	//Callback function called when data is received
	GameState input;			//I/O GameStates
	GameState output;
	
	bool started;				//Player : did we sended the start position packets ?
									//Server : did we received the two start position packets ?
	bool r1, r2;				//Used by the server : did we received the infos from teams A&B ?
}

//
// Private functions prototypes
//

void IO_initUART(XUartLite* uartlite, u16 device_id);

void IO_PSendInitPos(IOmanager * io, GameState* state);

//
// Functions
//

void initIO(IOmanager * io, IOType type, IOmanager_cb callback)
{
	int status;
	
	io->type = type;
	io->callback = callback;
	
	io->started = false;
	
	status = IO_initUART(&(io->uartlite[0]), XPAR_UARTLITE_1_DEVICE_ID);
	if(status != XST_SUCCESS) io->uartlite[0] = NULL;
	
	if(type == SERVER)
	{
		status = IO_initUART(&(io->uartlite[1]), XPAR_UARTLITE_2_DEVICE_ID);
		if(status != XST_SUCCESS) io->uartlite[1] = NULL;
	}
	
}

void IO_initUART(XUartLite* uartlite, u16 device_id)
{
	int status;
	XUartLite_Initialize(uartlite, device_id);
	
	status = XUartLite_SelfTest(uartlite);
	if(status  != XST_SUCCESS)
		return XST_FAILURE;
	
	XUartLite_ResetFifos(uartlite);
	
	return XST_SUCCESS;
}

void IO_send(IOmanager * io, GameState * output)
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

