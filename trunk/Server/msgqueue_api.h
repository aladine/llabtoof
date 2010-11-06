#ifndef MSGQUEUE_API_H
#define MSGQUEUE_API_H
#include "controlling.h"

void msgqueue_send(GameState* );
GameState * msgqueue_recv(GameState* );
void msgqueue_init();
#endif
