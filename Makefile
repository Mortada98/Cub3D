NAME = cub3D
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRCS = main.c \
	get_data/get_walls.c \
	get_map/get_map.c \
	parse_map/parse_map.c \
	utils/clean_up.c \
	utils/file_format.c \
	utils/utils1.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
