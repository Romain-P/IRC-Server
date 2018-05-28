//
// Created by romain on 26/05/18.
//

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "network.h"
#include "util.h"
#include "network_epoll.h"

void network_client_close(network_client_t *client) {
    epoll_del_client(client->server->epoll_id, client->id);
    close(client->id);
    client->closed = true;
}

void network_client_free(void *ptr) {
    network_client_t *client = ptr;

    if (client->bstate != FREED)
        free(client->buffer);
    free(client);
}

network_client_t *network_client_find(list_t *network_clients, session_t id) {
    network_client_t *current = NULL;

    for (iter_t *it = iter_begin(network_clients); it; iter_next(it)) {
        current = it->data;
        if (current->id == id)
            return current;
    }
    return NULL;
}

static bool too_much_data(network_client_t *client, size_t size, size_t max_size) {
    if (size <= max_size)
        return false;
    free(client->buffer);
    client->length = 0;
    client->bstate = FREED;
    return true;
}

void network_client_read(network_server_t *server,
                         network_client_t *client,
                         char *buffer,
                         size_t length)
{
    if (client->bstate == FREED) {
        client->buffer = malloc(sizeof(char) * length);
        client->length = 0;
        client->bstate = INCOMPLETE;
    }
    else if (client->bstate == INCOMPLETE)
        client->buffer = realloc(client->buffer, sizeof(char) * (client->length + length));

    strncpy(client->buffer + client->length, buffer, length);
    client->length += length;

    size_t max_size = server->config->packet_max_size;
    char const *delimiter = server->config->packet_delimiter;
    char **split = charset_split(client->buffer, client->length, delimiter);
    size_t len = str_array_length(split);

    bool data_remaining = !charset_ends_with(buffer, length, delimiter);
    if (data_remaining && not_split(split) && too_much_data(client, client->length, max_size)) goto done;

    for (size_t i = 0; i < len - data_remaining; ++i) {
        char *packet = split[i];
        size_t packet_length = strlen(packet);

        if (!too_much_data(client, packet_length, max_size)) {
            server->client_handler.on_received(client, packet, packet_length);
            if (client->closed) return;
        }
    }

    done:
    if (data_remaining) {
        if (client->bstate != FREED)
            free(client->buffer);
        client->length = strlen(split[len - 1]);
        client->buffer = malloc(sizeof(char) * (client->length));
        strncpy(client->buffer, split[len - 1], client->length);
    } else if (client->bstate != FREED) {
        free(client->buffer);
        client->bstate = FREED;
    }
    str_free_array(split);
}

void network_client_send(network_client_t *client, char *packet, size_t len) {
    send(client->id, packet, len, 0);
    client->server->client_handler.on_sent(client, packet, len);
}