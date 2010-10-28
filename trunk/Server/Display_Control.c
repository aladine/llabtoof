#include "xio.h"
#include "xtft.h"
#include <xparameters.h>
#include <pthread.h>
#include <errno.h>
#include "xmbox.h"
#include "display.h"
#include "controlling.h"
#include "xintc.h"
#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID

#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define PCK_SIZE         16
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define YELLOW 0x000000ff

#define BLUE 0x000000ff
#define WHITE 0x00ffffff
#define BLACK 0x00000000

#define TEAM_A 0
#define TEAM_B 1
static XTft TftInstance;


XMbox xmbox;
XMbox_Config *xmbox_ConfigPtr;

int rcvmsg[PCK_SIZE];
int rcvd;
int scoreA,foulA,scoreB,foulB;
GameState local_gs;
GameState prev_gs;
/*

typedef int SpecialEvent;
typedef struct
{
        unsigned x_pos, y_pos;
        unsigned char direction;
        unsigned char speed;
        unsigned char kick_direction; //changed :)
        unsigned char kick_speed;
}
Player;

typedef struct
{
        unsigned  x_pos, y_pos;
        unsigned char direction;
        unsigned char speed;
}
Ball;

typedef Player Team[5]; //strange syntax but probably correct

typedef struct
{
        Ball ball;
        Team players[2];
        SpecialEvent special;
}
GameState;

*/
volatile int taskrunning;


void tft_display_team(XTft *Tft,GameState gs,int j,int col)
{
	int i;
	for (i=0;i++;i<5)
	Draw_Player(Tft,gs.players[j][i].x_pos,gs.players[j][i].x_pos,col);
}

void tft_displaycontrol(XTft *Tft)
{
	tft_display_team(Tft,local_gs,TEAM_A,BLUE);
	tft_display_team(Tft,local_gs,TEAM_B,YELLOW);
	Draw_Ball(Tft,local_gs.ball.x_pos,local_gs.ball.y_pos,GREEN);
}

void tft_clear(XTft *Tft,GameState gs)
{
	tft_display_team(Tft,gs,TEAM_A,BLACK);
	tft_display_team(Tft,gs,TEAM_B,BLACK);
	Draw_Ball(Tft,gs.ball.x_pos,gs.ball.y_pos,BLACK);
}

void display_score(XTft *Tft,SpecialEvent sp)
{
	//GameState.game_time = sp % 0x0400;
  scoreA +=  sp / 0x8000;
  scoreB +=  sp / 0x4000 % 0x8000;    
  foulA  +=  sp / 0x2000 % 0x4000;        
  foulB +=  sp / 0x1000 % 0x2000; 
  tft_display_score(Tft,scoreA,foulA,scoreB,foulB); 
}
void control() {
  int ret,i,j;
  while(1) 
  {    
  
	 print("-- Display: Start Mailbox --\r\n");
	
    /* * Mailbox receiving */
		int nbytes = 0;		
		while (nbytes < PCK_SIZE) 
		{
		
		  (void)XMbox_Read(&xmbox, (u32*)(rcvmsg + nbytes), PCK_SIZE - nbytes, &rcvd);
		  nbytes += rcvd;
		}
		ret = XMbox_IsEmpty(&xmbox); 
		if (ret == 0) print("asdf");
		//prev_gs=local_gs;
		local_gs.ball.x_pos=rcvmsg[0];
				print("Display: Received from Processor 1 ");

		local_gs.ball.y_pos=rcvmsg[1];
		

	for (i = 0; i < 2; i++)
	      for (j = 0; j < 5; j++)
  		{
			local_gs.players[j][i].x_pos=rcvmsg[10*i+2*j+2];
			local_gs.players[j][i].y_pos=rcvmsg[10*i+2*j+3];
 		}
		
		local_gs.special=rcvmsg[22];
		print("Display: Received from Processor 1 ");
		
	//tft_clear(&TftInstance,prev_gs);

	// tft_displaycontrol(&TftInstance);
	 print("-- Screen Cleared --\r\n");
	 
		
	 /* end of mailbox */
	 
  }
  
}




int main (void) 
{

 int ret;
  
  print("-- Entering main() --\r\n");
  
  print("--Initialized --\r\n");

  /* * Init of XTFT Device */
  XStatus Status;
  XTft_Config *TftConfigPtr;
  
  /*
	* Get address of the XTft_Config structure for the given device id.
	*/
  print("\r Start Init of TFT \r\n");
  TftConfigPtr = XTft_LookupConfig(XPAR_XPS_TFT_0_DEVICE_ID);

  if (TftConfigPtr == (XTft_Config *)NULL) {
    print("\r Failed 1 \r\n");
	 return XST_FAILURE;
  }

  /*
	* Initialize all the TftInstance members and fills the screen with
	* default background color.
	*/
  print("\r Start to init \r\n");
  Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr, TftConfigPtr->BaseAddress);
  print("\r Init Success \r\n");
  if (Status != XST_SUCCESS) {
      print("\r Failed 2 \r\n");
		return XST_FAILURE;
  }
   // print("-- Screen Cleared --\r\n");
  /* * End of Init XTFT Device */
  
 Init(&TftInstance);
 display_score(&TftInstance, local_gs.special);
  
  print("\r Success 2 \r\n");
 // XTft_ClearScreen(&TftInstance);

  
  /* * Init of XMailbox */
  xmbox_ConfigPtr = XMbox_LookupConfig(XPAR_MBOX_0_DEVICE_ID );
  if (xmbox_ConfigPtr == (XMbox_Config *)NULL){
		print ("\tLookupConfig Failed\r\n");
		return XST_FAILURE;
	}
	
	Status = XMbox_CfgInitialize(&xmbox, xmbox_ConfigPtr, xmbox_ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS){
		print ("\tXMbox_CfgInitialize Failed.\r\n");
		return Status;
	}
   /* * End of init XMbox */
	
	XMbox_Flush(&xmbox); 

	
	


	

	
control();



}
