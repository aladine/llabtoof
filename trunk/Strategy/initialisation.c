#include <math.h>
#include "function.h"
#include "../structures.h"
#include "../io/io_structures.h"

//-------------------------------------------------------------------
//Initialisation of positions of players
//-------------------------------------------------------------------
void Initialisation_Beginning()
{
	//Team=Read_Value_Of_Dip_Switch(); //---0=teamA ---1=teamB
	Construct_Initial_Packet_Player0(team);
	Construct_Initial_Packet_Player1(team);
	Construct_Initial_Packet_Player2(team);
	Construct_Initial_Packet_Player3(team);
	Construct_Initial_Packet_Player4(team);
	Send_Queue();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------
//Initialisation of positions of players after gaol
//-------------------------------------------------------------------
void Initialisation_Goal()
{
	Construct_Initial_Packet_Player0(team);
	Construct_Initial_Packet_Player1(team);
	Construct_Initial_Packet_Player2(team);
	Construct_Initial_Packet_Player3(team);
	Construct_Initial_Packet_Player4(team);
	Send_Queue();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------
//Initialisation of positions of players after foul
//-------------------------------------------------------------------
void Initialisation_Foul(int team_ref)
{
	if(team_ref==team) //foul against us
	{
		if(ball.x<=320) //foul in our side
		{
			//---player0--
			player[0].x=20;
			player[0].y=200;
			
			//---player1--
			player[1].x=70;
			player[1].y=180;
			
			//---player2--
			player[2].x=70;
			player[2].y=220;
			
			//---player3--
			player[3].x=180;
			player[3].y=200;
			
			//---player4--
			player[4].x=340;
			player[4].y=200;
			Send_Queue();
		}
		else //foul in the opponent side
		{
			//---player0--
			player[0].x=320;
			player[0].y=200;
			
			//---player1--
			player[1].x=320;
			player[1].y=100;
			
			//---player2--
			player[2].x=320;
			player[2].y=300;
			
			//---player3--
			player[3].x=460;
			player[3].y=200;
			
			//---player4--
			player[4].x=520;
			player[4].y=300;
			Send_Queue();
		}
	}
	else //foul against the opponent tean
	{
		if(ball.x<=300) //foul in our side
		{
			//---player0--
			player[0].x=20;
			player[0].y=200;
			
			//---player1--
			player[1].x=50;
			player[1].y=180;
			
			//---player2--
			player[2].x=165;
			player[2].y=200;
			
			//---player3--
			player[3].x=320;
			player[3].y=100;
			
			//---player4--
			player[4].x=320;
			player[4].y=300;
			Send_Queue();
		}
		else //foul in the opponent side
		{
			//---player0--
			player[0].x=320;
			player[0].y=200;
			
			//---player1--
			player[1].x=320;
			player[1].y=100;
			
			//---player2--
			player[2].x=465;
			player[2].y=200;
			
			//---player3--
			player[3].x=570;
			player[3].y=200;
			
			//---player4--
			player[4].x=520;
			player[4].y=240;
			Send_Queue();
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------------
void Initialisation_of_Areas()
{
	if(team==0)// we are team A
	{
		Area_1.pox=20;// our side up 
		Area_1.poy=0;
		Area_1.poX=320;
		Area_1.poY=200;
		
		Area_2.pox=20;// our side down
		Area_2.poy=200;
		Area_2.poX=320;
		Area_2.poY=400;
		
		Area_3.pox=320; // other side up
		Area_3.poy=0;
		Area_3.poX=620;
		Area_3.poY=200;
		
		Area_4.pox=320; // other side down
		Area_4.poy=200;
		Area_4.poX=620;
		Area_4.poY=400;
		
		Area_red.pox=20; // red area
		Area_red.poy=160;
		Area_red.poX=70;
		Area_red.poY=240;
	}
	else
	{
		Area_3.pox=20;// other side up 
		Area_3.poy=0;
		Area_3.poX=320;
		Area_3.poY=200;
		
		Area_4.pox=20;// other side down
		Area_4.poy=200;
		Area_4.poX=320;
		Area_4.poY=400;
		
		Area_1.pox=320; // our side up
		Area_1.poy=0;
		Area_1.poX=620;
		Area_1.poY=200;
		
		Area_2.pox=320; // our side down
		Area_2.poy=200;
		Area_2.poX=620;
		Area_2.poY=400;
		
		Area_red.pox=570; // red area
		Area_red.poy=160;
		Area_red.poX=620;
		Area_red.poY=240;
	}
}
