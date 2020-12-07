# Phoenix Channels client

[Phoenix Channels](https://hexdocs.pm/phoenix/channels.html) implementation
in C. Intended to be used on ESP32 but there's nothing ESP32 specific about it.
It was developed on Linux and will likely work elsewhere. Currently uses
[libwebsockets](https://libwebsockets.org/) as a transport and
[cJSON](https://github.com/DaveGamble/cJSON) for JSON encoding/decoding

## Installation

idk copy the source files into your project i guess

## Usage

This is just an example, that gives the basic usage of the barebones API at the moment.
It doesn't really even do anything. Did i mention the library is WIP?

```c
#include <libwebsockets.h>
#include "phoenix_socket.h"

static struct my_conn {
    lws_sorted_usec_list_t	sul;	     /* schedule connection retry */
    struct lws		*wsi;	     /* related wsi if any */
    uint16_t		retry_count; /* count of consequetive retries */
    phx_socket_t    *socket;
} mco;

phx_ret_t sock_write(void* userdata, unsigned char* buffer, size_t size)
{
    struct my_conn *mco = userdata;
    lws_write(mco->wsi, buffer, size, LWS_WRITE_TEXT);
    return PHX_OK;
}

void sock_connect()
{
    phx_ret_t r;
    r = phx_sock_alloc(&mco->socket, mco, sock_write);
    if(r < 0)
    {
        printf("Failed to alloc socket");
        return;
    }

    r = phx_channel_join(mco->socket, "test", NULL);
    if(r < 0)
    {
        printf("failed to add join");
        return;
    }
}
```

## Justification

* Q: Why cJSON when jsmn exists?
  A: have you ever tried using jsmn?
* Q: Why make this?
  A: memes mostly

## License

The source code is released under Apache License 2.0.

Check [LICENSE](LICENSE) for more information.
