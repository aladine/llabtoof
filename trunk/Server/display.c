#include "xparameters.h"
#include "xio.h"
#include "xtft.h"
#include "display.h"
#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID

#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define BLUE 0x000000ff
#define WHITE 0x00ffffff
  

static XTft TftInstance;

//to shift value of x on display
int X(int x){
	return x+20;
}

//to shift value of y on display
int Y(int y){
	return y+40;
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
      }	   XTft_SetPixel(Tft, i, j, col);
      return 0;
   }
   return 1;

}

int Draw_Player(XTft *Tft, int x, int y,unsigned int col){
	XTft_DrawSolidBox(Tft,x-7,y+1,x+7,y-1,col);
	XTft_DrawSolidBox(Tft,x-6,y+3,x+6,y-3,col);
	//XTft_DrawSolidBox(&Tft,x-5,y+3,x+5,y-3,col);
	XTft_DrawSolidBox(Tft,x-5,y+5,x+5,y-5,col);
	//XTft_DrawSolidBox(Tft,x-3,y+5,x+3,y-5,col);
	XTft_DrawSolidBox(Tft,x-3,y+6,x+3,y-6,col);
	XTft_DrawSolidBox(Tft,x-1,y+7,x+1,y-7,col);

}


int Draw_Border(XTft *Tft){

	TftDrawLine(Tft,20,40,20,440,WHITE);
	TftDrawLine(Tft,20,40,620,40,WHITE);
	TftDrawLine(Tft,620,440,620,40,WHITE);
	TftDrawLine(Tft,20,440,620,440,WHITE);
	
	TftDrawLine(Tft,40,40,20,60,WHITE);
	TftDrawLine(Tft,600,40,620,60,WHITE);
	TftDrawLine(Tft,40,440,20,420,WHITE);
	TftDrawLine(Tft,600,440,620,420,WHITE);
	XTft_DrawSolidBox(Tft,0,210,20,270,GREEN);
	XTft_DrawSolidBox(Tft,620,210,640,270,GREEN);
	XTft_DrawSolidBox(Tft,20,190,70,290,RED);
	XTft_DrawSolidBox(Tft,570,190,620,290,RED);

}

int Draw_Ball(XTft *Tft, int x, int y,unsigned int col){
	
	XTft_DrawSolidBox(Tft,x-5,y+1,x+5,y-1,col);
	XTft_DrawSolidBox(Tft,x-4,y+2,x+4,y-2,col);
	XTft_DrawSolidBox(Tft,x-3,y+3,x+3,y-3,col);
	XTft_DrawSolidBox(Tft,x-2,y+4,x+2,y-4,col);
	XTft_DrawSolidBox(Tft,x-1,y+5,x+1,y-5,col);
}

float distance(int x, int y, int a, int b){
return ((x-a)*(x-a)+(y-b)*(y-b));
}

int in_circle(int x, int y){
if (distance(x,y,DISPLAY_COLUMNS/2,DISPLAY_ROWS/2)>50*50)
return 0;
return 1;
}


int Draw_Circle(XTft *TftInstance,  unsigned int col){
	int Xmin = DISPLAY_COLUMNS/2;
	int Ymin = DISPLAY_ROWS/2;
	int Index1,Index2;
		for (Index1 = Xmin-50; Index1 <= Xmin+50; Index1++) {		
			for (Index2 = Xmin-50; Index2 <= Xmin+50; Index2++) {
				if (in_circle(Index1,Index2)==1) 	   XTft_SetPixel(TftInstance, Index1, Index2, col);
			
			}
		}	
}


void Init(XTft *TftInstance){  

Draw_Border(TftInstance);
//Draw_Player(TftInstance,300,300,RED);
//Draw_Ball(TftInstance,400,400,RED);
Draw_Circle(TftInstance,GREEN);
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

