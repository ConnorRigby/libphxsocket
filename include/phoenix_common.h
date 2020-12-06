#ifndef PHOENIX_COMMON_H
#define PHOENIX_COMMON_H

typedef struct phx_socket phx_socket_t;
typedef struct phx_channel phx_channel_t;
typedef struct phx_push phx_push_t;
typedef struct phx_msg phx_msg_t;

typedef enum phx_ret
{
    PHX_ENULL = -3,
    PHX_ENOMEM = -2,
    PHX_EINVAL_TOKEN = -1,
    PHX_OK = 0,
} phx_ret_t;

#endif
