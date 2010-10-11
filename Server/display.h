#ifndef DISPLAY_H
#define DISPLAY_H

#include "xparameters.h"
#include "xio.h"
#include "xtft.h"
#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID
#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480
#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define BLUE 0x000000ff

static XTft TftInstance;

int X(int );
int Y(int );
int XTft_DrawSolidBox(XTft *, int , int , int , int , unsigned int);
int Draw_Player(XTft *, int , int ,unsigned int );
int Draw_Border(XTft *);
int Draw_Ball(XTft *, int , int ,unsigned int);
float distance(int x, int y, int a, int b);
int in_circle(int x, int y);
int Draw_Circle(XTft *Tft,  unsigned int col);
void Init(XTft *TftInstance);
int TftDrawLine(XTft *, int , int ,int, int , unsigned int);

#endif
