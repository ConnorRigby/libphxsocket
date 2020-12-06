#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "phoenix_socket.h"
#include "phoenix_message.h"
#include "phoenix_channel.h"
#include "phoenix_push.h"

struct phx_socket {
  jsmn_parser p;
  jsmntok_t* t;
  phx_channel_t* channels;
};

phx_ret_t phx_sock_alloc(phx_socket_t* socket)
{
  socket = malloc(sizeof(phx_socket_t));
  if(!socket)
    return PHX_ENOMEM;

  jsmn_init(&socket->p);

  // 5 tokens is the smallest a payload could possibly be
  // this isn't really even useful since an empty payload isnt' worth much
  socket->t = malloc(sizeof(jsmntok_t) * 5);
  if(!socket->t)
    return PHX_ENOMEM;
}

void phx_transport_connected(phx_socket_t* socket)
{

}

phx_ret_t phx_handle_msg(phx_socket_t* socket, const char* rawPayload, size_t len)
{
  phx_ret_t r;
  phx_msg_t* message;
  r = phx_msg_alloc(message);
  if(r < 0)
    return r;

  r = phx_msg_decode(socket->t, rawPayload, message);
  if(r < 0)
  {
    phx_msg_dealloc(message);
    return r;
  }
}

phx_ret_t phx_channel_join(phx_socket_t* socket, const char* topic, const char* params)
{
  phx_ret_t r;
  phx_channel_t* channel;
  channel = malloc(sizeof(phx_channel_t));
  if(!channel)
    return PHX_ENOMEM;

  // do i need to memcpy these?
  channel->topic = topic;
  channel->params = params;
  channel->next = NULL;

  r = phx_add_channel(socket, channel);
  if(r < 0)
  {
    free(channel);
    return r;
  }

  phx_push_t* push;
  push = malloc(sizeof(phx_push_t));
  if(!push)
  {
    free(channel);
    return PHX_ENOMEM;
  }

  r = phx_add_push(channel, push);
  if(r < 0)
  {
    free(channel);
    free(push);
    return r;
  }

  return PHX_OK;
}

/*
 * Private
*/

phx_ret_t phx_add_channel(phx_socket_t* socket, phx_channel_t* channel)
{
  if(socket->channels == NULL)
  {
    socket->channels = channel;
    return PHX_OK;
  }
  phx_channel_t* current = socket->channels;
  while(current)
    current = current->next;

  current->next = channel;
  return PHX_OK;
}

phx_ret_t phx_add_push(phx_channel_t* channel, phx_push_t* push)
{
  if(channel->pushes == NULL)
  {
    channel->pushes = push;
    return PHX_OK;
  }
  phx_push_t* current = channel->pushes;
  while(current)
    current = current->next;
  current->next = push;
  return PHX_OK;
}
