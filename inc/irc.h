//
// Created by romain on 14/05/18.
//

#ifndef MYIRC_IRC_H
# define MYIRC_IRC_H

# define IRC_BUFFER_SIZE        (512)
# define IRC_PACKET_DELIMITER   ("\r\n")
# define IRC_PARAM_SEPARATOR    (" ")

# include "network.h"

typedef struct command_s command_t;
typedef struct irc_instance_s irc_instance_t;
typedef struct user_s user_t;

extern irc_instance_t server;

struct user_s {
    network_client_t *client;
    char *name;
};

struct irc_instance_s {
    list_t users;
};

struct command_s {
    char const *named;
    void (*handler)(user_t *client, char **parameters);
};

void configure_client_handler(client_handler_t *handler);

void parse_packet(network_client_t *client, char const *packet, size_t len);

user_t *find_user(network_client_t *client);

void send_packet(user_t *user, char const *msg);

void kick_user(user_t *user, char *msg);

#endif //MYIRC_IRC_H
