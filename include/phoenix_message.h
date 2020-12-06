#ifndef PHOENIX_MESSAGE_H
#define PHOENIX_MESSAGE_H

#include "phoenix_socket.h"

#define JSMN_HEADER
#include "jsmn.h"

struct phx_msg {
  char* topic;
  char* event;
  size_t ref;
  size_t join_ref;
  jsmntok_t* payload;
};

phx_ret_t phx_msg_alloc(phx_msg_t*);
phx_ret_t phx_msg_dealloc(phx_msg_t*);
phx_ret_t phx_msg_decode(jsmntok_t*, const char*, phx_msg_t*);

#endif
