//
// Created by romain on 19/05/18.
//

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include "network_epoll.h"
#include "stderr.h"

void epoll_listen(session_t epoll_instance, on_data_received callback, void *network_ptr) {
    struct epoll_event ev;
    struct signalfd_siginfo info;
    sigset_t all;
    sigset_t sw;

    sigfillset(&all);
    sigprocmask(SIG_SETMASK, &all, NULL);
    sigemptyset(&sw);
    sigaddset(&sw, SIGINT);

    int signal_fd = signalfd(-1, &sw, 0);
    epoll_add_client(epoll_instance, signal_fd);

    while (epoll_wait(epoll_instance, &ev, 1, -1) > 0) {
        int client_notifier = ev.data.fd;

        if (client_notifier == signal_fd) {
            if (read(signal_fd, &info, sizeof(info)) != sizeof(info))
                continue;
            else if (info.ssi_signo == SIGINT) {
                callback(-1, network_ptr);
                break;
            }
        }
        else
            callback(client_notifier, network_ptr);
    }

    epoll_del_client(epoll_instance, signal_fd);
    close(signal_fd);
}

ssize_t epoll_create_instance(network_server_id_t server) {
    session_t epoll_instance = epoll_create(1);

    return epoll_add_client(epoll_instance, server) ? epoll_instance : -1;
}

bool epoll_add_client(session_t epoll_id, session_t id) {
    epoll_t epoll = { 0 };

    epoll.events = EPOLLIN;
    epoll.data.fd = id;
    if (epoll_ctl(epoll_id, EPOLL_CTL_ADD, id, &epoll) == ERROR)
        return eprintf("[EpollInstance %d]: error adding new client (id=%d)\n", epoll_id, id), false;
    return true;
}

bool epoll_del_client(session_t epoll_id, session_t id) {
    epoll_t epoll = { 0 };
    epoll.events = EPOLLIN;
    epoll.data.fd = id;

    if (epoll_ctl(epoll_id, EPOLL_CTL_DEL, id, &epoll) == ERROR)
        eprintf("[EpollInstance %d]: error deleting new client (id=%d)\n", epoll_id, id), false;
    return true;
}