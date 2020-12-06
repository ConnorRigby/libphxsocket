#ifndef PHOENIX_MESSAGE_H
#define PHOENIX_MESSAGE_H

#include "phoenix_socket.h"

struct phx_msg {
    const char* topic;
    const char* event;
    size_t ref;
    size_t join_ref;
    const char* payload;
};

phx_ret_t phx_msg_alloc(phx_msg_t*);
phx_ret_t phx_msg_dealloc(phx_msg_t*);
phx_ret_t phx_msg_decode(const char*, phx_msg_t*);
phx_ret_t phx_msg_encode(phx_msg_t*, unsigned char**, size_t*);

#endif
