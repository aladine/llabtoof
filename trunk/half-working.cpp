#include <iostream>
#include<math.h>
#include<stdio.h>
/***********************************************************************************************************************************************/
//////////////STRUCTURES///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
struct Player // data that will be sent to the server
{
	unsigned x, y;
	unsigned int id;
	unsigned int team;
	//	unsigned char direction;
	//	unsigned char speed;
};
struct Player_sent // data we received from the server
{
	unsigned x, y;
	unsigned int id;
	unsigned int team;
};
struct Player_up // data we send to the server
{
	unsigned int action; // 0 - movement 1 - kick
	unsigned int id;
	unsigned int team;
	unsigned char direction;
	unsigned char speed;
};
struct Ball // structure defining the ball
{
	unsigned  x, y;
	unsigned char direction;
	unsigned char speed;
};
struct Area // structure defining the areas
{
	int pox;
	int poy;
	int poX;
	int poY;
};
struct Direction
{
	int x;
	int y;
};
/***********************************************************************************************************************************************/
//////////////DEFINITIONS///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
typedef struct Player Player;
typedef struct Player_sent Player_sent;
typedef struct Player_up Player_up;
typedef struct Ball Ball;
typedef struct Area Area;
typedef struct Direction Direction;
/***********************************************************************************************************************************************/
//////////////GLOBALS///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
Direction direction[16];
Area Area_1,Area_2,Area_3,Area_4;
Area Area_red;
int Team;
Ball ball;
Player player[5];
Player_sent player_sent[5];
Player_up player_up[5];
Player_sent oplayer[5];
/***********************************************************************************************************************************************/
///////DECLATRATIONS////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
void Initialisation_of_Areas();
void Update_Strategy();  
int Choose_Strategy();
void Goalkeeper_Strategy_Defense();
void Defender_Strategy_Defense(Player_up player, int id);  
void Striker_Strategy_Attack(Player_sent player, int id);
void Pass_Stricker_free_A(Player_up player, int i);
void Pass_Stricker_free_B(Player_up player, int i);
int Which_Direction_About_Player_To_Ball(Player_sent player, Ball ball);
int Number_Opponent_Player_Area(int area);
int Where_Is_The_player(Player_sent player);
int Where_Is_The_Opponent_Player(Player_sent player);
void Send_Update_Queue(Player_up player);
int Distance_Player_Ball(Player_sent player, Ball ball);
void Shoot(int id); 
int Which_Player_In_My_Area(int My_Area); 
int How_Many_Opponents_In_The_Other_Area(int My_Area);
void Pass_To_The_Other_Striker(int id); 
int Where_Is_The_player(Player_sent player);
int Where_Is_The_Ball();
void Run_Average(Player_sent player, int id);
//////////////////////////////////////////////////
//////PART B/////////////////////////////////////
/////////////////////////////////////////////////
void Initialisation_Beginning();
void Initialisation_Goal();
void Initialisation_Foul(int team_ref);
void Construct_Initial_Packet_Player0(int Team);
void Construct_Initial_Packet_Player1(int Team);
void Construct_Initial_Packet_Player2(int Team);
void Construct_Initial_Packet_Player3(int Team);
void Construct_Initial_Packet_Player4(int Team);
int Read_Value();  // NOT WRITTEN !!!!!!!!!!!!!!!!!!!!
int Read_Value_Of_Dip_Switch();  // NOT WRITTEN !!!!!!!!!!!!!!!!!!!!
void Send_Queue(Player player);  // NOT WRITTEN !!!!!!!!!!!!!!!!!!!!
//////////////////////////////////////////////////
//////PART C/////////////////////////////////////
/////////////////////////////////////////////////
void Run_Average(Player_up player, int id);
void Run_Def(Player_up player, int id);
void Run_Quick(Player_up player, int id);
void Run(Player_up player, int id);
int Which_Direction_About_Player_To_Player(Player_sent playerA,Player_sent playerB );
int Which_Direction_About_Player_To_Red_Area(Player_sent player, int id);
int Which_Direction_About_Ball_To_Goal(int id);
/***********************************************************************************************************************************************/
//////////////MAIN///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
int main()
{
	return 0;
}
/***********************************************************************************************************************************************/
///////FUNCTIONS////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
void Initialisation_of_Areas()
{
	if(Team==0)// we are team A
	{
		Area_1.pox=0;// our side up 
		Area_1.poy=0;
		Area_1.poX=300;
		Area_1.poY=200;
		
		Area_2.pox=0;// our side down
		Area_2.poy=200;
		Area_2.poX=300;
		Area_2.poY=400;
		
		Area_3.pox=300; // other side up
		Area_3.poy=0;
		Area_3.poX=600;
		Area_3.poY=200;
		
		Area_4.pox=300; // other side down
		Area_4.poy=200;
		Area_4.poX=600;
		Area_4.poY=400;
		
		Area_red.pox=0; // red area
		Area_red.poy=160;
		Area_red.poX=50;
		Area_red.poY=240;
	}
	else
	{
		Area_3.pox=0;// other side up 
		Area_3.poy=0;
		Area_3.poX=300;
		Area_3.poY=200;
		
		Area_4.pox=0;// other side down
		Area_4.poy=200;
		Area_4.poX=300;
		Area_4.poY=400;
		
		Area_1.pox=300; // our side up
		Area_1.poy=0;
		Area_1.poX=600;
		Area_1.poY=200;
		
		Area_2.pox=300; // our side down
		Area_2.poy=200;
		Area_2.poX=600;
		Area_2.poY=400;
		
		Area_red.pox=550; // red area
		Area_red.poy=160;
		Area_red.poX=600;
		Area_red.poY=240;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
void Update_Strategy()
{
    if(Choose_Strategy()==0)
    {
        Goalkeeper_Strategy_Defense();
        Defender_Strategy_Defense(player_up[1], 1);
        Defender_Strategy_Defense(player_up[2], 2);
      //  Stricker_Strategy_Defense(player_up[3]);
      //  Stricker_Strategy_Defense(player_up[4]);
    }
    else
    {
      //  Goalkeeper_Strategy_Attack();
      //  Defender_Strategy_Attack(player_up[1]);
      //  Defender_Strategy_Attack(player_up[2]);
		Striker_Strategy_Attack(player_sent[3], 3);
		Striker_Strategy_Attack(player_sent[4], 4);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
int Choose_Strategy()
{
	if(Team==0)
	{
		if(ball.x<=300)
			return 0; //strategy defense
		else
			return 1; //strategy attack
	}
	else
	{
		if(ball.x>=300)
			return 0; //strategy defense
		else
			return 1; //strategy attack	
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Goalkeeper_Strategy_Defense()
{
    if(Distance_Player_Ball(player_sent[0], ball)<=3) // the goalkeeper has the ball
    {
		if(Team==0)
			Pass_Stricker_free_A(player_up[0], 0);
		else
			Pass_Stricker_free_B(player_up[0], 0);
    }
    else
    {
        if(170<=ball.y<=230) //170<ball.y<230
        {
            if(ball.x<=50) //ball in the red area we are team A
            {
                // calcul of the speed
                int b,c,v, alpha, time;
                alpha=fabs(ball.direction-8);
                b= ball.x/cos(alpha*22.5);
                c= sqrt(b*b-ball.x*ball.x);
                c= ball.y-player_sent[0].y+c;
                time= b/ball.speed;
                v=fabs(c/time);
				
                while(v%5!=0)
                    v=-1;
				
                player_up[0].speed=v;
                player_up[0].action=0;
				
                if(player_sent[0].y<ball.y)
                    player_up[0].direction=4;
                else
                    player_up[0].direction=12;
				
                Send_Update_Queue(player_up[0]);
            }
            else // ball not in the red area
            {
                if(ball.y<=player_sent[0].y)
                    player_up[0].direction=12;
                else
                    player_up[0].direction=4;
				
                player_up[0].speed=10;
                player_up[0].action=1;
                Send_Update_Queue(player_up[0]);
            }
			
			if(ball.x>=550) //ball in the red area we are team B
            {
                // calcul of the speed
                int b,c,v, alpha, time;
				if((13<=ball.direction)&&(ball.direction<=15))
					alpha=16-ball.direction;
				else
					alpha=fabs(ball.direction);
					
					b= (600-ball.x)/cos(22.5);
                c= sqrt(b*b-ball.x*ball.x);
                c= ball.y-player_sent[0].y+c;
                time= b/ball.speed;
                v=fabs(c/time);
				
                while(v%5!=0)
                    v=-1;
				
                player_up[0].speed=v;
                player_up[0].action=0;
				
                if(player_sent[0].y<ball.y)
                    player_up[0].direction=12;
                else
                    player_up[0].direction=4;
				
                Send_Update_Queue(player_up[0]);
            }
            else // ball not in the red area
            {
                if(ball.y<=player_sent[0].y)
                    player_up[0].direction=4;
                else
                    player_up[0].direction=12;
				
                player_up[0].speed=10;
                player_up[0].action=1;
                Send_Update_Queue(player_up[0]);
            }
        }
        else // 170>ball.y>230
        {
            player_up[0].direction=0;
            player_up[0].speed=0;
            player_up[0].action=0;
            Send_Update_Queue(player_up[0]);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Defender_Strategy_Defense(Player_up player, int id)
{
    if(Distance_Player_Ball(player_sent[id], ball)<=3) //Defender has the ball
    {
        if(player_sent[id].x>=300) // Defender pass the ball to the strcker when he is near the middle line
        {
            if(Team==0)
			{
				Pass_Stricker_free_A(player, id);
			}
			else
			{
				Pass_Stricker_free_B(player, id);
			}
        }
        else
        {
            switch (Number_Opponent_Player_Area(id))
            {
			case 0:
				Run_Average(player, id); // Defender run with the ball
				break;
			case 1:
					int i=Which_Player_In_My_Area(id);
					
				if(oplayer[i].x<player_sent[id].x)
					Run_Average(player, id); // Defender run with the ball
				else
					if(Team==0)
					{
						Pass_Stricker_free_A(player, id);
					}
					else
					{
						Pass_Stricker_free_B(player, id);
					}
				break;
			default:
					if(Team==0)
					{
						Pass_Stricker_free_A(player, id);
					}
					else
					{
						Pass_Stricker_free_B(player, id);
					}				break;
            }
        }
    }
    else // Defender hasn't the ball
    {
        if(Where_Is_The_Ball()==id) // ball in his area
        {
            if(Number_Opponent_Player_Area(id)!=0)
                Run_Quick(player, id);
            else
                Run(player, id);
        }
        else //ball in the area of the second defender
        {
            Run_Def(player, id);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Striker_Strategy_Attack(Player_sent player, int id)
{
	if(Distance_Player_Ball(player, ball)<=3) // the striker has the ball
	{
		switch (Number_Opponent_Player_Area(id))
		{
			case 0: // zero opponents in my area
			{
				if(Team==0) // I'm team A
				{
					if(player.x>450) // near the goal, close enough to shoot
					{
						Shoot(id);
					}
					else // not close enough
					{
						Run_Average(player_sent[id], id);
					}
				}
				else // I'm team B
				{
					if(player.x<150) // near the goal, close enough to shoot
					{
						Shoot(id);
					}
					else // not close enough
					{
						Run_Average(player_sent[id], id);
					}
				}
				break;
			}	
			case 1: // one opponent in my area
			{	
				if(Team==0) // I'm team A 
				{
					if(oplayer[Which_Player_In_My_Area(id)].x<player.x) // the player that is in my area is behind me
					{
						if(player.x>450) // near the goal, close enough to shoot
						{
							Shoot(id);
						}
						else // not close enough
						{
							Run_Average(player_sent[id], id);
						}	
					}
					else // the player that is in my area is in front of me
					{
						if(How_Many_Opponents_In_The_Other_Area(id)>=2) // there are 2 or more opponents in the area of the other striker
						{
							Shoot(id);
						}
						else // there is just one opponent in the other area or there are none
						{
							Pass_To_The_Other_Striker(id);
						}
						
					}
				}
				else // I'm team B
				{
					if(oplayer[Which_Player_In_My_Area(id)].x>player.x) // the player that is in my area is behind me
					{
						if(player.x<150) // near the goal, close enough to shoot
						{
							Shoot(id);
						}
						else // not close enough
						{
							Run_Average(player_sent[id], id);
						}	
					}
					else // the player that is in my area is in front of me
					{
						if(How_Many_Opponents_In_The_Other_Area(id)>=2) // there are 2 or more opponents in the area of the other striker
						{
							Shoot(id);
						}
						else // there is just one opponent in the other area or there are none
						{
							Pass_To_The_Other_Striker(id);
						}
						
					}
					
				}
				break;
			}
			default: // two or more than two opponents in my area	
			{
				if(How_Many_Opponents_In_The_Other_Area(id)>=2) // there are 2 or more opponents in the area of the other striker
				{
					Shoot(id);
				}
				else // there is just one opponent in the other area or there are none
				{
					Pass_To_The_Other_Striker(id);
				}
				break;
			}
		}
	}
	else // the striker doesn't have the ball
	{
		if(Where_Is_The_Ball()==id) // the ball is in the area of the striker
		{
			Run(player_up[id], id);
		}
		else // the ball is not in his area
		{
			player_up[id].direction=0;
            player_up[id].speed=0;
            player_up[id].action=0;
            Send_Update_Queue(player_up[id]);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------function returning the distance of the player to the ball-------------
int Distance_Player_Ball(Player_sent player, Ball ball)
{
	int distance;
	int xb=ball.x,
	yb=ball.y,
	xp=player.x,
	yp=player.y;
	
	distance= (xb-xp)*(xb-xp)+(yb-yp)*(yb-yp);
	distance=sqrt(distance);
    return distance-12;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------Pass to the stricker who has the area with the least of opponents--------
void Pass_Stricker_free_A(Player_up player, int i)
{
    int direct= Which_Direction_About_Player_To_Ball(player_sent[i], ball);
	
    if((0<=direct<4) || (12<direct<=15)) // ball in front the goalkeeper
    {
        if( Number_Opponent_Player_Area(3)<=Number_Opponent_Player_Area(4) )
        {
            player.action=1; //kick
            player.direction=15;
            player.speed=100; // 50px/sec
        }
        else
        {
			player.action=1; //kick
            player.direction=1;
            player.speed=100; // 50px/sec
        }
    }
    else
    {
        player.action=0; //movement
        if(12<=player.direction<16)
            player.direction=player.direction-2;
        else
            player.direction=player.direction+2;
        player.speed=50; // 50px/sec
    }
	
    Send_Update_Queue(player);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pass_Stricker_free_B(Player_up player, int i)
{
	int direct= Which_Direction_About_Player_To_Ball(player_sent[i], ball);
	if(4<=direct<12) // ball in front the goalkeeper
	{
		if( Number_Opponent_Player_Area(3)<=Number_Opponent_Player_Area(4) )
		{
			player.action=1; //kick
			player.direction=9;
			player.speed=100; // 50px/sec
		}
		else
		{
			player.action=1; //kick
			player.direction=7;
			player.speed=100; // 50px/sec
		}
	}
	else
	{
		player.action=0; //movement
		if((12<=player.direction<16)||(0<=player.direction<4))
			player.direction=player.direction-2;
		else
			player.direction=player.direction+2;
		player.speed=50; // 50px/sec
	}
	Send_Update_Queue(player);
}	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------direction of the player to the ball----------------------------------------
int Which_Direction_About_Player_To_Ball(Player_sent player, Ball ball)
{
	int i=0, direct=0;
	int a,b, xb=ball.x, yb=ball.y, xp=player.x, yp=player.y;
	a=xb-xp;
	b=yb-yp;
	while(i<16)
	{
		if ( (direction[i].x%a)==0)
			if ( (direction[i].y%b)==0)
				direct=i;
			else
				i++;
		else
			i++;
	}
	return direct;
}
//--maybe better to compare ball x and player x

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------number of opponent player in the area---------------------------------------
int Number_Opponent_Player_Area(int area)  // function to assess how many oponents
{  // are in a given area
	int number=0;
	for (int i=0; i<5; i++) // checking the position of each opponent player
	{
		if (Where_Is_The_Opponent_Player(oplayer[i])==area)
			number++; // increase the number if there is a player in this area
	}
	return number;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------the area where is the Player------------------------------------------------
int Where_Is_The_player(Player_sent player)
{ // the function returns the number of the area in which the  player is
	if (Area_1.pox<player.x<Area_1.poX) //checking if he is in area 1
		if (Area_1.poy<player.y<Area_1.poY)
			return 1;
	if (Area_2.pox<player.x<Area_2.poX)  // checking if he is in area 2
		if (Area_2.poy<player.y<Area_2.poY)
			return 2;
	if (Area_3.pox<player.x<Area_3.poX) // checking if he is in area 3
		if (Area_3.poy<player.y<Area_3.poY)
			return 3;
	if (Area_4.pox<player.x<Area_4.poX) // checking if he is in area 4
		if (Area_4.poy<player.y<Area_4.poY)
			return 4;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Where_Is_The_Opponent_Player(Player_sent player)
{ // the function returns the number of the area in which the  player is
	if (Area_1.pox<player.x<Area_1.poX) //checking if he is in area 1
		if (Area_1.poy<player.y<Area_1.poY)
			return 1;
	if (Area_2.pox<player.x<Area_2.poX)  // checking if he is in area 2
		if (Area_2.poy<player.y<Area_2.poY)
			return 2;
	if (Area_3.pox<player.x<Area_3.poX) // checking if he is in area 3
		if (Area_3.poy<player.y<Area_3.poY)
			return 3;
	if (Area_4.pox<player.x<Area_4.poX) // checking if he is in area 4
		if (Area_4.poy<player.y<Area_4.poY)
			return 4;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Send_Update_Queue(Player_up player)
{
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Shoot(int id)
{
	player_up[id].direction=Which_Direction_About_Ball_To_Goal(id);
	player_up[id].action=1;
	player_up[id].speed=100;
	Send_Update_Queue(player_up[id]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Which_Player_In_My_Area(int My_Area) // function returns the number of the player that is in my area 
{
	int Which_Player;
	for(int i=0;i<5;i++)
	{
		if(Where_Is_The_Opponent_Player(oplayer[i])==My_Area)
			Which_Player=i;
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
			player_up[id].direction=Which_Direction_About_Player_To_Player(player_sent[3], player_sent[4]);
		}
		else
		{
			player_up[id].direction=Which_Direction_About_Player_To_Player(player_sent[4], player_sent[3]);
		}
		Send_Update_Queue(player_up[id]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Where_Is_The_Ball()
{
	if (Area_1.pox<ball.x<Area_1.poX)
		if (Area_1.poy<ball.y<Area_1.poY)
			return 1;
	if (Area_2.pox<ball.x<Area_2.poX)
		if (Area_2.poy<ball.y<Area_2.poY)
			return 2;
	if (Area_3.pox<ball.x<Area_3.poX)
		if (Area_3.poy<ball.y<Area_3.poY)
			return 3;
	if (Area_4.pox<ball.x<Area_4.poX)
		if (Area_4.poy<ball.y<Area_4.poY)
			return 4;
}
void Run_Average(Player_sent player, int id)
{
    player_up[id].action=1;
    if(Team==0)
        player_up[id].direction=0;
    else
        player_up[id].direction=8;
    player_up[id].speed=15;
    Send_Update_Queue(player_up[id]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART B/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART B/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART B/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------
//Initialisation of positions of players
//-------------------------------------------------------------------
void Initialisation_Beginning()
{
	Team=Read_Value_Of_Dip_Switch(); //---0=teamA ---1=teamB
	Construct_Initial_Packet_Player0(Team);
	Construct_Initial_Packet_Player1(Team);
	Construct_Initial_Packet_Player2(Team);
	Construct_Initial_Packet_Player3(Team);
	Construct_Initial_Packet_Player4(Team);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------
//Initialisation of positions of players after gaol
//-------------------------------------------------------------------
void Initialisation_Goal()
{
	Construct_Initial_Packet_Player0(Team);
	Construct_Initial_Packet_Player1(Team);
	Construct_Initial_Packet_Player2(Team);
	Construct_Initial_Packet_Player3(Team);
	Construct_Initial_Packet_Player4(Team);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------
//Initialisation of positions of players after foul
//-------------------------------------------------------------------
void Initialisation_Foul(int team_ref)
{
	if(team_ref==Team) //foul against us
	{
		if(ball.x<=300) //foul in our side
		{
			//---player0--
			player[0].x=0;
			player[0].y=200;
			Send_Queue(player[0]);
			//---player1--
			player[1].x=30;
			player[1].y=180;
			Send_Queue(player[1]);
			//---player2--
			player[2].x=30;
			player[2].y=220;
			Send_Queue(player[2]);
			//---player3--
			player[3].x=140;
			player[3].y=200;
			Send_Queue(player[3]);
			//---player4--
			player[4].x=300;
			player[4].y=200;
			Send_Queue(player[4]);
		}
		else //foul in the opponent side
		{
			//---player0--
			player[0].x=300;
			player[0].y=200;
			Send_Queue(player[0]);
			//---player1--
			player[1].x=300;
			player[1].y=100;
			Send_Queue(player[1]);
			//---player2--
			player[2].x=300;
			player[2].y=300;
			Send_Queue(player[2]);
			//---player3--
			player[3].x=440;
			player[3].y=200;
			Send_Queue(player[3]);
			//---player4--
			player[4].x=500;
			player[4].y=300;
			Send_Queue(player[4]);
		}
	}
	else //foul against the opponent tean
	{
		if(ball.x<=300) //foul in our side
		{
			//---player0--
			player[0].x=0;
			player[0].y=200;
			Send_Queue(player[0]);
			//---player1--
			player[1].x=30;
			player[1].y=180;
			Send_Queue(player[1]);
			//---player2--
			player[2].x=145;
			player[2].y=200;
			Send_Queue(player[2]);
			//---player3--
			player[3].x=300;
			player[3].y=100;
			Send_Queue(player[3]);
			//---player4--
			player[4].x=300;
			player[4].y=300;
			Send_Queue(player[4]);
		}
		else //foul in the opponent side
		{
			//---player0--
			player[0].x=300;
			player[0].y=200;
			Send_Queue(player[0]);
			//---player1--
			player[1].x=300;
			player[1].y=100;
			Send_Queue(player[1]);
			//---player2--
			player[2].x=445;
			player[2].y=200;
			Send_Queue(player[2]);
			//---player3--
			player[3].x=550;
			player[3].y=200;
			Send_Queue(player[3]);
			//---player4--
			player[4].x=500;
			player[4].y=240;
			Send_Queue(player[4]);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------
void Construct_Initial_Packet_Player0(int Team)
{
	if(Team==1)
	{
		player[0].x=0;
		player[0].y=200;
		player[0].id=0;
		player[0].team=1;
		
	}
	else
	{
		player[0].x=600;
		player[0].y=200;
		player[0].id=0;
		player[0].team=0;
	}
	
	Send_Queue(player[0]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player1(int Team)
{
	if(Team=1)
	{
		player[1].x=150;
		player[1].y=100;
		player[1].id=1;
		player[1].team=1;
		
	}
	else
	{
		player[1].x=450;
		player[1].y=100;
		player[1].id=1;
		player[1].team=0;
	}
	
	Send_Queue(player[1]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player2(int Team)
{
	if(Team=1)
	{
		player[2].x=150;
		player[2].y=300;
		player[2].id=2;
		player[2].team=1;
		
	}
	else
	{
		player[2].x=450;
		player[2].y=300;
		player[2].id=2;
		player[2].team=0;
	}
	
	Send_Queue(player[2]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player3(int Team)
{
	if(Team=1)
	{
		player[3].x=250;
		player[3].y=200;
		player[3].id=3;
		player[3].team=1;
		
	}
	else
	{
		player[3].x=350;
		player[3].y=200;
		player[3].id=3;
		player[3].team=0;
	}
	
	Send_Queue(player[3]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Construct_Initial_Packet_Player4(int Team)
{
	if(Team=1)
	{
		player[4].x=350;
		player[4].y=200;
		player[4].id=4;
		player[4].team=1;
		
	}
	else
	{
		player[4].x=250;
		player[4].y=200;
		player[4].id=4;
		player[4].team=0;
	}
	
	Send_Queue(player[4]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Read_Value()
{
	//return value of North button;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Read_Value_Of_Dip_Switch()
{
	//return value of dip switch
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Send_Queue(Player player)
{
	// send the initial position packet for each player
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART C/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART C/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////PART C/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------the player run with the ball------------------------------------------
void Run_Average(Player_up player, int id)
{
    player.action=1;
    if(Team==0)
        player.direction=0;
    else
        player.direction=8;
    player.speed=15;
    Send_Update_Queue(player);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------the player run to defend the red area------------------------------------------
void Run_Def(Player_up player, int id)
{
    player.action=0;
    player.direction=Which_Direction_About_Player_To_Red_Area(player_sent[id], id);
    player.speed=50;
    Send_Update_Queue(player);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------the player run rapidly to the ball------------------------------------------
void Run_Quick(Player_up player, int id)
{
    player.action=0;
    player.direction=Which_Direction_About_Player_To_Ball(player_sent[id], ball);
    player.speed=50;
    Send_Update_Queue(player);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------the player run to the ball------------------------------------------
void Run(Player_up player, int id)
{
    player.action=0;
    player.direction=Which_Direction_About_Player_To_Ball(player_sent[id], ball);
    player.speed=20;
    Send_Update_Queue(player);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------direction of the player to another player----------------------------------------
int Which_Direction_About_Player_To_Player(Player_sent playerA,Player_sent playerB )
{
    int i=0, direct=0;
	int a,b, xb=playerB.x, yb=playerB.y, xp=playerA.x, yp=playerA.y;
	a=xb-xp;
	b=yb-yp;
	while(i<16)
	{
		if ( (direction[i].x%a)==0)
			if ( (direction[i].y%b)==0)
				direct=i;
			else
				i++;
		else
			i++;
	}
	return direct;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------direction to the red area--------------------------------------------------
int Which_Direction_About_Player_To_Red_Area(Player_sent player, int id)
{
	int xb, yb;
int i=0, direct=0;
int a,b, xp=player.x, yp=player.y;
if(id==1)
{
	if(Team==0)
	{
		 xb=50; yb=170;
	}
	else
	{
		xb=550; yb=230;
	}
}
else
{
	if(Team==0)
	{
		xb=50; yb=230;
	}
	else
	{
		xb=50; yb=170;
	}
}

a=xb-xp;
b=yb-yp;
while(i<16)
{
	if ( (direction[i].x%a)==0)
		if ( (direction[i].y%b)==0)
			direct=i;
			else
				i++;
		else
			i++;
			}
return direct;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------direction of the ball to goal----------------------------------------
int Which_Direction_About_Ball_To_Goal(int id)
{
    int i=0, direct=0;
	int a,b, xb, yb, xp=player_sent[id].x, yp=player_sent[id].y;
	
	if(Team==0) // we're A team
	{
		if(id==3) // we are striker 3
		{
			a=600-ball.x;
			b=160-ball.y;
		}
		else
		{
			a=600-ball.x;
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
	
	while(i<16)
	{
		if ( (direction[i].x%a)==0)
			if ( (direction[i].y%b)==0)
				direct=i;
			else
				i++;
			else
				i++;
	}
	return direct;
}