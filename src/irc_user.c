//
// Created by romain on 28/05/18.
//

#include <string.h>
#include <stdlib.h>
#include "irc.h"

user_t *find_user(network_client_t *client) {
    for (iter_t *it = iter_begin(&server.users); it; iter_next(it)) {
        user_t *user = it->data;
        if (user->client == client)
            return user;
    }
    return NULL;
}

void send_packet(user_t *user, char const *msg) {
    size_t len = strlen(msg);
    char packet[len + 2];
    strncpy(packet, msg, len);
    packet[len] = '\r';
    packet[len + 1] = '\n';
    network_client_send(user->client, packet, len + 2);
}

void kick_user(user_t *user, char *msg) {
    char *packet = msg ? msg : user->name;
    send_packet(user, packet);
    network_client_close(user->client);
}