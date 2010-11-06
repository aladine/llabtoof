#include "msgqueue_api.h"

// **** this int alfa_id should be put as a global var 
int alfa_id;
// ****
// **** this is the testing definition
#define key_t int
// **** 
key_t alfa_key = 1;

/*
void msgqueue_send(GameState *msg) 
{ 
  
  int ret;
  // send data to message queue of consumer 
  ret = msgsnd(alfa_id, msg, sizeof(GameState), 0); // blocking send
}


GameState * msgqueue_recv(GameState* disp_msg) 
{
   msgrcv(alfa_id, disp_msg, sizeof(disp_msg), 0, 0);
	return disp_msg;
}

void msgqueue_init() 
{ 
  alfa_id = msgget(alfa_key, IPC_CREAT);
  //xil_printf("alfa_id %d\r\n", alfa_id);
}
*/
