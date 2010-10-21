#include <stdio.h>
#include <math.h>

#define PI 3.14159265
/////////////////////////////////////////////////////////////////////////////////////////
////////THOSE ARE GLOBAL
/////////////////////////////////////////////////////////////////////////////////////////
Struct Area_1
{
	int pox, poy, poX, poY;
};// Other half of the pitch
Struct Area_2
{
	int pox, poy, poX, poY;
}; // Lelft side of our half
Struct Area_3
{
	int pox, poy,poX, poY;
};  // Right side of our half
Struct Area_4
{
	int pox, poy,poX, poY;
} ; // Red area
unsigned Team; // Which team are we

int Where_Is_The_Ball();
int Calculate_Where_The_Ball_Is_Going(struct ball *ball);
float Distance2_Between_Player_And_Ball(struct player *player,struct ball *ball);
void Move_To_The_Arrival_Position();
int Go_To_The_Middle();

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Goalkeeper Strategy///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//The goalkeeper shall move on the line of the goal. 
//If the ball is outside our half then he shall stand in the middle
//If the ball is on our side then he shall stand in a place wchich is where the ball will come
//If the ball is very close to the goal first half of the red zone gaolkeep should run to kick it
Goalkeeper_Strategy(XXXXXXXXXXXXX)
{
	int Ball_Arrival_Position;
	Calculate_Where_The_Bal_Is_Going(XXXXXXXXXXXXX);
	switch (Where_Is_The_Ball())
	{
		case 1: // ball is on the half
			Go_To_The_Middle();
			break;
		case 2:
			Move_To_The_Arrival_Position(Ball_Arrival_Position);
			break;
		case 3:
			Move_To_The_Arrival_Position(Ball_Arrival_Position);
			break;
		case 4:
			if(Team==1) // we are team A
			{
					if(ball->pos_x<25)
					{
						//runt to the ball and kick it!
					}
					else // it's in the red area but far 
					{
						Move_To_The_Arrival_Position(Ball_Arrival_Position);
					}
			}
			else // we are team B
			{
				if(ball->pos_x>575)
				{
					//runt to the ball and kick it!
				}
				else // it's in the red area but far 
				{
					Move_To_The_Arrival_Position(Ball_Arrival_Position);
				}
			}
			break;
			
	}
	
}
//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////OTHER FUNCTIONS///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
int Where_Is_The_Ball()
{
	if (Area_1.pox<ball_position.x<Area_1.poX)
		if (Area_1.poy<ball_position.y<Area_1.poY)
			return 1;
	if (Area_2.pox<ball_position.x<Area_2.poX)
		if (Area_2.poy<ball_position.y<Area_2.poY)
			return 2;
	if (Area_3.pox<ball_position.x<Area_3.poX)
		if (Area_3.poy<ball_position.y<Area_3.poY)
			return 3;
	if (Area_4.pox<ball_position.x<Area_4.poX)
		if (Area_4.poy<ball_position.y<Area_4.poY)
			return 4;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
int Calculate_Where_The_Ball_Is_Going(struct ball *ball) // this function is used to calculate where the ball
{									// will hit the goal
		if ((ball->direction)==0 || (ball->direction)==8) // ball going straight at the goal 
		{
			return (ball->y_pos);
		}
		if ((ball->direction)==1 || (ball->direction)==7) // ball going in direction 1
		{
			return (ball->y_pos)*sin(22.5*PI/180)+(ball->y_pos);
		}
		if ((ball->direction)==2 || (ball->direction)==6)
		{
			return (ball->y_pos)*sin(2*22.5*PI/180)+(ball->y_pos);
		}
		if ((ball->direction)==3 || (ball->direction)==5)
		{
			return (ball->y_pos)*sin(3*22.5*PI/180)+(ball->y_pos);
		}
		if ((ball->direction)==15 || (ball->direction)==9)
		{
			return (ball->y_pos)-sin(22.5*PI/180)*(ball->y_pos);
		}
		if ((ball->direction)==14 || (ball->direction)==10)
		{
			return (ball->y_pos)-sin(2*22.5*PI/180)*(ball->y_pos);
		}
		if ((ball->direction)==15 || (ball->direction)==11)
		{
			return (ball->y_pos)-sin(3*22.5*PI/180)*(ball->y_pos);
		}
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
float Distance2_Between_Player_And_Ball(struct player *player,struct ball *ball) //calculation of the distance
{
	return (((player->x_pos)-(ball->x_pos))*((player->x_pos)-(ball->x_pos))+((player->y_pos)-(ball->y_pos))*((player->y_pos)-(ball->y_pos)));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void Move_To_The_Arrival_Position()	// function calculates how to get to the position where  
{									// the ball will come
	
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void Go_To_The_Middle()	// function moves the goalkeeper to the middle of the goalpost  
{								
	
}