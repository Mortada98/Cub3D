#include "cub3d.h"

static int	load_texture(t_game *game, int idx, char *path)
{
	t_img	*tex;

	tex = &game->texture[idx];
	tex->ptr = mlx_xpm_file_to_image(game->mlx, path,
			&tex->width, &tex->height);
	if (!tex->ptr)
		return (print_error("Failed to load texture"));
	tex->addr = mlx_get_data_addr(tex->ptr, &tex->bpp,
			&tex->line_len, &tex->endian);
	if (!tex->addr)
		return (print_error("Texture data error"));
	return (0);
}

static int	create_frame(t_game *game)
{
	game->frame.ptr = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
	if (!game->frame.ptr)
		return (print_error("Failed to create frame"));
	game->frame.addr = mlx_get_data_addr(game->frame.ptr, &game->frame.bpp,
			&game->frame.line_len, &game->frame.endian);
	if (!game->frame.addr)
		return (print_error("Frame data error"));
	game->frame.width = WIN_WIDTH;
	game->frame.height = WIN_HEIGHT;
	return (0);
}

int	init_game(t_game *game)
{
	int	i;

	ft_bzero(&game->input, sizeof(t_input));
	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error("mlx_init failed"));
	game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "cub3D");
	if (!game->win)
		return (print_error("Failed to create window"));
	if (create_frame(game))
		return (1);
	i = 0;
	while (i < TEX_COUNT)
	{
		if (load_texture(game, i, game->config.texture[i]))
			return (1);
		i++;
	}
	game->running = 1;
	return (0);
}

static void	destroy_textures(t_game *game)
{
	int	i;

	i = 0;
	while (i < TEX_COUNT)
	{
		if (game->texture[i].ptr)
			mlx_destroy_image(game->mlx, game->texture[i].ptr);
		game->texture[i].ptr = NULL;
		i++;
	}
}

void	destroy_game(t_game *game)
{
	if (game->frame.ptr)
		mlx_destroy_image(game->mlx, game->frame.ptr);
	destroy_textures(game);
	if (game->win)
		mlx_destroy_window(game->mlx, game->win);
	if (game->mlx)
	{
#ifdef __linux__
		mlx_destroy_display(game->mlx);
#endif
		free(game->mlx);
	}
	free_map(&game->map);
	free_config(&game->config);
}
