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



/*DATA STRUCTURE FOR THE PLAYERS. The structure members are based on the communication protocol given to us.
* Im thinking that each player should be able to change his values, and then a specific function can be written that
* takes the structure and converts into a 32bit vector like in the communication protocol. This vector will then be
* sent to the server.
*/

typedef int SpecialEvent;
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

typedef Player Team[5]; //strange syntax but probably correct

typedef struct
{
        Ball ball;
        Team players[2];
        SpecialEvent special;
}
GameState;



// Structure to send to Mailbox

typedef struct
{
        int pos[22];
        SpecialEvent special;
}
GameMessage;

#endif
