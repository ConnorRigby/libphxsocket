/*
 * lws-minimal-ws-client
 *
 * Written in 2010-2020 by Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * This demonstrates a ws client that connects by default to libwebsockets.org
 * dumb increment ws server.
 */

#include <libwebsockets.h>
#include <string.h>
#include <signal.h>

#include <phoenix_socket.h>

/*
 * This represents your object that "contains" the client connection and has
 * the client connection bound to it
 */

static struct my_conn {
    lws_sorted_usec_list_t	sul;	     /* schedule connection retry */
    struct lws		*wsi;	     /* related wsi if any */
    uint16_t		retry_count; /* count of consequetive retries */
    phx_socket_t    *socket;
} mco;

static struct lws_context *context;
static int interrupted, port = 4000, ssl_connection = 0;
static const char *server_address = "localhost";

/*
 * The retry and backoff policy we want to use for our client connections
 */

static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };

static const lws_retry_bo_t retry = {
    .retry_ms_table			= backoff_ms,
    .retry_ms_table_count		= LWS_ARRAY_SIZE(backoff_ms),
    .conceal_count			= LWS_ARRAY_SIZE(backoff_ms),

    .secs_since_valid_ping		= 3,  /* force PINGs after secs idle */
    .secs_since_valid_hangup	= 10, /* hangup after secs idle */

    .jitter_percent			= 20,
};

phx_ret_t sock_write(void* userdata, unsigned char* buffer, size_t size)
{
    struct my_conn *mco = userdata;
    lws_write(mco->wsi, buffer, size, LWS_WRITE_TEXT);
    return PHX_OK;
}

/*
 * Scheduled sul callback that starts the connection attempt
 */

static void
connect_client(lws_sorted_usec_list_t *sul)
{
    struct my_conn *mco = lws_container_of(sul, struct my_conn, sul);
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof(i));

    i.context = context;
    i.port = port;
    i.address = server_address;
    i.path = "/device_socket/websocket?token=b8pjiYTvkcAHvK0mkdBWYcr1pwHvmzUQlz7oXv6J5nE&vsn=2.0.0";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = ssl_connection;
    i.local_protocol_name = "lws-minimal-client";
    i.pwsi = &mco->wsi;
    i.retry_and_idle_policy = &retry;
    i.userdata = mco;

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

    if (!lws_client_connect_via_info(&i))
        /*
         * Failed... schedule a retry... we can't use the _retry_wsi()
         * convenience wrapper api here because no valid wsi at this
         * point.
         */
        if (lws_retry_sul_schedule(context, 0, sul, &retry,
                                   connect_client, &mco->retry_count)) {
            lwsl_err("%s: connection attempts exhausted\n", __func__);
            interrupted = 1;
        }
}

static int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                 void *user, void *in, size_t len)
{
    struct my_conn *mco = (struct my_conn *)user;

    switch (reason) {

    case LWS_CALLBACK_WSI_CREATE:
        lwsl_user("WSI_CREATE\n");

        break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
                 in ? (char *)in : "(null)");
        goto do_retry;
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        lwsl_hexdump_notice(in, len);
        phx_handle_msg(mco->socket, (const char*)in, len);
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lwsl_user("%s: established\n", __func__);
        phx_transport_connected(mco->socket);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        goto do_retry;

    default:
        break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);

do_retry:
    /*
     * retry the connection to keep it nailed up
     *
     * For this example, we try to conceal any problem for one set of
     * backoff retries and then exit the app.
     *
     * If you set retry.conceal_count to be larger than the number of
     * elements in the backoff table, it will never give up and keep
     * retrying at the last backoff delay plus the random jitter amount.
     */
    if (lws_retry_sul_schedule_retry_wsi(wsi, &mco->sul, connect_client,
                                         &mco->retry_count)) {
        lwsl_err("%s: connection attempts exhausted\n", __func__);
        interrupted = 1;
    }

    return 0;
}

static const struct lws_protocols protocols[] = {
    { "lws-minimal-client", callback_minimal, 0, 0, },
    { NULL, NULL, 0, 0 }
};

static void
sigint_handler(int sig)
{
    interrupted = 1;
}

int main(int argc, const char **argv)
{
    struct lws_context_creation_info info;
    const char *p;
    int n = 0;

    signal(SIGINT, sigint_handler);
    memset(&info, 0, sizeof info);
    lws_cmdline_option_handle_builtin(argc, argv, &info);

    lwsl_user("LWS minimal ws client\n");

    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;

    if ((p = lws_cmdline_option(argc, argv, "-s")))
        server_address = p;

    if ((p = lws_cmdline_option(argc, argv, "-p")))
        port = atoi(p);

    if (lws_cmdline_option(argc, argv, "-n"))
        ssl_connection &= ~LCCSCF_USE_SSL;

    if (lws_cmdline_option(argc, argv, "-j"))
        ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;

    if (lws_cmdline_option(argc, argv, "-k"))
        ssl_connection |= LCCSCF_ALLOW_INSECURE;

    if (lws_cmdline_option(argc, argv, "-m"))
        ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    if (lws_cmdline_option(argc, argv, "-e"))
        ssl_connection |= LCCSCF_ALLOW_EXPIRED;

    info.fd_limit_per_thread = 1 + 1 + 1;

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    /* schedule the first client connection attempt to happen immediately */
    lws_sul_schedule(context, 0, &mco.sul, connect_client, 1);

    while (n >= 0 && !interrupted)
        n = lws_service(context, 0);

    lws_context_destroy(context);
    lwsl_user("Completed\n");

    return 0;
}
