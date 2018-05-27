//
// Created by romain on 27/05/18.
//

#include <string.h>
#include "irc.h"

void on_nickname_message(user_t *user, char **parameters) {
    user->name = strdup(parameters[0]);
}

void on_quit(user_t *user, char **parameters) {
    char *exit = parameters[0] ? parameters[0] : user->name;
    size_t len = strlen(exit);
    char msg[len + 2];
    msg[len + 1] = '\n';
    msg[len] = '\r';
    network_client_send(user->client, msg, len + 2);
    network_client_close(user->client);
    list_remove(&server.users, user);
}