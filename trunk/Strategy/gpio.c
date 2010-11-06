#include "gpio.h"
#include <sys/intr.h>

u16 GlobalIntrMask; /* GPIO channel mask that is needed by
			    * the Interrupt Handler */
volatile u32 IntrFlag; /* Interrupt Handler Flag */

void GpioDriverHandler(void *CallbackRef)
{
	XGpio *GpioPtr = (XGpio *)CallbackRef;
	XGpio MyDIP;
	int ret;
	//u32 a = XGpio_DiscreteRead(&Intc,  GPIO_CHANNEL1); 
	u32 DIPRead;

	IntrFlag = 1;
	/*
	 * Clear the Interrupt
	 */
	ret= GpioInputExample(&MyDIP,GPIO_DEVICE_ID_1,&DIPRead);
				xil_printf("DIP value is %X \r\n",DIPRead);

	XGpio_InterruptClear(GpioPtr, GlobalIntrMask);

}


int GpioIntrExample( XGpio* InstancePtr, u16 DeviceId,
			u16 IntrId, u16 IntrMask, u32 *DataRead)
{
	int Status;
	u32 delay;

	/* Initialize the GPIO driver. If an error occurs then exit */

	Status = XGpio_Initialize(InstancePtr, DeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = GpioSetupIntrSystem(InstancePtr,
				 DeviceId,
				 IntrId,
				 IntrMask);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	IntrFlag = 0;
	delay = 0;

	while(!IntrFlag && (delay < INTR_DELAY)) {
		delay++;
	}

	disable_interrupt(IntrId);
	//GpioDisableIntr(IntcInstancePtr,
		//	InstancePtr,
		//	IntrId,
		//	IntrMask);

	*DataRead = IntrFlag;

	return Status;

}


/******************************************************************************/
/**
*
* This function performs the GPIO set up for Interrupts
*
* @param	IntcInstancePtr is a reference to the Interrupt Controller
*		driver Instance
* @param	InstancePtr is a reference to the GPIO driver Instance
* @param	DeviceId is the XPAR_<GPIO_instance>_DEVICE_ID value from
*		xparameters.h
* @param	IntrId is XPAR_<INTC_instance>_<GPIO_instance>_IP2INTC_IRPT_INTR
*		value from xparameters.h
* @param	IntrMask is the GPIO channel mask
*
* @return	XST_SUCCESS if the Test is successful, otherwise XST_FAILURE
*
* @note		None.
*
******************************************************************************/
int GpioSetupIntrSystem(XGpio* InstancePtr,
			u16 DeviceId, u16 IntrId, u16 IntrMask)

{
	int Result;

	GlobalIntrMask = IntrMask;

	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 * specify the device ID that was generated in xparameters.h
	
	Result = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Result != XST_SUCCESS) {
		return Result;
	}
*/

	/* Hook up simple interrupt service routine for TestApp 

	Result = XIntc_Connect(IntcInstancePtr, IntrId,
				  (XInterruptHandler)GpioDriverHandler,
				  InstancePtr);
		*/		  
	
 		register_int_handler(IntrId, (XInterruptHandler)GpioDriverHandler, InstancePtr);
		enable_interrupt (IntrId);	  

	/*
	 * Enable the GPIO channel interrupts so that push button can be detected
	 * and enable interrupts for the GPIO device
	 */
	XGpio_InterruptEnable(InstancePtr, IntrMask);
	XGpio_InterruptGlobalEnable(InstancePtr);

	/* Enable the interrupt vector at the interrupt controller */
	//XIntc_Enable(IntcInstancePtr, IntrId);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	//Xil_ExceptionInit();
	//Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
	//			 (Xil_ExceptionHandler)XIntc_InterruptHandler,
	//			 IntcInstancePtr);

	/* Enable non-critical exceptions */
	//Xil_ExceptionEnable();

	/*
	 * Start the interrupt controller such that interrupts are recognized
	 * and handled by the processor
	 */
//	Result = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Result != XST_SUCCESS) {
		return Result;
	}

	return XST_SUCCESS;
}


int GpioInputExample(XGpio *GpioInstance,u16 DeviceId, u32 *DataRead)
{
	 int Status;

	 /*
	  * Initialize the GPIO driver so that it's ready to use,
	  * specify the device ID that is generated in xparameters.h
	  */
	 Status = XGpio_Initialize(GpioInstance, DeviceId);
	 if (Status != XST_SUCCESS) {
		  return XST_FAILURE;
	 }

	 /*
	  * Set the direction for all signals to be inputs
	  */
	 XGpio_SetDataDirection(GpioInstance, GPIO_CHANNEL1, 0xFFFF);

	 /*
	  * Read the state of the data so that it can be  verified
	  */
	 *DataRead = XGpio_DiscreteRead(GpioInstance, GPIO_CHANNEL1);

	 return XST_SUCCESS;

}

