/*
*/
#include "msgqueue_api.h"

// **** this int alfa_id should be put as a global var 
int alfa_id;
// ****
// **** this is the testing definition
#define key_t int
// **** 
key_t alfa_key = 1;
void msgqueue_send(uartdata* msg)
{
    
}
int msgqueue_recv(uartdata * disp_msg) 
{
     disp_msg -> data[0] = 0x82;
     disp_msg -> data[1] = 0x0F;     
     disp_msg -> data[2] = 0x0F;
     disp_msg -> data[3] = 0x00;  
}
/*
void msgqueue_send(uartdata *msg) 
{  
  struct uartdata send_msg;  
  int ret;
  send_msg.data[0] = msg->data[0];
  send_msg.data[1] = msg->data[1];
  send_msg.data[2] = msg->data[2];
  send_msg.data[3] = msg->data[3];
  //pthread_mutex_lock (&uart_mutex);
  //xil_printf("\rSend ID : %d , X = %d ,  Y = %d \r", send_msg.id, send_msg.x,send_msg.y);
  //pthread_mutex_unlock (&uart_mutex);
    // send data to message queue of consumer 
  ret = msgsnd(alfa_id, &send_msg, sizeof(send_msg), 0); // blocking send
  if(ret!=0)
  {		
    //pthread_mutex_lock (&uart_mutex);
	//print("\rERROR!! could not send data ");
    //pthread_mutex_unlock (&uart_mutex);
  }
  else
  {		
    //pthread_mutex_lock (&uart_mutex);
    //print("\rSend Message successfully !!");
    //pthread_mutex_unlock (&uart_mutex);
  }
}


int msgqueue_recv(uartdata * disp_msg) 
{
    if (msgrcv(alfa_id, disp_msg, sizeof(disp_msg), 0, 0) != sizeof(disp_msg)) 
	{
	  //pthread_mutex_lock (&uart_mutex);
	  //xil_printf("\rDisplay: msgrcv returns error: %d\r\n", errno);
	  //pthread_mutex_unlock (&uart_mutex);
	  return 0;
    }
    // print message
	//pthread_mutex_lock (&uart_mutex);
    //xil_printf ("\rDisplay received : Player ID = %d, x = %d  y= %d  \r\n", disp_msg.id, disp_msg.x,disp_msg.y);
    //pthread_mutex_unlock (&uart_mutex);
	return 1;
}

void msgqueue_init() 
{ 
  alfa_id = msgget (alfa_key, IPC_CREAT);
  //xil_printf("alfa_id %d\r\n", alfa_id);
}
*/
