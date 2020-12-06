#ifndef PHX_CHANNEL_H
#define PHX_CHANNEL_H

#include <stddef.h>
#include "phoenix_push.h"

struct phx_channel {
    const char* topic;
    const char* params;
    struct phx_channel* next;
    phx_push_t* pushes;
    size_t push_cnt;
};

#endif
