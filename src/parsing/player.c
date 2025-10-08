#include "cub3d.h"

static void	set_orientation(t_player *player, char dir)
{
	if (dir == 'N')
	{
		player->dir_x = 0;
		player->dir_y = -1;
		player->plane_x = 0.66;
		player->plane_y = 0;
	}
	else if (dir == 'S')
	{
		player->dir_x = 0;
		player->dir_y = 1;
		player->plane_x = -0.66;
		player->plane_y = 0;
	}
	else if (dir == 'E')
	{
		player->dir_x = 1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = 0.66;
	}
	else if (dir == 'W')
	{
		player->dir_x = -1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = -0.66;
	}
}

static int	handle_cell(t_game *game, int x, int y, int *has_player)
{
	char	c;

	c = game->map.grid[y][x];
	if (c == '0' || c == '1' || c == ' ')
		return (0);
	if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
	{
		if (*has_player)
			return (print_error("Multiple player positions"));
		*has_player = 1;
		game->player.x = x + 0.5;
		game->player.y = y + 0.5;
		set_orientation(&game->player, c);
		game->map.grid[y][x] = '0';
		return (0);
	}
	return (print_error("Invalid character in map"));
}

int	setup_player(t_game *game)
{
	int	x;
	int	y;
	int	has_player;

	has_player = 0;
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (handle_cell(game, x, y, &has_player))
				return (1);
			x++;
		}
		y++;
	}
	if (!has_player)
		return (print_error("Missing player start"));
	return (0);
}
