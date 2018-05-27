CC      = gcc

RM      = rm -f

CFLAGS  += -W -Wall -Wextra -Iinc

NAME    = server

SRCS    =	src/client_handler.c	\
		src/linked_list.c	\
		src/main.c		\
		src/network_client.c	\
		src/network_epoll.c	\
		src/network_server.c	\
		src/util.c

OBJS    = $(SRCS:.c=.o)


$(NAME):	$(OBJS)
		$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
