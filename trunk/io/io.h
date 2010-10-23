/* io.h -- io handler using UART
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
 
#include "structures.h"
#define PACKET_SIZE 4	// 4 bytes is 32 bits
#include "xmk.h"
#include <pthread.h>

#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"
typedef struct IOmanager_s IOmanager;
typedef void (*IOmanager_cb)(GameState*);



typedef enum { CONTROLLER, SERVER } IOType;
/**
 * Send informations to the server (if we are player) or to the two players (if we are the server).
 * 
 */
void IO_send(IOmanager * io, void * data);

struct IOmanager_s
{
	bool init;
	IOType type;			// Are we a player or the server ??
	XUartLite uartlite[2];	// UARTLite instances (just one is used if we're a player)
	IOmanager_cb callback;	// Callback function called when data is received
	
	char input_buffer0[PACKET_SIZE];
	char input_buffer1[PACKET_SIZE];
//	char input_count[2];
	
	pthread_t input_t;		// Pooling thread (checking for new stuff on input);
	
	GameState input;		// I/O GameStates
	GameState output;
	IOTeamID teamid;
	bool started;			// Player : did we sended the start position packets ?
							// Server : did we received the two start position packets ?
	bool r1, r2;			// Used by the server : did we received the infos from teams A&B ?*/
};

/**
 * Initialise I/O manager.
 * io 			a pointer to a IOmanager structure (containing IO buffers)
 * type 		a enum IOType value (PLAYER or CONTROLER)
 * callback		function pointer to the callback function
 * 
 * The callback function must take as first arguments a GameState pointer representing the input.
 * second element is a GameState pointer to be sended back when the function will return (output).
 */
void IO_init(IOmanager * io, IOType type, IOmanager_cb callback);



