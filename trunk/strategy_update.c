Strategy_Update()
{
	if //beginning of the game
	{
		Manual_Initialisation();
	}
	if //goal
	{
		Automatic_Initalisation ();
	}
	if ball out
	{
		if //corner_for_us
		{
			Corner_For_Us();
		}
		if //corner_for_them
		{
			Corner_For_Them();
		}
		if //goal_kick_for_us
		{
			Goal_Kick_For_Us();
		}
		if //goal_kick_for_them
		{
			Goal_Kick_For_Them();
		}
		if //touch/outside_on_the_left_or_right
		{
			if //in_oour_area_we_are_throwing
			{
				Initialisation_Attack_1();
			}
			if //in_their_area_we_are_throwing
			{
				Initialisation_Attack_2();
			}
			if //in_our_area_they_are_throwing
			{
				Automatic_initialisation();
			}
			if //in_their_area_they_are_throwing
			{
				Initialisation_Defense();
			}
		}
	}
	
	if //ball in
	{
		if //ball is ours
		{
			if //dip switches [00] - offensive strategy 1
			{
				
			}
			if //dip switches [01] - offensive strategy 2
			{
				
			}
			if //dip switches [10] - offensive strategy 3
			{
				
			}
		}
		if //ball is not ours
		{
			if // dip switch [0] - defensive strategy 1
			{
				
			}
			if // dip switch [1] - defensive strategy 2
			{
				
			}
		}
		if //dip switches [111] - super defensive strategy
		{
			
		}
			
	}
	
}
///////////////////////////////////////////////////////////////////////////
////////////////////////FUNCTIONS//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////	
Manual_Initialisation()
{
//1)	Receive the info from the servers asking for initial position of the players
//2)	Count the number of clicks of button South ( modulo 5 counter )
//3)	Confirm your choice by clicking button North
//4)	Based on the result of modulo 5 choose initial positions
//5)	Send data back to server
}
///////////////////////////////////////////////////////////////////////////	
Automatic_Initialisation()
{
//1)	Randomly choose a number 1-5 
//2)	Based on the result choose initial positions
//3)	Update positions
}
///////////////////////////////////////////////////////////////////////////	
Corner_For_Us()
{
//1)	Put one of the strikers on the corner 
//2)	Put one defender and one striker in the goal area
//3)	Put the second defender in the middle of the pitch/
//4)	Random the direction of the kick between X and Y 
//5)	Kick the ball
}
///////////////////////////////////////////////////////////////////////////	
Corner_For_Us()
{
//1)	Put the appropriate defender on the corner of the goal 
//2)	Put one striker in the middle on the goal area
//3)	Put the other striker in the middle of the pitch
//4)	Put the other defender appropriately 
//5)	Put the goalkeeper in the middle of the goal 
}
///////////////////////////////////////////////////////////////////////////	
Goal_Kick()
{
//1)	Put the players in appropriate positions
//2)	Put the goalkeeper 
//3)	If there are >=2 opponents in our area kick the ball to the  nearest striker
//4)	If there is 1 opponent kick the ball to the defender furthest  from him
//5)	If there are no opponents in our area kick the ball to the closest player
}
///////////////////////////////////////////////////////////////////////////	
Goal_Kick_For_Them()
{
//1)	If we win put 2 strikers in the middle and 2 defenders in the middle of our area
//2)	If we loose put 2 strikers  in the middle of the opponent’s area and the defenders in the middle of our area
}

Initialisation_Attack_1()
{
//1)	The defender throws the ball to the nearest player NOT the goalkeeper
//2)	Put the players randomly on our area 
}

Initialisation_Attack_2();
{
//1)	If we loose and the ball is in the 4th quarter 
//	a.	2 strikers near goal
//	b.	One of the defender throws the ball
//	c.	The other defender stays in the middle of the pitch
//	d.	The defender throws the ball in the middle of the goal area
//2)	If we loose and the ball is  in the 3rd quarter 
//	a.	One striker in their area
//	b.	The second striker throws the ball  to the first
//	c.	Both defenders on the middle line
//	3)	If we win 
//	a.	Striker throws the ball  to the other striker
//	b.	Two defenders in the middle of the pitch
}

Initialisation_ Defense();
{
//1)	Put two defenders on our side
//2)	Put two strikers slightly closer to our side from the line of throwing
}

Super_Defensive_Strategy
{
//1)	Put all the players on the goal line 4 pixels apart from each other.
//2)	Move them one pixel to the right and then one to the left
//3)	Wait for the end of the match 
}

