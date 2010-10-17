/* io.c -- io handler using UART
 * 
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

#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"

#define UARTLITE_DEVICE_ID	XPAR_UARTLITE_0_DEVICE_ID

typedef enum
{
	TEAM_A = 0;
	TEAM_B = 1;
}
IOTeamID;

// I/O data structures definitions
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

struct IOPacketP2S_initial
{
	IOPacketP2SType packet_type : 1;
	unsigned unused : 1;
	IOTeamID teamid : 1;
	unsigned playerid : 4;
	unsigned xpos : 10;
	unsigned ypos : 10;
	unsigned unused : 5;
}

struct IOPacketP2S_update
{
	IOPacketP2SType packet_type : 1;
	IOPacketP2SKickMove kickmove : 1;
	IOTeamID teamid : 1;
	unsigned playerid : 4;
	unsigned direction : 4;
	unsigned speed : 4;
	unsigned unused : 17;
}

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
	
	XUartLite uartlite[2];
	GameState input;
	GameState output;
	
}

void initIO(IOmanager * io, IOType type, IOmanager_cb callback)
{
	int status;
	
	status = initUART(&(io->uartlite[0]), XPAR_UARTLITE_0_DEVICE_ID);
	if(status != XST_SUCCESS) io->uartlite[0] = NULL;
	
	if(type == CONTROLER)
	{
		status = initUART(&(io->uartlite[1]), XPAR_UARTLITE_0_DEVICE_ID);
		if(status != XST_SUCCESS) io->uartlite[1] = NULL;
	}
	
	
	
}

void initUART(XUartLite* uartlite, u16 device_id)
{
	int status;
	XUartLite_Initialize(uartlite, device_id);
	
	status = XUartLite_SelfTest(uartlite);
	if(status  != XST_SUCCESS)
		return XST_FAILURE;
	
	XUartLite_ResetFifos(uartlite);
	
	return XST_SUCCESS;
}

