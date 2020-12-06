#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cJSON.h"

#include "phoenix_message.h"

#define TMP_BUFFER_SIZE 255

phx_ret_t phx_msg_alloc(phx_msg_t *message)
{
    message = malloc(sizeof(message));
    if (message == NULL)
        return PHX_ENOMEM;

    return PHX_OK;
}

phx_ret_t phx_msg_dealloc(phx_msg_t *message)
{
    free(message);
    return PHX_OK;
}

phx_ret_t phx_msg_encode(phx_msg_t *message, unsigned char** buffer_ptr, size_t *len)
{
    size_t r = 0;
    cJSON *message_j = NULL;
    cJSON *join_ref_j = NULL;
    cJSON *ref_j = NULL;
    cJSON *topic_j = NULL;
    cJSON *event_j = NULL;
    cJSON *payload_j = NULL;

    message_j = cJSON_CreateArray();
    if (!message_j)
        return PHX_ENOMEM;

    char *tmp = malloc(TMP_BUFFER_SIZE);
    if (!tmp)
        return PHX_ENOMEM;

    memset(tmp, 0, TMP_BUFFER_SIZE);
    r = sprintf(tmp, "%d", message->join_ref);
    if (r < 0)
    {
        free(tmp);
        return PHX_ENOMEM;
    }
    join_ref_j = cJSON_CreateString(tmp);
    if (join_ref_j)
    {
        cJSON_AddItemToArray(message_j, join_ref_j);
    }
    else
    {
        free(tmp);
        return PHX_ENOMEM;
    }

    memset(tmp, 0, TMP_BUFFER_SIZE);
    r = sprintf(tmp, "%d", message->ref);
    if (r < 0)
    {
        free(tmp);
        return PHX_ENOMEM;
    }
    ref_j = cJSON_CreateString(tmp);
    if (ref_j)
    {
        cJSON_AddItemToArray(message_j, ref_j);
    }
    else
    {
        free(tmp);
        return PHX_ENOMEM;
    }
    free(tmp);
    tmp = NULL;

    topic_j = cJSON_CreateString(message->topic);
    if (topic_j)
        cJSON_AddItemToArray(message_j, topic_j);
    else
        return PHX_ENOMEM;

    event_j = cJSON_CreateString(message->event);
    if(event_j)
        cJSON_AddItemToArray(message_j, event_j);
    else
        return PHX_ENOMEM;

    payload_j = cJSON_CreateObject();
    if(payload_j)
        cJSON_AddItemToArray(message_j, payload_j);

    tmp = cJSON_Print(message_j);
    if(!tmp)
        return PHX_ENOMEM;
    *len = strlen(tmp);
    *buffer_ptr = malloc(*len);
    unsigned char* buffer = *buffer_ptr;
    if(!buffer)
    {
        free(tmp);
        return PHX_ENOMEM;
    }
    memset(buffer, 0, *len);
    memcpy(buffer, tmp, *len);
    free(tmp);
    return PHX_OK;
}

// #define PHX_MSG_V2_INDEX_ARR 0
// #define PHX_MSG_V2_INDEX_JOIN_REF 1
// #define PHX_MSG_V2_INDEX_REF 2
// #define PHX_MSG_V2_INDEX_TOPIC 3
// #define PHX_MSG_V2_INDEX_EVENT 4
// #define PHX_MSG_V2_INDEX_PAYLOAD 5

phx_ret_t phx_msg_decode(const char *rawPayload, phx_msg_t *message)
{
    // // Sanity checks
    // if (t[PHX_MSG_V2_INDEX_ARR].type != JSMN_ARRAY)
    //     return PHX_EINVAL_TOKEN;/

    // if (t[PHX_MSG_V2_INDEX_JOIN_REF].type != JSMN_PRIMITIVE)
    //     return PHX_EINVAL_TOKEN;

    // if (t[PHX_MSG_V2_INDEX_REF].type != JSMN_PRIMITIVE)
    //     return PHX_EINVAL_TOKEN;

    // if (t[PHX_MSG_V2_INDEX_TOPIC].type != JSMN_STRING)
    //     return PHX_EINVAL_TOKEN;

    // if (t[PHX_MSG_V2_INDEX_EVENT].type != JSMN_STRING)
    //     return PHX_EINVAL_TOKEN;

    // if (t[PHX_MSG_V2_INDEX_PAYLOAD].type != JSMN_OBJECT)
    //     return PHX_EINVAL_TOKEN;

    // long ref;
    // size_t len;

    // // join_ref
    // ref = strtol(rawPayload + t[PHX_MSG_V2_INDEX_JOIN_REF].start, NULL, 10);
    // message->join_ref = ref;

    // // ref
    // ref = strtol(rawPayload + t[PHX_MSG_V2_INDEX_REF].start, NULL, 10);
    // message->ref = ref;

    // // topic
    // len = t[PHX_MSG_V2_INDEX_TOPIC].end - t[PHX_MSG_V2_INDEX_TOPIC].start;
    // message->topic = malloc(len);
    // if (!message->topic)
    //     return PHX_ENOMEM;
    // memcpy(message->topic, rawPayload + t[PHX_MSG_V2_INDEX_TOPIC].start, len);

    // // event
    // len = t[PHX_MSG_V2_INDEX_EVENT].end - t[PHX_MSG_V2_INDEX_EVENT].start;
    // message->event = malloc(len);
    // if (!message->event)
    //     return PHX_ENOMEM;
    // memcpy(message->event, rawPayload + t[PHX_MSG_V2_INDEX_EVENT].start, len);

    // payload.
    // message->payload=&t[PHX_MSG_V2_INDEX_PAYLOAD];

    // the first 4 objects can be freed now but i haven't decided how to do it yet.
    return PHX_OK;
}
