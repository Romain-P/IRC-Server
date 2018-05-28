//
// Created by romain on 27/05/18.
//

#include <string.h>
#include "irc.h"
#include "util.h"

static void on_nickname_message(user_t *user, char **parameters);
static void on_quit(user_t *user, char **parameters);

static command_t const commands[] = {
        {"NICK", &on_nickname_message },
        {"QUIT", &on_quit },
        { NULL, NULL }
};

static void on_nickname_message(user_t *user, char **parameters) {
    user->name = strdup(parameters[0]);
}

static void on_quit(user_t *user, char **parameters) {
    if (*parameters && **parameters)
        kick_user(user, *parameters);
    else
        kick_user(user, NULL);
}

void parse_packet(network_client_t *client, char const *packet, size_t len) {
    char **split = charset_split(packet, len, IRC_PARAM_SEPARATOR);

    for (int i = 0; commands[i].named; ++i) {
        if (!strcmp(commands[i].named, split[0])) {
            commands[i].handler(find_user(client), split + 1);
            break;
        }
    }

    str_free_array(split);
}