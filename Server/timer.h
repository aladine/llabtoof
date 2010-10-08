

#include "xparameters.h"
#include "xtmrctr.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID

/*
 * This example only uses the 1st of the 2 timer counters contained in a
 * single timer counter hardware device
 */
#define TIMER_COUNTER_0	 0

int TmrCtrPolledExample(u16 DeviceId, u8 TmrCtrNumber);

XTmrCtr TimerCounter; /* The instance of the Tmrctr Device */



