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
 
 
 //Callback function ? Should use this when recieving data.
//void (*IOmanager_cb)(void); //Void function pointer ?
 
//typedef struct io_player_manager IOPlayermanager;

//typedef void (*IOmanager_cb)(GameState*);

void IOPlayer_init(IOmanager * io, IOmanager_cb callback );
void IO_PSendInitPos(IOmanager * io, GameState* state);
void IOPlayer_send(IOmanager * io, GameState * output);
void IO_PSendUpdate(IOmanager * io, GameState* state);


