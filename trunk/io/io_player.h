/* io_player.h -- io convertion functions for player
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

#ifndef IO_PLAYER_H
#define IO_PLAYER_H

#include "io_structures.h"
#include "io.h"

typedef struct
{
	IOmanager io;
	IOmanager_cb callback;

	GameState input;		// I/O GameStates

	TeamID team;
	bool started;
	
	char received;
}
IOPlayermanager;

/**
 * IOPlayer_init : init the I/O stack for the player
 * io				a pointer to an empty IOPlayermanager structure
 * team			the team that the player will play
 * callback		a function pointer called when data is recieved
 */
void IOPlayer_init(IOPlayermanager * io, TeamID team, IOmanager_cb callback);


void IOPlayer_send(IOPlayermanager * io, GameState * output);

#endif //IO_PLAYER_H


