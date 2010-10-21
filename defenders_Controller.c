////////////////////////
//Variables
////////////////////////
unsigned strategy_number; // defines the strategy that is being used
unsigned Ball_Position; // defines the position of the ball in regard to the areas
Struct Area_1{
	int pox =, poy=, poX=, poY=;
};// Other half of the pitch
Struct Area_2{
	int pox =, poy=, poX=, poY=;
}; // Lelft side of our half
Struct Area_3{
	int pox =, poy=, poX=, poY=;
};  // Right side of our half
Struct Area_4{
	int pox =, poy=, poX=, poY=;
} ; // Red area

Struct Player{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFENDERS’ STRATEGY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Defenders_Strategy_Function() // defines the strategy of the defenders
{
	Balll_Position=Where_Is_The_Ball(); // returns the number of the area with ball
	switch(strategy_number) // decides which strategy is played
	{
	case 1:  //Strategy 1	
		switch(Ball_Position)
		{
			case 0:  //Ball is in area 0
				if (Defender_Near_Ball()==0){
					
				}
				break;
			case 1: //Ball is in area 1
				if (Player_Near_Ball(player[1])==0) //player1 near the ball
					if ( (11<Which_Direction_About_Player_To_Ball(player[1])) || (5>Which_Direction_About_Player_To_Ball(player[1])) ) //player1 in front of the ball
						if (Number_Opponent_Player_Area(2)==0) //any opponnents players in the area
							int area=Run_or_not(player[1], 2);
  							if(( area==1) || ( area==2)) // any stricker free..
								Run0 (player[1]); //the player1 run with the ball
							else
								pass(player[1],player[area] ); //player1 pass the ball to the striker free
	
				break;
			case 3: //Ball is in area 3

				break;
			case 4: //Ball is in area 4
		
				break;
}	
case 2:
	} 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OTHER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Where_Is_The_Ball()
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Where_Is_The_player(Player player)  // function to decide where is the player
{ // the function returns the number of the area in which the  player is
if (Area_1.pox<player_position.x<Area_1.poX) //checking if he is in area 1
	if (Area_1.poy<player_position.y<Area_1.poY)
		return 1;
if (Area_2.pox<player_position.x<Area_2.poX)  // checking if he is in area 2
	if (Area_2.poy<player_position.y<Area_2.poY)
		return 2;
if (Area_3.pox<player_position.x<Area_3.poX) // checking if he is in area 3
	if (Area_3.poy<playerl_position.y<Area_3.poY)
		return 3;
if (Area_4.pox<player_position.x<Area_4.poX) // checking if he is in area 4
	if (Area_4.poy<player_position.y<Area_4.poY)
		return 4;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Number_Opponent_Player_Area(int area)  // function to assess how many oponents 
{  // are in a given area
	int number=0;
	for (i=0; i<5; i++) // checking the position of each opponent player
{
	if (Where_Is_The_Opponent_Player(opp_player[i])==area) 
		number++; // increase the number if there is a player in this area
}
return number;		
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Nearest_player_ball (ball_position)
{
	int nearest=200000000000000000000000000;
	for (i=0; i<5; i++) // checking the position of each of our player
{	
if (player_near_ball(player[i])<=nearest)
	nearest=i;
}
return nearest;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Which_Direction_About_Player_To_Ball(Player player, ball_position)
{
	int i=0, direct=0;
	int a,b, xb=ball_position.x, yb=ball_position.y, xp=player.x, yp=player.y;
	a=xb-xp; 
	b=yb-yp;
	while(i<16)
	{
		if ( (direction[i][0]%a)==0)
			if ( (direction[i][0]%b)==0)
				direct=i;
			else
				i++;
		else
			i++
	}
return direct;	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pass (Player p1, Player p2)
{
		
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Run0(Player p1, Area area)
{
		int midle = (area.poX-area.pox)/2;
		if (p1.x <=midle)
		{
			if( Number_Opponent_Player_Area(3)<=Number_Opponent_Player_Area(4) )
			{
				if( Which_Direction_About_Player_To_Ball(p1, ball_position)==0)
				{
					packet.kick=1;
					packet.direction=0;
					packet.speed=1010; // 50px/sec
				}
				else
				{
					Move_Player_Arround_Ball(p1, 0); // allows the player to kick the ball in the direction 0
				}
			}
			else
			{
				if( Which_Direction_About_Player_To_Ball(p1, ball_position)==1)
				{
					packet.kick=1;
					packet.direction=1;
					packet.speed=1010; // 50px/sec
				}
				else
				{
					Move_Player_Arround_Ball(p1, 1); // allows the player to kick the ball in the direction 0
				}
			}
		}
		else
		{
			if( Number_Opponent_Player_Area(3)>Number_Opponent_Player_Area(4) )
			{
				if( Which_Direction_About_Player_To_Ball(p1, ball_position)==0)
				{
					packet.kick=1;
					packet.direction=0;
					packet.speed=1010; // 50px/sec
				}
				else
				{
					Move_Player_Arround_Ball(p1, 0); // allows the player to kick the ball in the direction 0
				}
			}
			else
			{
				if( Which_Direction_About_Player_To_Ball(p1, ball_position)==15)
				{
					packet.kick=1;
					packet.direction=15;
					packet.speed=1010; // 50px/sec
				}
				else
				{
					Move_Player_Arround_Ball(p1, 15); // allows the player to kick the ball in the direction 0
				}
			}
		}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Run_Or_Not(Player player, int ind) 
{
		int i, area_player_free=ind;
		for(i=1; i<6; i++)
		{
			if(( Number_Opponent_Player_Area(i) <2) && (i!=ind))
				area_player_free=i;
		}
		return ares_player_free;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Player_Near_Ball(Player player, ball_position)  //function returning the distance of the player to the ball
{
	int distance, xb=ball_position.x, yb=ball_position.y, xp=player.x, yp=player.y;

	distance= (xb-xp)(xb-xp)+(yb-yp)(yb-yp);
	distance=sqrt(distance);
	if(distance==12)
		return 0;
	else
		return distance-12;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////