#include <math.h>
#include "function.h"
#include "../structures.h"
#include "../io/io_structures.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Defender_Strategy_Defense(int id)
{
	int i;
    if(Distance_Player_Ball(id)<=3) //Defender has the ball
    {
        if(player_sent[id].x>=320) // Defender pass the ball to the strcker when he is near the middle line
        {
            if(team==0)
			{
				Pass_Stricker_free_A(id);
			}
			else
			{
				Pass_Stricker_free_B(id);
			}
        }
        else
        {
            switch (Number_Opponent_Player_Area(id))
            {
				case 0:
					Run_Average(id); // Defender run with the ball
					break;
				case 1:
					i=Which_Player_In_My_Area(id);
					
					if(oplayer[i].x<player_sent[id].x)
						Run_Average(id); // Defender run with the ball
					else
						if(team==0)
						{
							Pass_Stricker_free_A(id);
						}
						else
						{
							Pass_Stricker_free_B(id);
						}
					break;
				default:
					if(team==0)
					{
						Pass_Stricker_free_A(id);
					}
					else
					{
						Pass_Stricker_free_B(id);
					}				break;
            }
        }
    }
    else // Defender hasn't the ball
    {
        if(Where_Is_The_Ball()==id) // ball in his area
        {
            if(Number_Opponent_Player_Area(id)!=0)
                Run_Quick(id);
            else
                Run(id);
        }
        else //ball in the area of the second defender
        {
            Run_Def(id);
        }
    }
}
