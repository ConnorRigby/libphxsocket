#ifndef PHOENIX_SOCKET_H
#define PHOENIX_SOCKET_H

#define JSMN_HEADER
#include "jsmn.h"

#include <stddef.h>
#include "phoenix_common.h"
#include "phoenix_channel.h"
#include "phoenix_push.h"

phx_ret_t phx_sock_alloc(phx_socket_t*);
void phx_transport_connected(phx_socket_t*);
phx_ret_t phx_handle_msg(phx_socket_t* socket, const char* rawPayload, size_t len);
phx_ret_t phx_channel_join(phx_socket_t*, const char* topic, const char* params);
phx_ret_t phx_channel_leave(phx_socket_t*);
phx_ret_t phx_add_channel(phx_socket_t*, phx_channel_t*);
phx_ret_t phx_add_push(phx_channel_t*, phx_push_t*);

#endif
