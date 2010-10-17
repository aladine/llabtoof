/* players.c - represents the Team controllers in the project.
 *
 * Copyright 2010 Per Kristian Kjoell <a0075664@nus.edu.sg>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * What I(Per) have done so far is:
 *  -Taken the code from lab2 and added to/edited it.
 *  -I have added another thread as player 5.
 *  -I have created a struct for the players that contain all the different bitsequences(e.g team_id) (given to us in the communication datasheet
 * here: http://wiki.nus.edu.sg/display/ee4214/Communication+Protocol+and+Other+Details ) and implemented them as members in the struct.
 *  -I created an array(5x32) that can store all the comm.bits. I'm thinking that the playerthreads can work with the struct, and then
 * update the array with the member data. Then the communication part have to tranfer this info to the server. I'm wondering if we should use
 * two arrays, one for incoming and one for outgoing traffic or just use one with mutexes or something. Anyway my plan was just to implement
 * something real simple at first to test communication and the simplest possible positioning/movement of the players. The simplest
 * tactic would just be to make all the players run towards the ball.
 *  -I have made each player create their own struct.
 *  -I have made a function that initializes position. Currently its the only thing the players do. I tried to make them position
 * themselves in a straight line at the halfway line.
 *  -More comments and explanation is commented in the code.
 *
 *
 * TODO :	Fix all my mistakes.
 * 			Write a function that takes the struct info and puts it into the bitarray, in the correct order. edit : NO THIS WILL BE DONE IN I/O.
 * 			Establish communication by using uartlite API, rightclick the RS2323 in the system assembly view to find the API.
 * 			(could for example transfer one line of the array at the time)
 * 			Fix thread priorities. Currently player1 has top priority and player5 the lowest.
 * 			Create function that move players towards the ball.
 * 			Expand on stuff until it resembles a tactic ^^.
 * 			Lots more.
 */
// 2morrow : implement general fixes, i.e create static structurs and implement a function that updates the array. Maybe add
// ball to array. Use Dans functions to create primitive functions: Locate ball(pos, direction), move, kick(direction, speed),
// stop, wander, identify_zone(defense/attack), move_behind_ball(defend). Create a goalkeeper role. Fix dynamic priorites.
#include "xmk.h"
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include "structures.h"

struct sched_param sched_par; //Dont know how this works yet.
pthread_attr_t attr; // I think this atttrbute is used to give threads different priorities. Taken from lab2.
pthread_t tid1, tid2, tid3, tid4, tid5;
//declare mutex here. Init in main_prog.
pthread_mutex_t sw_mutex; //software mutex. unused sofar but useful :P.

struct ball ball; //spawns ball. I don't think ball needs a thread of its own since we have no "direct" control of the ball.

// UPDATING AND COMMUNICATION : START EDIT? :ALL OF THIS COMMUNICATION WILL BE DONE BY THE I/O MODULE MADE BY ADRIEN. ILL LET THE CODE STAY
// HERE FOR NOW. SAME WITH THE UPDATING FUNCTIONS.

// Create an array that contains 5 player communication vectors, each with 32 bit. Maybe use structure instead? This gives actual bits?
// Player 1 is in player_array[0][x] etc. Ball is in d[6].
// Each thread will save the structure members to the correct bitpositions in this array, corresponding to the communication protocol given.
// Example : At initialization player1 stores his x position in player_array[0][24-15]. The bit sequence should be taken from player1.x_pos
unsigned char data_from_server[6][32]; //Contains communication "bits" from server to ctrlr. I.e x & y position of players and ball.
unsigned char data_from_player[5][32]; //Contains communication "bits" from ctrler to server.I.e direction/speed/kick. Init. packet?
//unsigned char opposition_players[5][32];

//COMMUNICATION HERE?
// We need to make communication functions that transfer the information in this array(or a better structure :P)
// to the server. Note : Check the uartlite api by righ clicking on RS232 in the system assembly view. Lots of useful functions there.

//mutex here?
//void update_data_from_player(struct player *player_x){ //information that goes from player to server.
//
//}
//the two update functions update the structs with values taken from data_from_server. This is information from server to player.
void update_player_position(int i,struct player *player) { //i determines player. player 1-5 = int 0-4.
	//Convert 10 position bits to integer,(temp) and stores it in the pos variables.
	//A bit lol to convert from "bits" to integer this way ;P ? Create a separate func?
	int temp = 0;
	int j,k;
	for(j = 0; j<10; j++){
		temp = temp + (data_from_server[i][15+j]*(2^j));
	}
	player->x_pos = temp;
	temp=0;
	for(k = 0; k<10; k++){
		temp = temp + (data_from_server[i][5+k]*(2^k));
	}
	player->y_pos = temp;
}
void update_ball(struct ball *ball){
	//Convert bits to integer(temp) according to given comm. protocol, and stores it in the pos variables.
	//A bit lol to convert from "bits" to integer this way ;P ? Create a separate func?
	//y_pos only goes to 400. Higher resolution than 1 pixel? Hopefully not^^
	int temp = 0;
	int j,k,l;
	for(j = 0; j<10; j++){
		temp = temp + (data_from_server[6][15+j]*(2^j));
	}
	ball->x_pos = temp;
	temp=0;
	for(k = 0; k<10; k++){
		temp = temp + (data_from_server[6][5+k]*(2^k));
	}
	ball->y_pos = temp;
	//convert direction bits to integer.
	temp=0;
	for(l = 0; l<4; l++){
		temp = temp + (data_from_server[6][25+l]*(2^l));
	}
	ball->direction = temp;
	//convert speed bits to integer.
	temp=0;
	if(data_from_server[6][4]==0){ //if msb == 0.
		temp = (data_from_server[6][3] * 20) + (data_from_server[6][2] * 10) + (data_from_server[6][1] * 5);
	}
	else{	//msb = 1 and equal to 40. errorcatcher?
		if( (data_from_server[6][3] + data_from_server[6][2]) >= 1 ) //bit 3 or 2 = 1
			temp = 40 + (data_from_server[6][3] * 30) + (data_from_server[6][2] * 10) + (data_from_server[6][1] * 10);
		else //eroorocatcher?
			temp = 40 + (data_from_server[6][1] * 5);
	}
	ball->speed = temp;
}




// UPDATING AND COMMUNICATION : END


// A function to initialize the position of the players. Used at the start of the game.
// All the players get the same x position, y position is decided by player_id. Currently standing in a line on the middle of the field.
// Should expand on this so that different player roles get positioned in different positions.

void initialize_position(struct player *player){ //add player role to this function?
	player->packet_type = 1; //Sets the packet_type to initalization.
	player->x_pos=300; //All the players stand at (or close to) the halfwayline. I think the x coordinates go from goal to goal.
	//could use a switch here.
	if (player->player_id == 1) //player 1 gets this pos
		player->y_pos=100; //random y position. this should be at the throw in line.
	else if (player->player_id == 2) //player 2 gets this pos
		player->y_pos=150;	//random y position.
	else if (player->player_id == 3) //player 3 gets this pos
		player->y_pos=200;	//random y position.
	else if (player->player_id == 4) //player 4 gets this pos
		player->y_pos=250;   //random y position.
	else if (player->player_id == 5) //player 5 gets this pos
		player->y_pos=300;	//random y position. this should be at the throw in line.
	else
		player->y_pos=200;
}

//PRIMITIVE PLAYER ACTIONS - START :
//calculate distance between player and ball.
float distance_sq(struct player *player,struct ball *ball){
	return (((player->x_pos)-(ball->x_pos))*((player->x_pos)-(ball->x_pos))+ ((player->y_pos)-(ball->y_pos))*((player->y_pos)-(ball->y_pos)));
}

//check if the player possess the ball
int possess_ball(struct player *player,struct ball *ball){
	if (distance_sq(player,ball)<=225) //where is this value from?
		return 1;
	else return 0;
}
//move player. Valid inputs : Direction(0-15), Speed(0,5,10,...,50). Expand to catch wrong inputs?
void move(struct player *player, unsigned int direction, unsigned int speed){
	player->direction = direction;
	player->speed = speed;
}
//shoots the ball. Valid inputs: Direction(0-15) but only "in front of the player?". Speed(0,5,10,...,50,60,...,100).
// Function will set player speed to speed/2 since ball will get twice this speed. Instant accel?
void shoot(unsigned int direction, unsigned int speed){
	player->direction = direction;
	player->speed = speed;
	//some kick variable.
}
//PRIMITIVE PLAYER ACTIONS - END.



//Player 1 behavior.
void* player_1 () {
	//Start by creating player struct. Keeping this outside the while so it only happens once, but I'm sure we'll change it later :).
	struct player player1;
	player1.team_id=0; //Keeping all players on Team A for now.
	player1.player_id=1; //player1 = 0001, player2=0010, player3=0011, player4=0100,player5=0101.
	initialize_position(&player1); //Points player1 to the function initialize_position. Should move this inside the loop after test.
	// update_struct_to_array(&player1); //This does not exist yet, maybe a dumb solution but works as a basic starting point.
	// Im thinking that we can hardware lock the array between server and client, And then the server can just go to the array and get info at will.
  while(1) {
	//do nothing for now ;(
	//Should do a state check: If initialize, then run initialize_position, else run a function that determines movement(e.g run to ball).
  }
}

//Player 2 behavior.
void* player_2 () {
	//Start by creating player struct. Keeping this outside the while so it only happens once, but I'm sure we'll change it later :).
	struct player player2;
	player2.team_id=0; //Keeping all players on Team A for now.
	player2.player_id=2; //player1 = 0001, player2=0010, player3=0011, player4=0100,player5=0101.
	initialize_position(&player2); //Points player2 to the function initialize_position. Should move this inside the loop after test.
	// update_struct_to_array(&player2); //This does not exist yet, maybe a dumb solution but works as a basic starting point.
	// Im thinking that we can hardware lock the array between server and client, And then the server can just go to the array and get info at will.
  while(1) {
	//do nothing for now ;(
	//Should do a state check: If initialize, then run initialize_position, else run a function that determines movement(e.g run to ball).
  }
}

//Player 3 behavior.
void* player_3 () {
	//Start by creating player struct. Keeping this outside the while so it only happens once, but I'm sure we'll change it later :).
	struct player player3;
	player3.team_id=0; //Keeping all players on Team A for now.
	player3.player_id=3; //player1 = 0001, player2=0010, player3=0011, player4=0100,player5=0101.
	initialize_position(&player3); //Points player3 to the function initialize_position. Should move this inside the loop after test.
	// update_struct_to_array(&player3); //This does not exist yet, maybe a dumb solution but works as a basic starting point.
	// Im thinking that we can hardware lock the array between server and client, And then the server can just go to the array and get info at will.
  while(1) {
	//do nothing for now ;(
	//Should do a state check: If initialize, then run initialize_position, else run a function that determines movement(e.g run to ball).
  }
}

//Player 4 behavior.
void* player_4 () {
	//Start by creating player struct. Keeping this outside the while so it only happens once, but I'm sure we'll change it later :).
	struct player player4;
	player4.team_id=0; //Keeping all players on Team A for now.
	player4.player_id=4; //player1 = 0001, player2=0010, player3=0011, player4=0100,player5=0101.
	initialize_position(&player4); //Points player4 to the function initialize_position. Should move this inside the loop after test.
	// update_struct_to_array(&player4); //This does not exist yet, maybe a dumb solution but works as a basic starting point.
	// Im thinking that we can hardware lock the array between server and client, And then the server can just go to the array and get info at will.
  while(1) {
	//do nothing for now ;(
	//Should do a state check: If initialize, then run initialize_position, else run a function that determines movement(e.g run to ball).
  }
}

//Player 5 behavior.
void* player_5 () {
	//Start by creating player struct. Keeping this outside the while so it only happens once, but I'm sure we'll change it later :).
	struct player player5;
	player5.team_id=0; //Keeping all players on Team A for now.
	player5.player_id=5; //player1 = 0001, player2=0010, player3=0011, player4=0100,player5=0101.
	initialize_position(&player5); //Points player5 to the function initialize_position. Should move this inside the loop after test.
	// update_struct_to_array(&player5); //This does not exist yet, maybe a dumb solution but works as a basic starting point.
	// Im thinking that we can hardware lock the array between server and client, And then the server can just go to the array and get info at will.
  while(1) {
	//do nothing for now ;(
	//Should do a state check: If initialize, then run initialize_position, else run a function that determines movement(e.g run to ball).
  }
}

int main (void) {

  //Start Xilkernel
  xilkernel_main ();

  //Control does not reach here

}

int main_prog(void) {   // This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)

  int ret;
  //create and initialize software mutex.
  //sw_mutex is a software mutex. This initializes it. This mutex can be locked and unlocked when the threads read and write to the dataarrays.
  pthread_mutex_init(&sw_mutex, NULL);

  print("-- Entering main() --\r\n");




  //LAUNCHING THREADS START - should make this a separate function for readability. Currently (17.09) just copied from lab 2. Priority
  // decreasing from 1-5. I.e player1 has higher priority then player5. Need to be changed later.
  pthread_attr_init (&attr);
 print("--Initialized --\r\n");

  // Priority 1 for thread 1
  sched_par.sched_priority = 1;
  pthread_attr_setschedparam(&attr,&sched_par);
 print("--Priority Set --\r\n");

 //start thread 1
  ret = pthread_create (&tid1, &attr, (void*)player_1, NULL);
	print("--returned --\r\n");
 if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching player_1...\r\n", ret);
  }
  else {
    xil_printf ("Thread 1 launched with ID %d \r\n",tid1);
  }

  // Priority 2 for thread 2
  sched_par.sched_priority = 2;
  pthread_attr_setschedparam(&attr,&sched_par);

  //start thread 2
  ret = pthread_create (&tid2, &attr, (void*)player_2, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching player_2...\r\n", ret);
  }
  else {
    xil_printf ("Thread 2 launched with ID %d \r\n",tid2);
  }

   // Priority 3 for thread 3
  sched_par.sched_priority = 3;
  pthread_attr_setschedparam(&attr,&sched_par);

 //start thread 3
  ret = pthread_create (&tid3, &attr, (void*)player_3, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching player_3...\r\n", ret);
  }
  else {
    xil_printf ("Thread 3 launched with ID %d \r\n",tid3);
  }

  // Priority 4 for thread 4
  sched_par.sched_priority = 4;
  pthread_attr_setschedparam(&attr,&sched_par);

  //start thread 4
  ret = pthread_create (&tid4, &attr, (void*)player_4, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching player_4...\r\n", ret);
  }
  else {
    xil_printf ("Thread 4 launched with ID %d \r\n",tid4);
  }

  // Priority 5 for thread 5
  sched_par.sched_priority = 5;
  pthread_attr_setschedparam(&attr,&sched_par);

  //start thread 5
  ret = pthread_create (&tid5, &attr, (void*)player_5, NULL);
  if (ret != 0) {
      xil_printf ("-- ERROR (%d) launching player_5...\r\n", ret);
  }
  else {
    xil_printf ("Thread 5 launched with ID %d \r\n",tid5);
  }
  //LAUNCHING THREADS END




} //end main.

