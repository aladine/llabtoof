#ifndef FUNCTION_H
#define FUNCTION_H

#include "structures.h"
#include "io/io_player.h"
#include "io/io_structures.h"
/***********************************************************************************************************************************************/
//////////////STRUCTURES///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
struct Player0 // data that will be sent to the server
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
struct Ball_Our // structure defining the ball
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




/***********************************************************************************************************************************************/
//////////////DEFINITIONS///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
typedef struct Player0 Player0;
typedef struct Player_sent Player_sent;
typedef struct Player_up Player_up;
typedef struct Ball_Our Ball_Our;
typedef struct Area Area;
/***********************************************************************************************************************************************/
//////////////GLOBALS///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
IOPlayermanager io_stuff; 
Area Area_1,Area_2,Area_3,Area_4;
Area Area_red;
int team;
Ball_Our ball;
Player0 player[5];
Player_sent player_sent[5];
Player_up player_up[5];
GameState New_State;
Player_sent oplayer[5];
int BC0, BC1, BC2, BC3, BC4;
int F0;
//////////////////////////////////////////////////
//////NEW With Adrien/////////////////////////////////////
/////////////////////////////////////////////////
//IOPlayermanager io_stuff;
//GameState State_Received;
//GameState New_State;
/***********************************************************************************************************************************************/
///////DECLATRATIONS////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************************************************************/
void Initialisation_of_Areas(); // TESTED
void Update_Strategy();  
int Choose_Strategy();
void Goalkeeper_Strategy_Defense();
void Defender_Strategy_Defense(int id);  
void Striker_Strategy_Attack(int id);
void Pass_Stricker_free_A(int id);
void Pass_Stricker_free_B(int id);
int Which_Direction_About_Player_To_Ball(int id);
int Number_Opponent_Player_Area(int area); //TESTED
int Where_Is_The_Opponent_Player(int id);//TESTED
void Send_Update_Queue(int id);
int Distance_Player_Ball(int id); //TESTED
void Shoot(int id); 
int Which_Player_In_My_Area(int My_Area); //TESTED
int How_Many_Opponents_In_The_Other_Area(int My_Area);//TESTED
void Pass_To_The_Other_Striker(int id); 
int Where_Is_The_player(int id); //TESTED
int Where_Is_The_Ball(); //TESTED
void Run_Average(int id);
//////////////////////////////////////////////////
//////PART B/////////////////////////////////////
/////////////////////////////////////////////////
void Initialisation_Beginning(); //TESTED
void Initialisation_Goal(); //TESTED 
void Initialisation_Foul(int team_ref); //TESTED
void Construct_Initial_Packet_Player0(int Team); //TESTED
void Construct_Initial_Packet_Player1(int Team); // TESTED
void Construct_Initial_Packet_Player2(int Team); // TESTED
void Construct_Initial_Packet_Player3(int Team); // TESTED
void Construct_Initial_Packet_Player4(int Team); // TESTED
void Send_Queue();   
//////////////////////////////////////////////////
//////PART C/////////////////////////////////////
/////////////////////////////////////////////////
void Run_Def(int id);
void Run_Quick(int id);
void Run(int id);
int Which_Direction_About_Player_To_Player(int idA,int idB );
int Which_Direction_About_Player_To_Red_Area(int id);
int Which_Direction_About_Ball_To_Goal(int id);
//////////////////////////////////////////////////
//////NEW WITH ADRIEN/////////////////////////////
/////////////////////////////////////////////////
void callback(GameState *State_Received);
void send_to_io();
void Copy_State_Received();

#endif 
