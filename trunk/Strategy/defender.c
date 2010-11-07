#include <math.h>
#include "function.h"
#include "structures.h"
#include "io_structures.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Defender_Strategy_Defense(int id)
{
	int i;
    if(Distance_Player_Ball(id)<=15) //Defender has the ball
    {
		//printf("\nI, defender %d, have the ball",id);
        if((team==0&&player_sent[id].x>=290)||(team==1&&player_sent[id].x<=350)) // Defender pass the ball to the strcker when he is near the middle line
        {
			//	printf("\nI, defender %d, pass the ball",id);
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
			//	printf("\nI, defender %d, count the number of opponents",id);
            switch (Number_Opponent_Player_Area(id))
            {
				case 0:
					//			printf("\nI, defender %d, 0 opponents in my area, I run with the ball",id);
					Run_Average(id); // Defender run with the ball
					break;
				case 1:
					
					i=Which_Player_In_My_Area(id);
					//			printf("\nI, defender %d, 1 opponent (oplayer[%d])in my area",id,i);
					if((Team==0&&oplayer[i].x<player_sent[id].x)||(Team==1&&oplayer[i].x>player_sent[id].x))
					{
						//				printf("\nI, defender %d, he is behind me I run with the ball",id);
						Run_Average(id); // Defender run with the ball
					}
					else
					{
						//				printf("\nI, defender %d, he is in front of me. I pass the ball",id);
						if(team==0)
						{
							Pass_Stricker_free_A(id);
						}
						else
						{
							Pass_Stricker_free_B(id);
						}
					}
					break;
				default:
					//			printf("\nI, defender %d, there are 2 or more opponents in my area, I pass the ball",id);
					if(team==0)
					{
						Pass_Stricker_free_A(id);
					}
					else
					{
						Pass_Stricker_free_B(id);
					}
					break;
            }
        }
    }
    else // Defender hasn't the ball
    {
		//printf("\nI, defender %d, 1 don't have the ball",id);	
        if(Where_Is_The_Ball()==id) // ball in his area
        {
			//		printf("\nI, defender %d, ball is in my area",id);
            if(Number_Opponent_Player_Area(id)!=0)
			{
				//			printf("\nI, defender %d, 1 opponent in my area so I run quick",id);
				Run_Quick(id);
			}
            else
			{
				//			printf("\nI, defender %d, 0 opponents in my area so I just run",id);
                Run(id);
			}
        }
        else //ball in the area of the second defender
        {
			//		printf("\nI, defender %d, ball is somewhere else",id);
            Run_Def(id);
        }
    }
}
