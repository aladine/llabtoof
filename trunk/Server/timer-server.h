#ifndef MSGQUEUE_API_H
#define MSGQUEUE_API_H
#include "controlling.h"
#include "xtmrctr.h"
#include "xintc.h"
#include "xil_exception.h"

void timer_a_int_handler(void * baseaddr_p) ;
int Timer_Init(XTmrCtr *myTimer);
//int Interrupt_Enable(XIntc * myIntc)	;
int Timer_Start_Count(XTmrCtr *myTimer);


#endif



