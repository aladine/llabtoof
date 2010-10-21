#ifndef CONTROLLING_H
#define CONTROLLING_H

//#include "math.h" 
//#include "xio.h"
// include : msgqueue, xio, xuartlite, other utilities
// include : structure.h : below 

#define NO_KICK -1
// Message Queue Max receive every refresh
#define MAX_MESSAGE_COUNT 10
// Timeslot every refresh
#define TIME_SLOT (1/40)
// Poportional Constant
#define CONST_PROP 100
// Display Constant
#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define BLUE 0x000000ff


struct player
{
	unsigned x_pos, y_pos;
	unsigned char direction;
	unsigned char speed;
	unsigned char kick;
};

struct ball
{
	unsigned  x_pos, y_pos;
	unsigned char direction;
	unsigned char speed;
};

struct game_state
{
	struct ball ball_state;
};

struct uartdata
{
    unsigned char data[4];
};

#endif
