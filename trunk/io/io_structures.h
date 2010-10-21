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
IOPacketP2S_update = 4;

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


