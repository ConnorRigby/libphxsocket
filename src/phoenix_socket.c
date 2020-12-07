#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "phoenix_socket.h"
#include "phoenix_message.h"
#include "phoenix_channel.h"
#include "phoenix_push.h"

struct phx_socket
{
    phx_channel_t *channels;
    size_t channel_cnt;
    size_t ref;
    void* userdata;

    // Callbacks
    phx_ret_t (*send_callback)(void* userdata, unsigned char* buffer, size_t len);
};

phx_ret_t phx_sock_alloc(phx_socket_t **socket_ptr, void* userdata, phx_ret_t (*send_callback)(void* userdata, unsigned char* buffer, size_t len))
{
    *socket_ptr = malloc(sizeof(phx_socket_t));
    if (!socket_ptr)
        return PHX_ENOMEM;
    phx_socket_t* socket = *socket_ptr;

    socket->channels = NULL;
    socket->channel_cnt = 0;
    socket->ref = 0;
    socket->userdata = userdata;
    socket->send_callback = send_callback;
    return PHX_OK;
}

void phx_transport_connected(phx_socket_t *socket)
{
    phx_channel_t* current_channel = socket->channels;
    phx_push_t* current_push = NULL;
    while(current_channel)
    {
        current_push = current_channel->pushes;
        while(current_push)
        {
            phx_ret_t ret;
            size_t len = 0;
            unsigned char* buffer;
            ret = phx_msg_encode(current_push->message, &buffer, &len);
            if(ret == PHX_OK)
                socket->send_callback(socket->userdata, buffer, len);

            current_push = current_push->next;
        }
        current_channel = socket->channels->next;
    }
}

phx_ret_t phx_handle_msg(phx_socket_t *socket, const char *rawPayload, size_t len)
{
}

phx_ret_t phx_channel_join(phx_socket_t *socket, const char *topic, const char *params)
{
    phx_ret_t r;
    phx_channel_t* channel;
    r = phx_add_channel(socket, &channel, topic, params);
    if (r < 0)
        return r;

    phx_msg_t* msg = malloc(sizeof(phx_msg_t));
    msg->ref = socket->ref++;
    msg->topic = topic;
    // msg->payload = params;
    msg->event = "phx_join";

    phx_push_t* push;

    r = phx_add_push(channel, &push, msg);

    return PHX_OK;
}

/*
 * Private
*/

phx_ret_t phx_add_channel(phx_socket_t *socket, phx_channel_t** channel_ptr, const char* topic, const char* params)
{
    if(socket == NULL)
        return PHX_ENULL;

    *channel_ptr = malloc(sizeof(phx_channel_t));
    phx_channel_t* channel = *channel_ptr;
    if(!channel)
        return PHX_ENOMEM;
    channel->pushes =  NULL;
    channel->push_cnt = 0;
    channel->topic = topic;
    channel->params = params;
    channel->next = socket->channels;
    socket->channels = channel;
    socket->channel_cnt++;
    return PHX_OK;
}

phx_ret_t phx_add_push(phx_channel_t *channel, phx_push_t **push_ptr, phx_msg_t* msg)
{
    if(channel == NULL)
        return PHX_ENULL;

    *push_ptr = malloc(sizeof(phx_push_t));
    phx_push_t* push = *push_ptr;
    if(!push)
        return PHX_ENOMEM;

    push->message = msg;
    push->next = channel->pushes;
    channel->pushes = push;
    channel->push_cnt++;
    return PHX_OK;
}
