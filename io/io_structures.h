#ifndef IO_STRUCTURES_H
#define IO_STRUCTURES_H

#include "../structures.h"

typedef void (*IOmanager_cb)(GameState*);

//
// I/O data structures definitions
//

// Player to Server Communication

typedef enum 
{
	UPDATE = 0,
	INITIAL_POSITION = 1
}
IOPacketP2SType;

typedef enum
{
	MOVEMENT = 0,
	KICK = 1
}
IOPacketP2SKickMove;

typedef struct
{
	IOPacketP2SType packet_type : 1;
	unsigned unused0 : 1;
	TeamID teamid : 1;
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
	TeamID teamid : 1;
	unsigned playerid : 4;
	unsigned direction : 4;
	unsigned speed : 4;
	unsigned unused : 17;
}
IOPacketP2S_update;

// Server to Player Communication

typedef enum
{
	INFO = 0,
	CONTROL = 1
}
IOPacketS2PType;

typedef enum 
{
	PLAYER = 1,
	BALL = 0
}
IOPacketS2PInfoType;

typedef enum 
{
	FOUL = 1,
	GOAL = 0
}
IOPacketS2PControlType;

typedef struct IOPacketS2P_info_player_s
{
	IOPacketS2PType packet_type : 1;
	TeamID teamid : 1;
	IOPacketS2PInfoType playerball : 1;
	
	unsigned playerid : 4;
	unsigned xpos : 10;
	unsigned ypos : 10;
	unsigned unused : 5;
}IOPacketS2P_info_player;

typedef struct IOPacketS2P_info_ball_s
{
	IOPacketS2PType packet_type : 1;
	unsigned unused0 : 1; //Does not use team_id bit. Does this work ?
	IOPacketS2PInfoType playerball : 1;
	
	unsigned direction : 4;
	unsigned xpos : 10;
	unsigned ypos : 10;
	unsigned speed : 4;
	unsigned unused : 1;
}IOPacketS2P_info_ball;

typedef struct IOPacketS2P_control_s
{
	IOPacketS2PType packet_type : 1;
	TeamID teamid : 1;
	IOPacketS2PControlType foulgoal : 1;
	
	unsigned unused : 29;
} IOPacketS2P_control;


#endif //IO_STRUCTURES_H
