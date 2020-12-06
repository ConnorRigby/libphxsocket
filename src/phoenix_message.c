#include <stdlib.h>
#include <string.h>

#include "phoenix_message.h"

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

#define PHX_MSG_V2_INDEX_ARR 0
#define PHX_MSG_V2_INDEX_JOIN_REF 1
#define PHX_MSG_V2_INDEX_REF 2
#define PHX_MSG_V2_INDEX_TOPIC 3
#define PHX_MSG_V2_INDEX_EVENT 4
#define PHX_MSG_V2_INDEX_PAYLOAD 5

phx_ret_t phx_msg_decode(jsmntok_t *t, const char *rawPayload, phx_msg_t *message)
{
  // Sanity checks
  if (t[PHX_MSG_V2_INDEX_ARR].type != JSMN_ARRAY)
    return PHX_EINVAL_TOKEN;

  if (t[PHX_MSG_V2_INDEX_JOIN_REF].type != JSMN_PRIMITIVE)
    return PHX_EINVAL_TOKEN;

  if (t[PHX_MSG_V2_INDEX_REF].type != JSMN_PRIMITIVE)
    return PHX_EINVAL_TOKEN;

  if (t[PHX_MSG_V2_INDEX_TOPIC].type != JSMN_STRING)
    return PHX_EINVAL_TOKEN;

  if (t[PHX_MSG_V2_INDEX_EVENT].type != JSMN_STRING)
    return PHX_EINVAL_TOKEN;

  if (t[PHX_MSG_V2_INDEX_PAYLOAD].type != JSMN_OBJECT)
    return PHX_EINVAL_TOKEN;

  long ref;
  size_t len;

  // join_ref
  ref = strtol(rawPayload + t[PHX_MSG_V2_INDEX_JOIN_REF].start, NULL, 10);
  message->join_ref = ref;

  // ref
  ref = strtol(rawPayload + t[PHX_MSG_V2_INDEX_REF].start, NULL, 10);
  message->ref = ref;

  // topic
  len = t[PHX_MSG_V2_INDEX_TOPIC].end - t[PHX_MSG_V2_INDEX_TOPIC].start;
  message->topic = malloc(len);
  if (!message->topic)
    return PHX_ENOMEM;
  memcpy(message->topic, rawPayload + t[PHX_MSG_V2_INDEX_TOPIC].start, len);

  // event
  len = t[PHX_MSG_V2_INDEX_EVENT].end - t[PHX_MSG_V2_INDEX_EVENT].start;
  message->event = malloc(len);
  if (!message->event)
    return PHX_ENOMEM;
  memcpy(message->event, rawPayload + t[PHX_MSG_V2_INDEX_EVENT].start, len);

  // payload.
  message->payload=&t[PHX_MSG_V2_INDEX_PAYLOAD];

  // the first 4 objects can be freed now but i haven't decided how to do it yet.
  return PHX_OK;
}
