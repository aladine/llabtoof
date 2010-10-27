/* structures.c -- main data structures for the game elements
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

/*DATA STRUCTURE FOR THE PLAYERS. The structure members are based on the communication protocol given to us.
* Im thinking that each player should be able to change his values, and then a specific function can be written that
* takes the structure and converts into a 32bit vector like in the communication protocol. This vector will then be
* sent to the server.
*/
#ifndef STRUCTURES_H
#define STRUCTURES_H

#define NO_KICK -1

typedef enum
{
	TEAM_A = 0,
	TEAM_B = 1
}
TeamID;

typedef enum
{
	EVENT_GOAL = 0,
	EVENT_FOOL = 1,
	EVENT_NONE = 2
}
SpecialEvent;

typedef struct
{
	unsigned x_pos, y_pos;
	unsigned char direction;
	unsigned char speed;
	unsigned char kick_direction; //changed :)
	unsigned char kick_speed;
}
Player;

typedef struct
{
	unsigned  x_pos, y_pos;
	unsigned char direction;
	unsigned char speed;
}
Ball;

typedef Player Team[5];	//strange syntax but probably correct

typedef struct
{
	Ball ball;
	Team players[2];
	SpecialEvent special;
}
GameState;

#endif //STRUCTURES_H
