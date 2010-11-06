#include <math.h>
#include "function.h"
#include "../structures.h"
#include "../io/io_structures.h"


void Striker_Strategy_Attack(int id)
{
	if(Distance_Player_Ball(id)<=15) // the striker has the ball
	{
		switch (Number_Opponent_Player_Area(id))
		{
			case 0: // zero opponents in my area
			{
				if(team==0) // I'm team A
				{
					if(player_sent[id].x>470) // near the goal, close enough to shoot
					{
						Shoot(id);
					}
					else // not close enough
					{
						Run_Average(id);
					}
				}
				else // I'm team B
				{
					if(player_sent[id].x<170) // near the goal, close enough to shoot
					{
						Shoot(id);
					}
					else // not close enough
					{
						Run_Average(id);
					}
				}
				break;
			}	
			case 1: // one opponent in my area
			{	
				if(team==0) // I'm team A 
				{
					if(oplayer[Which_Player_In_My_Area(id)].x<player_sent[id].x) // the player that is in my area is behind me
					{
						if(player_sent[id].x>470) // near the goal, close enough to shoot
						{
							Shoot(id);
						}
						else // not close enough
						{
							Run_Average(id);
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
					if(oplayer[Which_Player_In_My_Area(id)].x>player_sent[id].x) // the player that is in my area is behind me
					{
						if(player_sent[id].x<170) // near the goal, close enough to shoot
						{
							Shoot(id);
						}
						else // not close enough
						{
							Run_Average(id);
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
			Run( id);
		}
		else // the ball is not in his area
		{
			player_up[id].direction=0;
            player_up[id].speed=0;
            player_up[id].action=0;
            Send_Update_Queue(id);
		}
	}
}
