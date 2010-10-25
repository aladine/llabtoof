/* io_server.h -- io convertion functions for server
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
 
#ifndef IO_SERVER_H
#define IO_SERVER_H

typedef struct io_server_manager IOServermanager;

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

//Callback function ? Should use this when recieving data.
//void (*IOmanager_cb)(void); //Void function pointer ?


void IOServer_init(IOServermanager * server, IOmanager_cb callback);
void IOServer_send(IOServermanager * server, GameState * output);


#endif //IO_SERVER_H
