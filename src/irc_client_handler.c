//
// Created by romain on 26/05/18.
//

#include <unistd.h>
#include "network.h"
#include "stderr.h"
#include "irc.h"
#include "util.h"

static void on_connect(network_client_t *client) {
    user_t *user = malloc(sizeof(user_t));
    user->client = client;
    user->name = strdup("undefined");

    list_add(&server.users, user);

    eprintf("[Client %d] connected\n", client->id);
}

static void on_disconnect(network_client_t *client) {
    user_t *found = find_user(client);

    if (found) {
        if (found->name)
            free(found->name);
        list_remove(&server.users, found);
        free(found);
    }
    eprintf("[Client %d] disconnected\n", client->id);
}

static void on_received(network_client_t *client, char const *packet, size_t len) {
    eprintf("[Server] Recv \t<--\t[Client %d]:\t\t", client->id);
    fflush(stderr);
    write(STDERR_FILENO, packet, len);
    eprintf("\n");

    user_t *found = find_user(client);

    if (found)
        parse_packet(client, packet, len);
}

static void on_sent(network_client_t *client, char const *packet, size_t len) {
    eprintf("[Server] Sent \t-->\t[Client %d]:\t\t", client->id);
    fflush(stderr);
    write(STDERR_FILENO, packet, len - 2);
    eprintf("\n");
}

void configure_client_handler(client_handler_t *handler) {
    handler->on_received = &on_received;
    handler->on_connect = &on_connect;
    handler->on_sent = &on_sent;
    handler->on_disconnect = &on_disconnect;
}