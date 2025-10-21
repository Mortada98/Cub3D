NAME		= cub3D

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -Iincludes -Iminilibx-linux
LDFLAGS		= -Lminilibx-linux -lmlx -lXext -lX11 -lm

SRC_FILES	= \
	src/main.c \
	src/parsing/parse_scene.c \
	src/parsing/parse_texture.c \
	src/parsing/parse_color.c \
	src/parsing/parse_map.c \
	src/parsing/player.c \
	src/parsing/validate.c \
	src/parsing/scene_config.c \
	src/render/image.c \
	src/render/background.c \
	src/render/ray_setup.c \
	src/render/ray_texture.c \
	src/render/raycast.c \
	src/hooks/events.c \
	src/game/init.c \
	src/game/update.c \
	src/utils/memory.c \
	src/utils/error.c \
	src/utils/string.c \
	src/utils/convert.c \
	src/utils/io.c \
	src/utils/dispose.c

OBJ_DIR		= build
OBJ_FILES	= $(SRC_FILES:%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.c includes/cub3d.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

bonus: all

.PHONY: all clean fclean re bonus
