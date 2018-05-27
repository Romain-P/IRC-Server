//
// Created by romain on 26/05/18.
//

#include <unistd.h>
#include "network.h"
#include "stderr.h"

static void on_connect(network_client_t *client) {

    eprintf("[Client %d] connected\n", client->id);
}

static void on_disconnect(network_client_t *client) {

    eprintf("[Client %d] disconnected\n", client->id);
}

static void on_received(network_client_t *client, char const *packet, size_t len) {


    eprintf("[Server] Recv \t<--\t[Client %d]:\t\t", client->id);
    fflush(stdout);
    write(STDERR_FILENO, packet, len);
    eprintf("\n");
}

static void on_sent(network_client_t *client, char const *packet, size_t len) {
    eprintf("[Server] Sent \t-->\t[Client %d]:\t\t", client->id);
    fflush(stdout);
    write(STDERR_FILENO, packet, len);
    eprintf("\n");
}

void configure_client_handler(client_handler_t *handler) {
    handler->on_received = &on_received;
    handler->on_connect = &on_connect;
    handler->on_sent = &on_sent;
    handler->on_disconnect = &on_disconnect;
}