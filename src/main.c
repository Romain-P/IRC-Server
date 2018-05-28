#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "linked_list.h"
#include "stderr.h"
#include "network.h"
#include "irc.h"

irc_instance_t server = {
        .users = list_init
};

static void on_server_close() {
    //free anything we want (users already freed by network see @on_disconnect)
}

int main(int ac, char **args) {
    if (ac != 2) {
        eprintf("USAGE: %s [port]\n", *args);
        return 84;
    }

    network_server_t server;

    server_config_t config = {
            .host = INADDR_ANY,
            .port = atoi(args[1]),
            .packet_delimiter = IRC_PACKET_DELIMITER,
            .packet_max_size = IRC_BUFFER_SIZE,
            .configure_handlers = &configure_client_handler,
            .on_server_close = &on_server_close
    };

    network_server_start(&server, &config);
    return 0;
}