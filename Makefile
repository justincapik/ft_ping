NAME		=	ft_ping

CC			=	gcc

##
##		FILE DESCRIPTOR
##

INCLUDE = includes

SRC_PATH = srcs

SRCS =	main.c			\
		parsing.c		\
		lookups.c		\
		packets.c		\
		loop.c


##
##		SETTING VPATH
##

vpath %.c $(foreach dir, $(SRC_PATH), $(dir):)


##
##		DEPENDENCE DESCRIPTOR
##

IDEP = includes/ft_ping.h

OBJ_PATH = objs

OBJS = $(addprefix $(OBJ_PATH)/, $(SRCS:.c=.o))

##
##		FLAGS CONSTRUCTION
##

CFLAGS += -Wall -Wextra -Werror 
LFLAGS =

IFLAGS = 	$(foreach dir, $(INCLUDE), -I$(dir) ) \

$(OBJ_PATH)/%.o:	%.c $(IDEP)
	$(CC) -c $< -o $@ $(CFLAGS) $(IFLAGS) $(LFLAGS)


all:		$(NAME)

$(NAME):	$(OBJ_PATH) $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(CFLAGS) $(LFLAGS) $(IFLAGS)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -rf $(NAME)

$(OBJ_PATH):
	mkdir $(OBJ_PATH)

re:			fclean all

.SILENT:	all $(NAME) fclean clean re 
.PHONY:		clean fclean re