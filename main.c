#include <stdio.h>
#include <stdlib.h>

int main()
{
    int run=1;
    int game_state,north_button;
    //-------------------------------------------------------------------
    //Initialisation of positions of players
    //-------------------------------------------------------------------
    while(run=1)
    {
        if(Read_Value(north_button)==1)
        {
            Initialisation_Beginning();
            run=0;
        }

    }
    //--------------------------------------------------------------------
    // Updating of the strategy
    //--------------------------------------------------------------------
    while(run==0)
    {
        if(Receive_Packet()==1)
        {
            //int action=Indentification_Packet(packet);
            switch Indentification_Packet(packet)
            {
                case 0:
                    Initialisation_Foul(team_ref); //--team ref helps to know which team has the ball
                    break;
                case 1:
                    Initialisation_Goal();
                    break;
                case 2:
                    Update_Strategy();
                    break;
            }
        }
    }
    return 0;
}

int Indentification_Packet(Packet packet)
{
    int action; //---0=foul  --1=goal ---2=game

    if(packet[0]==0) //---info
    {
        if(packet[3]==1) //---player
        {

        }
        else //--ball
        {

        }
        action=2;
    }
    else //---control
    {
        Team_ref=packet[1]; //--team which scored or committed the foul
        if(packet[2]==1) //--gaol or foul
            action=0;
        else
            action=1;
    }

    return action;
}

void Update_Strategy();
{
    Goalkeeper_Strategy();
    if(ball.x<=(pitch.x/2)
    {
        Defender_Strategy_1();
        Stricker_strategy_1();
    }
    else
    {
        Defender_Strategy_2();
        Stricker_strategy_2();
    }
}

