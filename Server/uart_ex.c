#include "xuartlite_3.h"

XUartLite_mIsReceiveEmpty(BASEADDR) is false when there is data
waiting to be read
XUartLite_RecvByte(BASEADDR) return one character (char);

/* Enable interrupt requests in the interrupt controller */
XIntc_mEnableIntr(INTC_BASEADDR,
TIMER_INTERRUPT_MASK | UART_INTERRUPT_MASK);
/* Enable UART interrupts */
XUartLite_mEnableIntr(BASEADDR);

//receive
 while(1)
    {
       //Get a command from the user via UART
       inp = XUartLite_RecvByte(0x84000000);
}



LINK:

http://forums.xilinx.com/t5/EDK-and-Platform-Studio/Interrupt-with-a-timer/m-p/14838
http://newsgroups.derkeiler.com/Archive/Comp/comp.arch.fpga/2008-02/msg00854.html


//using DIP








