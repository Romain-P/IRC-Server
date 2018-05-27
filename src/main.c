#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "linked_list.h"
#include "stderr.h"
#include "network.h"
#include "irc.h"

static void on_server_close() {
    printf("worked\n");
}

int main(int ac, char **args) {
    network_server_t server;

    server_config_t config = {
            .host = INADDR_ANY,
            .port = atoi(args[1]),
            .packet_delimiter = "\r\n",
            .packet_max_size = IRC_BUFFER_SIZE,
            .configure_handlers = &configure_client_handler,
            .on_server_close = &on_server_close
    };

    network_server_start(&server, &config);
    return 0;
}