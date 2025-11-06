#include "cub3d.h"

static int	check_extension(const char *path)
{
	int	len;

	len = ft_strlen(path);
	if (len < 4)
		return (0);
	return (!ft_strncmp(path + len - 4, ".cub", 4));
}

static void	init_game_struct(t_game *game)
{
	ft_bzero(game, sizeof(t_game));
}

static void	setup_hooks(t_game *game)
{
	mlx_hook(game->win, EVENT_KEY_PRESS, MASK_KEY_PRESS, handle_key_press, game);
	mlx_hook(game->win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE, handle_key_release, game);
	mlx_hook(game->win, EVENT_DESTROY, 0, handle_close, game);
	mlx_loop_hook(game->mlx, game_loop, game);
}

int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
		return (print_error("Usage: ./cub3D <map.cub>"));
	if (!check_extension(argv[1]))
		return (print_error("Invalid file extension"));
	init_game_struct(&game);
	if (parse_scene(&game, argv[1]))
	{
		destroy_game(&game);
		return (1);
	}
	if (init_game(&game))
	{
		destroy_game(&game);
		return (1);
	}
	setup_hooks(&game);
	mlx_loop(game.mlx);
	destroy_game(&game);
	return (0);
}
