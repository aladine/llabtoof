#include <math.h>
#include "function.h"
#include "../structures.h"
#include "../io/io_structures.h"

void Goalkeeper_Strategy_Defense()
{
    int stop=1;
    if(Area_red.pox<player_sent[0].x<Area_red.poX)
    {
        if(Area_red.poy<player_sent[0].y<Area_red.poY){} //goalkeeper in the red area=> do nothing
        else 
        {
            player_up[0].action=0;
            player_up[0].direction=Which_Direction_About_gp_To_Red_Area();
            player_up[0].speed=50;
            Send_Update_Queue(0);
            stop=0;
        }
    }
    else
    {
        player_up[0].action=0;
        player_up[0].direction=Which_Direction_About_gp_To_Red_Area();
        player_up[0].speed=50;
        Send_Update_Queue(0);
        stop=0;
    }
	
    if(stop==0) //end of execution if stop equals to 0
	{
		goto END_OF_THIS_FUNCTION;
	}
	// put the goalkeeper code already writen here
    if(Distance_Player_Ball(0)<=3) // the goalkeeper has the ball
    {
		if(team==0)
			Pass_Stricker_free_A(0);
		else
			Pass_Stricker_free_B(0);
    }
    else
    {
        if(170<=ball.y<=230) //170<ball.y<230
        {
            if(ball.x<=70) //ball in the red area we are team A
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
				
                Send_Update_Queue(0);
            }
            else // ball not in the red area
            {
                if(ball.y<=player_sent[0].y)
                    player_up[0].direction=12;
                else
                    player_up[0].direction=4;
				
                player_up[0].speed=10;
                player_up[0].action=1;
                Send_Update_Queue(0);
            }
			
			if(ball.x>=570) //ball in the red area we are team B
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
				
                Send_Update_Queue(0);
            }
            else // ball not in the red area
            {
                if(ball.y<=player_sent[0].y)
                    player_up[0].direction=4;
                else
                    player_up[0].direction=12;
				
                player_up[0].speed=10;
                player_up[0].action=1;
                Send_Update_Queue(0);
            }
        }
        else // 170>ball.y>230
        {
            player_up[0].direction=0;
            player_up[0].speed=0;
            player_up[0].action=0;
            Send_Update_Queue(0);
        }
    }
END_OF_THIS_FUNCTION:
	stop=1;
	
}
