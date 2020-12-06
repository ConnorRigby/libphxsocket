#ifndef PHOENIX_PUSH_H
#define PHOENIX_PUSH_H

#include "phoenix_message.h"

struct phx_push {
    struct phx_push* next;
    phx_msg_t* message;
};

#endif
