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

#include "xparameters.h"
#include "xstatus.h"

#include "io.h"

//
// Private functions prototypes
//

void IO_inputThread(IOmanager * io);

//
// Functions
//

void IO_init(IOmanager * io, u16 device_id, IO_cb callback, void* callback_arg)
{
	int status;

	io->init = FALSE;
	io->callback = callback;
	io->callback_arg = callback_arg;

	status = XUartLite_Initialize(&(io->uartlite), device_id);
	if(status != XST_SUCCESS) return;

	status = XUartLite_SelfTest(&(io->uartlite));
	if(status != XST_SUCCESS) return;

	XUartLite_ResetFifos(&(io->uartlite));
	
	

	// Launch input thread (that will pool for new data)
	status = pthread_create(&io->input_t, NULL, (void*)IO_inputThread, io);

	io->init = TRUE;
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
 *
 */
void IO_inputThread(IOmanager * io)
{
	if(!io) return;

	char i;

	char received = 0,
		 total = 0;

	for(i=0; i<4; i++) io->input_buffer[i] = 0;	//empty input buffer
	
	//IO_send(io, "TH\r\n");

	//main pooling thread
	while(1)
	{
		received = XUartLite_Recv(&(io->uartlite), io->input_buffer+total, PACKET_SIZE-total);
		total += received;

		if(total == 4)
		{
			//IO_send(io, "R!\r\n");
			io->callback(io->callback_arg, io->input_buffer);	//empty input buffer
			for(i=0; i<4; i++) io->input_buffer[i] = 0;
			total = 0;
		}

		if(!received) 
		{
			//usleep(1000); //sleep for 1ms if there is no activity
			unsigned j;
			for(j=0; j<200; j++) {asm("nop");asm("nop");}
		}
	}
}

