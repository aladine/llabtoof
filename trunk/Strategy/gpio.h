#ifndef GPIO_H
#define GPIO_H

#include "xparameters.h"
#include "xgpio.h"
#include "xmk.h"


#define GPIO_DEVICE_ID		XPAR_BUTTONS_3BIT_DEVICE_ID 
#define GPIO_DEVICE_ID_1	XPAR_DIP_SWITCHES_4BIT_DEVICE_ID 
#define INTC_GPIO_INTERRUPT_ID	XPAR_INTC_0_GPIO_0_VEC_ID 
#define GPIO_CHANNEL1 1
#define INTR_DELAY	0x00FFFFFF
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
void GpioDriverHandler(void *CallbackRef);

int GpioIntrExample( XGpio* InstancePtr, u16 DeviceId,
			u16 IntrId, u16 IntrMask, u32 *DataRead);
int GpioSetupIntrSystem( XGpio* InstancePtr,
			u16 DeviceId, u16 IntrId, u16 IntrMask);
int GpioInputExample(XGpio *GpioInstance,u16 DeviceId, u32 *DataRead);

#endif
