#ifndef MSGQUEUE_API_H
#define MSGQUEUE_API_H
#include "controlling.h"

void msgqueue_send(uartdata* );
int msgqueue_recv(uartdata* );
void msgqueue_init();
#endif
