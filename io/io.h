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

#ifndef IO_H
#define IO_H

#define PACKET_SIZE 4	// 4 bytes is 32 bits

typedef struct IOmanager_s IOmanager;
typedef void (*IO_cb)(void*, char*);


typedef enum { CONTROLLER, SERVER } IOType;


/**
 * Initialise I/O manager.
 * io 			a pointer to a IOmanager structure (containing IO buffers)
 * type 		a enum IOType value (PLAYER or CONTROLER)
 * callback		function pointer to the callback function
 * 
 * The callback function must take as first arguments a GameState pointer representing the input.
 * second element is a GameState pointer to be sended back when the function will return (output).
 */
void IO_init(IOmanager * io, IO_cb callback, void* callback_arg);


/**
 * Send informations to the server (if we are player) or to the two players (if we are the server).
 * 
 */
void IO_send(IOmanager * io, void * data);


#endif //IO_H