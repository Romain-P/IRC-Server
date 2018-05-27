//
// Created by romain on 26/05/18.
//

#include <unistd.h>
#include "network.h"
#include "stderr.h"
#include "irc.h"
#include "util.h"

irc_instance_t server = { list_init };

static command_t const commands[] = {
        {"NICK", &on_nickname_message },
        {"QUIT", &on_quit},
        { NULL, NULL }
};

static void on_connect(network_client_t *client) {
    user_t *user = malloc(sizeof(user_t));
    user->client = client;
    user->name = NULL;

    list_add(&server.users, user);

    eprintf("[Client %d] connected\n", client->id);
}

static void on_disconnect(network_client_t *client) {
    user_t *found = NULL;

    for (iter_t *it = iter_begin(&server.users); it; iter_next(it)) {
        user_t *user = it->data;
        if (user->client == client) {
            found = user;
            break;
        }
    }

    if (found) {
        if (found->name)
            free(found->name);
        list_remove(&server.users, found);
    }
    eprintf("[Client %d] disconnected\n", client->id);
}

static void on_received(network_client_t *client, char const *packet, size_t len) {
    user_t *found = NULL;

    for (iter_t *it = iter_begin(&server.users); it; iter_next(it)) {
        user_t *user = it->data;
        if (user->client == client) {
            found = user;
            break;
        }
    }

    if (found) {
        char **split = charset_split(packet, len, " ");
        for (int i = 0; commands[i].named; ++i) {
            if (!strcmp(commands[i].named, split[0])) {
                commands[i].handler(found, split + 1);
                break;
            }
        }
        str_free_array(split);
    }

    eprintf("[Server] Recv \t<--\t[Client %d]:\t\t", client->id);
    fflush(stderr);
    write(STDERR_FILENO, packet, len);
    eprintf("\n");
}

static void on_sent(network_client_t *client, char const *packet, size_t len) {
    eprintf("[Server] Sent \t-->\t[Client %d]:\t\t", client->id);
    fflush(stderr);
    write(STDERR_FILENO, packet, len);
    eprintf("\n");
}

void configure_client_handler(client_handler_t *handler) {
    handler->on_received = &on_received;
    handler->on_connect = &on_connect;
    handler->on_sent = &on_sent;
    handler->on_disconnect = &on_disconnect;
}