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

phx_ret_t phx_msg_encode(phx_msg_t *message, unsigned char **buffer_ptr, size_t *len)
{
    size_t r = 0;
    phx_ret_t ret = PHX_OK;
    char *tmp;
    unsigned char *buffer;
    cJSON *message_j = NULL;
    cJSON *join_ref_j = NULL;
    cJSON *ref_j = NULL;
    cJSON *topic_j = NULL;
    cJSON *event_j = NULL;
    cJSON *payload_j = NULL;

    message_j = cJSON_CreateArray();
    if (!message_j)
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    tmp = malloc(TMP_BUFFER_SIZE);
    if (!tmp)
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    memset(tmp, 0, TMP_BUFFER_SIZE);
    r = sprintf(tmp, "%d", message->join_ref);
    if (r < 0)
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    join_ref_j = cJSON_CreateString(tmp);
    if (join_ref_j)
    {
        cJSON_AddItemToArray(message_j, join_ref_j);
    }
    else
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    memset(tmp, 0, TMP_BUFFER_SIZE);
    r = sprintf(tmp, "%d", message->ref);
    if (r < 0) {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    ref_j = cJSON_CreateString(tmp);
    if (ref_j)
    {
        cJSON_AddItemToArray(message_j, ref_j);
    }
    else
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    topic_j = cJSON_CreateString(message->topic);
    if (topic_j)
        cJSON_AddItemToArray(message_j, topic_j);
    else
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    event_j = cJSON_CreateString(message->event);
    if (event_j)
        cJSON_AddItemToArray(message_j, event_j);
    else
    {
        ret = PHX_ENOMEM;
        goto cleanup;
    }

    payload_j = cJSON_CreateObject();
    if (payload_j)
        cJSON_AddItemToArray(message_j, payload_j);

    tmp = cJSON_Print(message_j);
    if (!tmp)
        return PHX_ENOMEM;
    *len = strlen(tmp);
    *buffer_ptr = malloc(*len);
    buffer = *buffer_ptr;
    if (!buffer)
        goto cleanup;

    memset(buffer, 0, *len);
    memcpy(buffer, tmp, *len);

cleanup:
    free(tmp);
    cJSON_Delete(message_j);
    return ret;
}

// #define PHX_MSG_V2_INDEX_ARR 0
// #define PHX_MSG_V2_INDEX_JOIN_REF 1
// #define PHX_MSG_V2_INDEX_REF 2
// #define PHX_MSG_V2_INDEX_TOPIC 3
// #define PHX_MSG_V2_INDEX_EVENT 4
// #define PHX_MSG_V2_INDEX_PAYLOAD 5

phx_ret_t phx_msg_decode(const char *rawPayload, phx_msg_t *message)
{
    // cJSON_Parse();
    return PHX_OK;
}
