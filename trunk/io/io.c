/* io.c -- io handler using UART
 *
 * Copyright 2010 Adrien Beraud <adrienberaud@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "xparameters.h"
#include "xstatus.h"


#include "io.h"
#include <sys/intr.h>

//
// Private functions prototypes
//

void IO_inputThread(IOmanager * io);
void IO_receivedHandler(IOmanager * io, unsigned received);

//
// Functions
//

void IO_init(IOmanager * io, u16 device_id, u16 interrupt_id, IO_cb callback, void* callback_arg)
{
	int status;

	io->init = FALSE;
	io->callback = callback;
	io->callback_arg = callback_arg;

	status = XUartLite_Initialize(&(io->uartlite), device_id);
	if(status != XST_SUCCESS) 
	{
		xil_printf(" Error : Xuartlite_initialize failed. Code : %d\n",status);
		return;
	}
	
	status = XUartLite_SelfTest(&(io->uartlite));
	if(status != XST_SUCCESS)
	{
		xil_printf(" Error : Xuartlite_selftest failed. Code : %d\n",status);
		return;
	}
	
	XUartLite_ResetFifos(&(io->uartlite));
	
	status = register_int_handler(XPAR_XPS_INTC_0_RS232_DCE_INTERRUPT_INTR, (void*)XUartLite_InterruptHandler, &(io->uartlite));
	if (status != XST_SUCCESS)
	{
		xil_printf(" Error : register_int_handler failed. Code : %d\n",status);
		return;
	}
		  
	enable_interrupt(interrupt_id);
			
	XUartLite_EnableInterrupt(&(io->uartlite));
	
	// Launch input thread (that will pool for new data)
	status = pthread_create(&io->input_t, NULL, (void*)IO_inputThread, io);
	
	io->init = TRUE;
    //  xil_printf("\n  IO_init in io.c finished.   \n");
}

void IO_send(IOmanager * io, void * data)
{
	if(!io || !(io->init) || !data) return;

	XUartLite_Send(&(io->uartlite), data, 4);
}


/**
 * Rationale :
 * The Microblaze FIFO buffer is 16 chars-long
 * We use a baud-rate of 115200 bps = 14400 Bps
 * This means that the buffer takes a maximum of 1.11ms to be filled
 * So we can sleep for 1ms between each check.
 */
void IO_inputThread(IOmanager * io)
{
	if(!io) return;

	char i;
		  
	char * ib_start = &(io->input_buffer_start);
	char * ib_size = &(io->input_buffer_received);

	for(i=0; i<BUFFER_SIZE; i++) io->input_buffer[i] = 0;   //empty input buffer
	
	*ib_start = 0;
	*ib_size = 0;
	
	XUartLite_SetRecvHandler(&(io->uartlite), (XUartLite_Handler)IO_receivedHandler, io);
	IO_receivedHandler(io, 0);
	
	//main pooling thread
    //  xil_printf(" IO_inputThread started.");
	while(1)
	{
		while(*ib_size >= PACKET_SIZE)
		{
			io->callback(io->callback_arg, io->input_buffer+(*ib_start%BUFFER_SIZE));
			ib_start += PACKET_SIZE;
			ib_size -= PACKET_SIZE;	 //possibility of multi-"threading" problems here
		}
	
	}
}

void IO_receivedHandler(IOmanager * io, unsigned received)
{		  
	io->input_buffer_received += received;
		  
	XUartLite_Recv(&(io->uartlite), 
		       io->input_buffer+((io->input_buffer_start+io->input_buffer_received)%BUFFER_SIZE), 
		       PACKET_SIZE);
}
