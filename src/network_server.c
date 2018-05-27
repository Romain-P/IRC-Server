//
// Created by romain on 19/05/18.
//

#include <stdlib.h>
#include <unistd.h>
#include "network.h"
#include "network_epoll.h"
#include "stderr.h"

static void config_socket(network_server_t *server) {
    server->id = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = server->config->host;
    sin.sin_port = htons(server->config->port);

    int status = 1;
    setsockopt(server->id, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(status));
    bind(server->id, (struct sockaddr*)&sin, sizeof(sin));
    listen(server->id, SOMAXCONN);
}

static void new_connection(network_server_t *server) {
    struct sockaddr_in in;
    socklen_t sz = sizeof(in);

    int new_connection = accept(server->id, (struct sockaddr*) &in, &sz);
    epoll_add_client(server->epoll_id, new_connection);

    network_client_t *client = malloc(sizeof(network_client_t));
    client->id = new_connection;
    client->length = 0;
    client->server = server;

    list_add(&server->clients, client);
    server->client_handler.on_connect(client);
}

static void read_data(network_server_t *server, int client) {
    char buffer[NETWORK_READ_SIZE];

    ssize_t bytes = read(client, buffer, NETWORK_READ_SIZE);
    network_client_t *found = network_client_find(&server->clients, client);

    if (bytes <= 0) {
        server->client_handler.on_disconnect(found);
        list_remove(&server->clients, found);
        network_client_close(found);
        network_client_free(found);
    } else {
        network_client_read(server, found, buffer, (size_t) bytes);
    }
}

static void on_client_ready(session_t client, void *server_ptr) {
    network_server_t *server = server_ptr;

    if (client == -1) {
        for (iter_t *it = iter_begin(&server->clients); it; iter_next(it)) {
            network_client_t *nc = it->data;
            server->client_handler.on_disconnect(nc);
            network_client_close(nc);
        }
        list_clear(&server->clients, &network_client_free);
        close(server->id);
        server->config->on_server_close();
    }
    else if (client == server->id)
        new_connection(server);
    else
        read_data(server, client);
}

void network_server_start(network_server_t *server, server_config_t *config) {
    server->config = config;
    config_socket(server);

    if ((server->epoll_id = (session_t) epoll_create_instance(server->id)) == ERROR)
        raise_error("Network epoll error: can't create epoll instance\n");

    server->clients = list_init;
    server->config->configure_handlers(&server->client_handler);

    epoll_listen(server->epoll_id, &on_client_ready, server);
}