
#include <math.h>
#include "function.h"
#include "../structures.h"
#include "../io/io_structures.h"



#define TEAM_A 0
#define TEAM_B 1
#define PI 3.141592654

/***********************************************************************************************************************************************/
///////FUNCTIONS////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
void Update_Strategy()
{
    if(Choose_Strategy()==0)
    {
        BC0=1;
		BC1=1;
		BC2=1;
		BC3=1;
		BC4=1;
		
    }
    else
    { // IF WE HAVE ENOUGH TIME WE SHOULD WRITE EXTRA STRATEGIES
        BC0=1;
		BC1=1;
		BC2=1;
		BC3=1;
		BC4=1;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
int Choose_Strategy()
{
	if(team==0)
	{
		if(ball.x<=320)
			return 0; //strategy defense
		else
			return 1; //strategy attack
	}
	else
	{
		if(ball.x>=320)
			return 0; //strategy defense
		else
			return 1; //strategy attack	
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------direction of the goalkeeper to the red area--------------------------------------------------
int Which_Direction_About_gp_To_Red_Area()
{
	int xb, yb;
	int i=0, direct=0;
	int a,b, xp=player_sent[0].x, yp=player_sent[0].y;
	
	if(team==0)
	{
		xb=25; yb=200;
	}
	else
	{
		xb=615; yb=200;
	}
	a=xb-xp;
	b=yb-yp;
	return Coor_To_Direction(a, b);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Coor_To_Direction(int a, int b)
{
	float alpha= atan2(a, b);
	if(alpha < 0) alpha += 2*PI;
	int temp = round(8*alpha/PI);
	return temp%16;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------function returning the distance of the player to the ball-------------
int Distance_Player_Ball(int id)
{
	int distance;
	distance= (ball.x-player_sent[id].x)*(ball.x-player_sent[id].x)+(ball.y-player_sent[id].y)*(ball.y-player_sent[id].y);
	distance=sqrt(distance);
    return distance;//-12;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------Pass to the stricker who has the area with the least of opponents--------
void Pass_Stricker_free_A(int id)
{
    int direct= Which_Direction_About_Player_To_Ball(id);
	
    if((0<=direct<4) || (12<direct<=15)) // ball in front the goalkeeper
    {
        if( Number_Opponent_Player_Area(3)<=Number_Opponent_Player_Area(4) )
        {
            player_up[id].action=1; //kick
            player_up[id].direction=15;
            player_up[id].speed=100; // 50px/sec
        }
        else
        {
			player_up[id].action=1; //kick
            player_up[id].direction=1;
            player_up[id].speed=100; // 50px/sec
        }
    }
    else
    {
        player_up[id].action=0; //movement
        if(12<=player_up[id].direction<16)
            player_up[id].direction=player_up[id].direction-2;
        else
            player_up[id].direction=player_up[id].direction+2;
        player_up[id].speed=50; // 50px/sec
    }
	
    Send_Update_Queue(id);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pass_Stricker_free_B(int id)
{
	int direct= Which_Direction_About_Player_To_Ball(id);
	if(4<=direct<12) // ball in front the goalkeeper
	{
		if( Number_Opponent_Player_Area(3)<=Number_Opponent_Player_Area(4) )
		{
			player_up[id].action=1; //kick
			player_up[id].direction=9;
			player_up[id].speed=100; // 50px/sec
		}
		else
		{
			player_up[id].action=1; //kick
			player_up[id].direction=7;
			player_up[id].speed=100; // 50px/sec
		}
	}
	else
	{
		player_up[id].action=0; //movement
		if(((12<=player_up[id].direction) && (player_up[id].direction<16))||((0<=player_up[id].direction) && (player_up[id].direction<4)))
			player_up[id].direction=player_up[id].direction-2;
		else
			player_up[id].direction=player_up[id].direction+2;
		player_up[id].speed=50; // 50px/sec
	}
	Send_Update_Queue(id);
}	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------direction of the player to the ball----------------------------------------
int Which_Direction_About_Player_To_Ball(int id)
{
	int direct=0;
	int a, b;
	
	a=ball.x-player_sent[id].x;
	b=ball.y-player_sent[id].y;
	
	return Coor_To_Direction(a, b);
}
//--maybe better to compare ball x and player x

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------number of opponent player in the area---------------------------------------
int Number_Opponent_Player_Area(int area)  // function to assess how many oponents
{  // are in a given area
	int number=0;
	int i;
	for (i=0; i<5; i++) // checking the position of each opponent player
	{
		if (Where_Is_The_Opponent_Player(i)==area)
		{
			number++;
		}// increase the number if there is a player in this area
	}
	return number;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------the area where is the Player------------------------------------------------
int Where_Is_The_player(int id)
{ // the function returns the number of the area in which the  player is
	if (Area_red.pox<=player_sent[id].x)
	{
		if(player_sent[id].x<=Area_red.poX)//checking if he is in area 1
		{
			if (Area_red.poy<=player_sent[id].y&&player_sent[id].y<=Area_red.poY)
			{
				
				return 0;
			}
		}
	}
	if (Area_1.pox<=player_sent[id].x)
	{
		
		if(player_sent[id].x<=Area_1.poX)//checking if he is in area 1
		{
			if (Area_1.poy<=player_sent[id].y&&player_sent[id].y<=Area_1.poY)
			{
				return 1;
			}
		}
	}
	if (Area_2.pox<=player_sent[id].x)
	{
		if(player_sent[id].x<Area_2.poX)//checking if he is in area 2
		{
			if (Area_2.poy<=player_sent[id].y&&player_sent[id].y<=Area_2.poY)
			{
				return 2;
			}
		}
	}
	if (Area_3.pox<=player_sent[id].x)
	{
		if(player_sent[id].x<=Area_3.poX)//checking if he is in area 1
		{
			if (Area_3.poy<=player_sent[id].y&&player_sent[id].y<=Area_3.poY)
			{
				return 3;
			}
		}
	}
	if (Area_4.pox<=player_sent[id].x)
	{
		if(player_sent[id].x<=Area_4.poX)//checking if he is in area 1
		{
			if (Area_4.poy<=player_sent[id].y&&player_sent[id].y<=Area_4.poY)
			{
				return 4;
			}
		}
	}
	xil_printf("Dupa");
	return 50;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Where_Is_The_Opponent_Player(int id)
{ // the function returns the number of the area in which the  player is
	if (Area_1.pox<=oplayer[id].x)
	{
		
		if(oplayer[id].x<=Area_1.poX)//checking if he is in area 1
		{
			
			if (Area_1.poy<=oplayer[id].y)
			{
				if(oplayer[id].y<=Area_1.poY)
				{
					return 1;
				}
			}
		}
	}
	
	if (Area_2.pox<=oplayer[id].x)
	{
		if(oplayer[id].x<=Area_2.poX)//checking if he is in area 2
		{
			if (Area_2.poy<=oplayer[id].y)
			{
				if(oplayer[id].y<=Area_2.poY)
				{
					return 2;
				}
			}
		}
	}
	if (Area_3.pox<=oplayer[id].x)
	{
		if(oplayer[id].x<=Area_3.poX)//checking if he is in area 1
		{
			if (Area_3.poy<=oplayer[id].y)
			{
				if(oplayer[id].y<=Area_3.poY)
				{
					return 3;
				}
			}
		}
	}
	if (Area_4.pox<=oplayer[id].x)
	{
		if(oplayer[id].x<=Area_4.poX)//checking if he is in area 1
		{
			if (Area_4.poy<=oplayer[id].y)
			{
				if(oplayer[id].y<=Area_4.poY)
				{
					return 4;
				}
			}
		}
	}
	xil_printf("Error");
	return 50;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Send_Update_Queue(int id) 
{
	//int ret;
	if(player_up[id].action==0)
	{
		New_State.players[team][id].direction=player_up[id].direction;
		New_State.players[team][id].speed=player_up[id].speed;
		F0++;
	}
	else
	{
		New_State.players[team][id].kick_speed=player_up[id].speed;	
		New_State.players[team][id].kick_direction=player_up[id].direction;
		F0++;
	}
	
	
	if(F0==5)
	{	
		F0=0;
		if(team==0)
			IOPlayer_send(&io_stuff, &New_State);
		else
			IOPlayer_send(&io_stuff, &New_State);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Shoot(int id)
{
	player_up[id].direction=Which_Direction_About_Ball_To_Goal(id);
	player_up[id].action=1;
	player_up[id].speed=100;
	Send_Update_Queue(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Which_Player_In_My_Area(int My_Area) // function returns the number of the player that is in my area 
{
	int Which_Player;
	int i;
	for(i=0;i<5;i++)
	{
		if(Where_Is_The_Opponent_Player(i)==My_Area)
		{
			Which_Player=i;
		}
	}
	return Which_Player;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int How_Many_Opponents_In_The_Other_Area(int My_Area)
{
	
	if(My_Area==3)
	{
		return Number_Opponent_Player_Area(4);
	}
	else if(My_Area==4)
	{
		return Number_Opponent_Player_Area(3);
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pass_To_The_Other_Striker(int id)
{
	player_up[id].action=1;
	player_up[id].speed=30;
	if(id==3)
	{
		player_up[id].direction=Which_Direction_About_Player_To_Player(3, 4);
	}
	else
	{
		player_up[id].direction=Which_Direction_About_Player_To_Player(4, 3);
	}
	Send_Update_Queue(id);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Where_Is_The_Ball()
{
	if (Area_1.pox<=ball.x)
	{
		
		if(ball.x<=Area_1.poX)//checking if he is in area 1
		{
			
			if (Area_1.poy<=ball.y)
			{
				if(ball.y<=Area_1.poY)
				{
					return 1;
				}
			}
		}
	}
	
	if (Area_2.pox<=ball.x)
	{
		if(ball.x<=Area_2.poX)//checking if he is in area 2
		{
			if (Area_2.poy<=ball.y)
			{
				if(ball.y<=Area_2.poY)
				{
					return 2;
				}
			}
		}
	}
	if (Area_3.pox<=ball.x)
	{
		if(ball.x<=Area_3.poX)//checking if he is in area 1
		{
			if (Area_3.poy<=ball.y)
			{
				if(ball.y<=Area_3.poY)
				{
					return 3;
				}
			}
		}
	}
	if (Area_4.pox<=ball.x)
	{
		if(ball.x<=Area_4.poX)//checking if he is in area 1
		{
			if (Area_4.poy<=ball.y)
			{
				if(ball.y<=Area_4.poY)
				{
					return 4;
				}
			}
		}
	}
}

void Run_Average(int id)
{

    player_up[id].action=1;

    if(team==0)

        player_up[id].direction=0;

    else

        player_up[id].direction=8;

    player_up[id].speed=15;

    Send_Update_Queue(id);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART B/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART B/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART B/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------
void Construct_Initial_Packet_Player0(int team)
{
	if(team==0)
	{
		player[0].x=20;
		player[0].y=200;
		player[0].id=0;
		player[0].team=1;
		
	}
	else
	{
		player[0].x=620;
		player[0].y=200;
		player[0].id=0;
		player[0].team=0;
	}
	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player1(int team)
{
	if(team==0)
	{
		player[1].x=170;
		player[1].y=100;
		player[1].id=1;
		player[1].team=1;
		
	}
	else
	{
		player[1].x=470;
		player[1].y=100;
		player[1].id=1;
		player[1].team=0;
	}
	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player2(int team)
{
	if(team==0)
	{
		player[2].x=170;
		player[2].y=300;
		player[2].id=2;
		player[2].team=1;
		
	}
	else
	{
		player[2].x=470;
		player[2].y=300;
		player[2].id=2;
		player[2].team=0;
	}
	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player3(int team)
{
	if(team==0)
	{
		player[3].x=270;
		player[3].y=200;
		player[3].id=3;
		player[3].team=1;
		
	}
	else
	{
		player[3].x=370;
		player[3].y=200;
		player[3].id=3;
		player[3].team=0;
	}
	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player4(int team)
{
	if(team==0)
	{
		player[4].x=370;
		player[4].y=200;
		player[4].id=4;
		player[4].team=1;
		
	}
	else
	{
		player[4].x=270;
		player[4].y=200;
		player[4].id=4;
		player[4].team=0;
	}
	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Send_Queue()
{
	int i;
	for(i=0;i<5;i++)
	{
		New_State.players[team][i].x_pos=player[i].x;
		New_State.players[team][i].y_pos=player[i].y;
	}
	IOPlayer_send(&io_stuff, &New_State);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART C/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART C/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART C/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------the player run to defend the red area------------------------------------------
void Run_Def(int id)
{
    player_up[id].action=0;
    player_up[id].direction=Which_Direction_About_Player_To_Red_Area(id);
    player_up[id].speed=50;
    Send_Update_Queue(id);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------the player run rapidly to the ball------------------------------------------
void Run_Quick(int id)
{
    player_up[id].action=0;
    player_up[id].direction=Which_Direction_About_Player_To_Ball(id);
    player_up[id].speed=50;
    Send_Update_Queue(id);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------the player run to the ball------------------------------------------
void Run(int id)
{
    player_up[id].action=0;
    player_up[id].direction=Which_Direction_About_Player_To_Ball(id);
    player_up[id].speed=20;
    Send_Update_Queue(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------direction of the player to another player----------------------------------------
int Which_Direction_About_Player_To_Player(int idA,int idB )
{
    int i=0, direct=0;
	int a,b;
	a=player_sent[idB].x-player_sent[idA].x;
	b=player_sent[idB].y-player_sent[idA].y;
	return Coor_To_Direction(a, b);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------direction to the red area--------------------------------------------------
int Which_Direction_About_Player_To_Red_Area(int id)
{
	int xb, yb;
	int i=0, direct=0;
	int a,b;
	if(id==1)
	{
		if(team==0)
		{
			xb=70; yb=170;
		}
		else
		{
			xb=570; yb=230;
		}
	}
	else
	{
		if(team==0)
		{
			xb=70; yb=230;
		}
		else
		{
			xb=70; yb=170;
		}
	}
	
	a=xb-player_sent[id].x;
	b=yb-player_sent[id].y;
	return Coor_To_Direction(a, b);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------direction of the ball to goal----------------------------------------
int Which_Direction_About_Ball_To_Goal(int id)
{
    int i=0, direct=0;
	int a,b;//xb, yb, xp=player_sent[id].x, yp=player_sent[id].y;
	
	if(team==0) // we're A team
	{
		if(id==3) // we are striker 3
		{
			a=620-ball.x;
			b=160-ball.y;
		}
		else
		{
			a=620-ball.x;
			b=230-ball.y;
		}
	}
	else
	{
		if(id==3) // we are striker 3
		{
			a=ball.x;
			b=160-ball.y;
		}
		else
		{
			a=ball.x;
			b=230-ball.y;
		}               
	}
	
	return Coor_To_Direction(a, b);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void callback(GameState *State_Received)
{
	// COPYING THE STATE TO OUR STRUCTURE
	Copy_State_Received(State_Received);
	//Identification of the packet
	switch(State_Received->special) //---info
	{
		case EVENT_FOOL:
			Initialisation_Foul(State_Received->special);
			break;
		case EVENT_GOAL:
			Initialisation_Goal();
			break;
		case EVENT_NONE:
			Update_Strategy();
			break;          
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Copy_State_Received(GameState *State_Received)
{
	int i;
	ball.x = State_Received->ball.x_pos;
	ball.y= State_Received->ball.y_pos;
	ball.speed= State_Received->ball.speed;
	ball.direction= State_Received->ball.direction;
	
	if(team==TEAM_A)
	{
		for(i=0 ; i<5 ; i++)
		{
			player_sent[i].x = State_Received->players[TEAM_A][i].x_pos;
			player_sent[i].y = State_Received->players[TEAM_A][i].y_pos;
			player_sent[i].team = 0;
			player_sent[i].id = i;
		}
		for(i=0;i<5;i++)
		{
			oplayer[i].x= State_Received->players[TEAM_B][i].x_pos;
			oplayer[i].y= State_Received->players[TEAM_B][i].y_pos;
			oplayer[i].team=1;
			oplayer[i].id=i;
		}
	}
	else
	{
		for(i=0;i<5;i++)
		{
			player_sent[i].x= State_Received->players[TEAM_B][i].x_pos;
			player_sent[i].y= State_Received->players[TEAM_B][i].y_pos;
			player_sent[i].team=1;
			player_sent[i].id=i;
		}
		for(i=0;i<5;i++)
		{
			oplayer[i].y= State_Received->players[TEAM_A][i].y_pos;
			oplayer[i].team=0;
			oplayer[i].id=i;
		}
	}
}
