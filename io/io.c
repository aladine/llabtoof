/* io.c -- io handler using UART
 * 
 * Copyright 2010 Adrien Beraud <adrienberaud@gmail.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * 		http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include "io.h"

#include "xmk.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"
#include <pthread.h>

struct IOmanager_s
{
	bool init;
	XUartLite uartlite[2];	// UARTLite instances (just one is used if we're a player)
	IO_cb callback;	// Callback function called when data is received
	void* callback_arg;
	
	char input_buffer[PACKET_SIZE];
	//char input_buffer1[PACKET_SIZE];
	
	pthread_t input_t;		// Pooling thread (checking for new stuff on input);
};

//
// Private functions prototypes
//

void IO_initUART(XUartLite* uartlite, u16 device_id);
void IO_inputThread(IOmanager * io);

//
// Functions
//

void IO_init(IOmanager * io, IO_cb callback, void* callback_arg)
{
	int status;
	
	io->init = 0;
	io->callback = callback;
	io->callback_arg = callback_arg;
	
	/*io->input_count[0] = 0;
	io->input_count[1] = 0;*/
	
	status = IO_initUART(&(io->uartlite[0]), XPAR_UARTLITE_1_DEVICE_ID);
	if(status != XST_SUCCESS) return;
	
	/*if(type == SERVER)
	{
		status = IO_initUART(&(io->uartlite[1]), XPAR_UARTLITE_2_DEVICE_ID);
		if(status != XST_SUCCESS) io->uartlite[1] = NULL;
	}*/
	
	// Launch input thread (that will pool for new data)
	status = pthread_create(&io->input_t, NULL, (void*)IO_inputThread, io);
	
	io->init = 1;
}

void IO_send(IOmanager * io, void * data)
{
	if(!io || !(io->init) || !data)
		return;
	XUartLite_Send(io->uartlite[0], data, 4);
	/*if(io->type == SERVER)
		XUartLite_Send(io->uartlite[1], data, 4);*/
}

void IO_initUART(XUartLite* uartlite, u16 device_id)
{
	if(!uartlite) return;
	int status;
	XUartLite_Initialize(uartlite, device_id);
	
	status = XUartLite_SelfTest(uartlite);
	if(status  != XST_SUCCESS)
		return XST_FAILURE;
	
	XUartLite_ResetFifos(uartlite);
	
	return XST_SUCCESS;
}


/**
 * Rationale :
 * The Microblaze FIFO buffer is 16 chars-long
 * We use a baud-rate of 115200 bps = 14400 Bps
 * This means that the buffer takes a maximum of 1.11ms to be filled
 * So we can sleep for 1ms between each check.
 * 
 * 
 */
void IO_inputThread(IOmanager * io)
{
	if(!io) return;
	
	char i;
	
	char received0 = 0,
		 received1 = 0;
	
	char total0 = 0, 
		 total1 = 0;
	
	for(i=0; i<4; i++)
	{
		io->input_buffer0[i] = 0;
	//	io->input_buffer1[i] = 0;
	}
	
	//main pooling thread
	while(1)
	{
		received0 = XUartLite_Recv(io->uartlite[0], io->input_buffer0+total0, PACKET_SIZE-total0);
		total0 += received0;
		
		if(total0 == 4)
		{
			io->callback(io->callback_arg, io->input_buffer);
			for(i=0; i<4; i++) io->input_buffer[i] = 0;
		}
		
		/*if(io->type == SERVER)
		{
			received1 = XUartLite_Recv(io->uartlite[1], io->input_buffer1+total1, PACKET_SIZE-total1);
			total1 += received1;
			
			if(total1 == 4) 
			{
				io->callback(io->input_buffer1);
				for(i=0; i<4; i++) io->input_buffer1[i] = 0;
			}
		}*/
		
		if(!received0 && !received1) usleep(1000); //sleep for 1ms if there is no activity
	}
}
