#include "xparameters.h"
#include "xio.h"
#include "xtft.h"
#include "display.h"
#include "controlling.h"

static int scoreA,foulA,scoreB,foulB,time;

/* check anything in the field*/
int in_border(int x, int y){
	if((x>=20) && (x<=620) && (y>=0) && (y<=400) && (x+y>=40) && (600-x+y>=0) && (600-x+400-y>=0) && (x+400-y>=40)) return 1;
	return 0;
  	
}

/*display time counting down 3 2 1*/
void count_down(XTft *Tft,int count){
  XTft_SetPosChar(Tft, 470,420);
  XTft_SetColor(Tft, WHITE,BLACK);
  TftWriteString(Tft, (u8*)"Start:");
    XTft_SetPosChar(Tft, 530,420);
  XTft_SetColor(Tft, WHITE,BLACK);
  XTft_Write(Tft,'0'+count);
}


void XTft_DrawBox(XTft *Tft, int xmin, int ymin, int xmax, int ymax, unsigned int col)
{		
int i,j;
		for (i=xmin; i<=xmax; i++) {
		   XTft_SetPixel(Tft, i, ymin, col);
		   XTft_SetPixel(Tft, i, ymax, col);
			}
		for (j=ymin; j<=ymax; j++) {
		   XTft_SetPixel(Tft, xmin, j, col);
		   XTft_SetPixel(Tft, xmax, j, col);
			}
}

void XTft_DrawRoundBox(XTft *Tft, int xmin, int ymin, int xmax, int ymax, unsigned int col)
{		
int i,j;
		for (i=xmin+2; i<=xmax-2; i++) {
		   XTft_SetPixel(Tft, i, ymin, col);
		   XTft_SetPixel(Tft, i, ymax, col);
			}
		for (j=ymin+2; j<=ymax-2; j++) {
		   XTft_SetPixel(Tft, xmin, j, col);
		   XTft_SetPixel(Tft, xmax, j, col);
			}
		XTft_SetPixel(Tft, xmin+1, ymin+1, col);
		XTft_SetPixel(Tft, xmax-1, ymin+1, col);
		XTft_SetPixel(Tft, xmin+1, ymax-1, col);
		XTft_SetPixel(Tft, xmax-1, ymax-1, col);
}

void tft_display_score(XTft *Tft,int _scoreA,int _foulA, int _scoreB, int _foulB, int time){
 
	//TftWriteString(Tft, (u8*)"Time : 0" + round(time/60) + ":" + time%60 );
	int minu,sec,minu1,minu2, sec1,sec2;
	minu=(int) (time/60);
	sec=time %60;

  minu1=(int) (minu/10);
  minu2=time %10;
  sec1=(int) (sec/60);
  sec2=sec %60;

  XTft_SetPosChar(Tft, 140,420);
  XTft_SetColor(Tft, YELLOW,BLACK);
  TftWriteString(Tft, (u8*)"TEAM A" );

  XTft_SetPosChar(Tft, 350,420);
  XTft_SetColor(Tft, BLUE,BLACK);
  TftWriteString(Tft, (u8*)"TEAM B" );
  
	XTft_SetPosChar(Tft, 150,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  TftWriteString(Tft, (u8*)"WIN" );

  XTft_SetPosChar(Tft, 360,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  TftWriteString(Tft, (u8*)"LOSE" );
  
  XTft_SetPosChar(Tft, 220,420);
  XTft_SetColor(Tft, WHITE,BLACK);
  TftWriteString(Tft, (u8*)"Score:" );
  
  XTft_SetPosChar(Tft, 220,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  TftWriteString(Tft, (u8*)"Foul:" );


 XTft_SetPosChar(Tft, 290,420);
  XTft_SetColor(Tft, YELLOW,BLACK);
  XTft_Write(Tft,'0'+scoreA);

  XTft_SetPosChar(Tft, 310,420);
  XTft_SetColor(Tft, BLUE,BLACK);
  XTft_Write(Tft,'0'+scoreB);

  XTft_SetPosChar(Tft, 290,440);
  XTft_SetColor(Tft, YELLOW,BLACK);
  XTft_Write(Tft,'0'+foulA);
  
  XTft_SetPosChar(Tft, 310,440);
  XTft_SetColor(Tft, BLUE,BLACK);
  XTft_Write(Tft,'0'+foulB);;

  XTft_SetPosChar(Tft, 470,420);
  XTft_SetColor(Tft, WHITE,BLACK);
  TftWriteString(Tft, (u8*)"  TIME    " );

XTft_SetPosChar(Tft, 480,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  XTft_Write(Tft,'0'+minu1);
XTft_SetPosChar(Tft, 490,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  XTft_Write(Tft,'0'+minu2);
XTft_SetPosChar(Tft, 500,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  XTft_Write(Tft,':');
XTft_SetPosChar(Tft, 510,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  XTft_Write(Tft,'0'+sec1);
XTft_SetPosChar(Tft, 520,440);
  XTft_SetColor(Tft, WHITE,BLACK);
  XTft_Write(Tft,'0'+sec2);
  
XTft_DrawRoundBox(Tft,210,410,330,460,WHITE);
XTft_DrawRoundBox(Tft,130,410,200,460,WHITE);
XTft_DrawRoundBox(Tft,340,410,410,460,WHITE);
XTft_DrawRoundBox(Tft,460,410,550,460,WHITE);//time
}

int Draw_Player(XTft *Tft, int x, int y,unsigned int col){
	if (x<7) x=7;
	if (y<7) y=7;
	XTft_DrawBox(Tft,x-7,y-1,x+7,y+1,col);
	XTft_DrawBox(Tft,x-6,y-3,x+6,y+3,col);
	XTft_DrawBox(Tft,x-5,y-5,x+5,y+5,col);
	XTft_DrawBox(Tft,x-3,y-6,x+3,y+6,col);
	XTft_DrawBox(Tft,x-1,y-7,x+1,y+7,col);
	XTft_DrawBox(Tft,x-4,y-4,x+4,y+4,col);
	XTft_DrawSolidBox(Tft,x-2,y-2,x+2,y+2,col);

}


int Draw_Border(XTft *Tft){
	//XTft_DrawSolidBox(Tft,20,0,620,400,GREEN);
	int i,j;
	for (i=20; i<=620; i++) {
         for (j=0; j<=400; j++) {
			if (in_border(i,j)==1) XTft_SetPixel(Tft, i, j, GREEN);
         }
      }
	
	TftDrawLine(Tft,20,20,20,380,WHITE);
	TftDrawLine(Tft,40,0,600,0,WHITE);
	TftDrawLine(Tft,620,380,620,20,WHITE);
	TftDrawLine(Tft,40,400,600,400,WHITE);
	
	TftDrawLine(Tft,40,0,20,20,WHITE);
	TftDrawLine(Tft,600,0,620,20,WHITE);
	TftDrawLine(Tft,40,400,20,380,WHITE);
	TftDrawLine(Tft,600,400,620,380,WHITE);
	
	
}

int Draw_Ball(XTft *Tft, int x, int y,unsigned int col){
	if (x<5) x=5;
	if (y<5) y=5;
	XTft_DrawBox(Tft,x-5,y-1,x+5,y+1,col);
	XTft_DrawBox(Tft,x-4,y-2,x+4,y+2,col);
	XTft_DrawBox(Tft,x-3,y-3,x+3,y+3,col);
	XTft_DrawBox(Tft,x-2,y-4,x+2,y+4,col);
	XTft_DrawBox(Tft,x-1,y-5,x+1,y+5,col);
   XTft_SetPixel(Tft, x, y, col);

}



float distance(int x, int y, int a, int b){
	return ((x-a)*(x-a)+(y-b)*(y-b));
}



int Draw_Circle(XTft *TftInstance,  unsigned int col){
	int Xmin = DISPLAY_COLUMNS/2 ;
	int Ymin = DISPLAY_ROWS/2 -20;
	int Index1,Index2;
		for (Index1 = Xmin-50; Index1 < Xmin+50; Index1++) {		
			for (Index2 = Ymin-50; Index2 < Ymin+50; Index2++) {
				if ((distance(Index1,Index2,Xmin,Ymin)>49*49-1) && (distance(Index1,Index2,Xmin,Ymin)<51*51+1)) 
				XTft_SetPixel(TftInstance, Index1, Index2, col);
			
			}
		}	
}


void tft_display_team(XTft *Tft,GameState gs,int j,int col)
{
	int i;
		
	for (i=0;i<5;i++)
	{ 
		//xil_printf(" ha %d %d \r\n",gs.players[j][i].x_pos,gs.players[j][i].y_pos);

	Draw_Player(Tft,gs.players[j][i].x_pos,gs.players[j][i].y_pos,col);
	}
}

void tft_displaycontrol(XTft *Tft,GameState gs)
{		

	tft_display_team(Tft,gs,TEAM_A,BLUE);
	tft_display_team(Tft,gs,TEAM_B,YELLOW);
	Draw_Ball(Tft,gs.ball.x_pos,gs.ball.y_pos,WHITE);//

}

void tft_clear(XTft *Tft,GameState gs)
{
	tft_display_team(Tft,gs,TEAM_A,GREEN);
	tft_display_team(Tft,gs,TEAM_B,GREEN);
	Draw_Ball(Tft,gs.ball.x_pos,gs.ball.y_pos,GREEN);
}

void display_score(XTft *Tft,SpecialEvent sp)
{
	//GameState.game_time = sp % 0x0400;
  time = sp % 0x0400;
  scoreA +=  sp / 0x8000;
  scoreB +=  sp / 0x4000 % 0x8000;    
  foulA  +=  sp / 0x2000 % 0x4000;        
  foulB +=  sp / 0x1000 % 0x2000; 
  tft_display_score(Tft,scoreA,foulA,scoreB,foulB,time); 
}

void UpdateScreen(XTft *Tft){  
	TftDrawLine(Tft,320,0,320,400,WHITE);
	XTft_DrawSolidBox(Tft,0,170,20,230,GREEN);
	XTft_DrawSolidBox(Tft,620,170,638,230,GREEN);
	XTft_DrawSolidBox(Tft,20,160,70,240,RED);
	XTft_DrawSolidBox(Tft,570,160,620,240,RED);
	Draw_Circle(Tft,WHITE);

}

void Init(XTft *TftInstance){  

Draw_Border(TftInstance);
Draw_Player(TftInstance,300,300,YELLOW);
Draw_Player(TftInstance,500,300,BLUE);

Draw_Ball(TftInstance,350,350,RED);
UpdateScreen(TftInstance);
}


 int TftWriteString(XTft *InstancePtr, const u8 *CharValue)
{	
	while (*CharValue != 0) {
		XTft_Write(InstancePtr, *CharValue);
		CharValue++;
	}

	return XST_SUCCESS;
}

int XTft_DrawSolidBox(XTft *Tft, int x1, int y1, int x2, int y2, unsigned int col)
{
   int xmin,xmax,ymin,ymax,i,j;

   if (x1 >= 0 &&
       x1 <= DISPLAY_COLUMNS-1 &&
       x2 >= 0 &&
       x2 <= DISPLAY_COLUMNS-1 &&
       y1 >= 0 &&
       y1 <= DISPLAY_ROWS-1 &&
       y2 >= 0 &&
       y2 <= DISPLAY_ROWS-1) {
      if (x2 < x1) {
         xmin = x2;
         xmax = x1;
      }
      else {
         xmin = x1;
         xmax = x2;
      }
      if (y2 < y1) {
         ymin = y2;
         ymax = y1;
      }
      else {
         ymin = y1;
         ymax = y2;
      }

      for (i=xmin; i<=xmax; i++) {
         for (j=ymin; j<=ymax; j++) {
	   XTft_SetPixel(Tft, i, j, col);
         }
      }	  
      return 0;
   }
   return 1;

}

 int TftDrawLine(XTft *InstancePtr, int ColStartPos, int RowStartPos,
			int ColEndPos, int RowEndPos, unsigned int PixelVal)
{
	int Slope;
	int YIntercept;
	int Xmin;
	int Ymin;
	int Xmax;
	int Ymax;
	int Index1;
	int Index2;
	int Mx;

	/*
	 * Check whether the given position of X,Y dimensions
	 * are below the limits of the screen.
	 */
	if (ColStartPos >= 0 && ColStartPos <= (DISPLAY_COLUMNS - 1) &&
		ColEndPos >= 0 && ColEndPos <= (DISPLAY_COLUMNS - 1) &&
		RowStartPos >= 0 && RowStartPos <= (DISPLAY_ROWS - 1) &&
		RowEndPos >= 0 && RowEndPos <= (DISPLAY_ROWS - 1)) {
		/*
		 * Check the exception case where slope can be infinite
		 * that is vertical line.
		 */
		if (ColEndPos-ColStartPos != 0) {
			/*
			 * Calculate slope.
			 */
			Slope = ((RowEndPos - RowStartPos) /
				(ColEndPos - ColStartPos) * 100000);

			/*
			 * Calculate y intercept.
			 */
			YIntercept = RowStartPos -
					((Slope / 100000) * ColStartPos);
		} else {
			/*
			 * Divide by zero.
			 */
			Slope = 0;
			YIntercept = (-1);
		}

		/*
		 * Update the min and max position by conditional checking.
		 */
		if (ColEndPos < ColStartPos) {
			Xmin = ColEndPos;
			Xmax = ColStartPos;
		} else {
			Xmin = ColStartPos;
			Xmax = ColEndPos;
		}
		if (RowEndPos < RowStartPos) {
			Ymin = RowEndPos;
			Ymax = RowStartPos;
		} else {
			Ymin = RowStartPos;
			Ymax = RowEndPos;
		}

		/*
		 * Increment X and Y position values and calculate
		 * slope at the corresponding x position. Check the condition
		 * for general line equation and set the pixel. Otherwise check
		 * for the case of vertical line.
		 */
		for (Index1 = Xmin; Index1 <= Xmax; Index1++) {
			Mx = (Slope * Index1) / 100000;
			for (Index2 = Ymin; Index2 <= Ymax; Index2++) {
				if ((Index2 - Mx) == YIntercept) {

					/*
					 * Calculate visible line.
					 */
					XTft_SetPixel(InstancePtr, Index1,
						 Index2, PixelVal);
				}
				else {
					/*
					 * Divide by zero.
					 */
					if((Slope == 0) &&
							(YIntercept == -1)) {
						/*
						 * Vertical line.
						 */
						XTft_SetPixel(InstancePtr,
							Index1, Index2,
							PixelVal);
					}
				}
			}
		}
		return XST_SUCCESS;
	} else {
		return XST_FAILURE;
	}
}
