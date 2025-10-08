#include "cub3d.h"

static int	check_neighbors(t_map *map, int x, int y)
{
	int	dx;
	int	dy;

	if (x == 0 || y == 0 || x == map->width - 1 || y == map->height - 1)
		return (print_error("Map not enclosed by walls"));
	dy = -1;
	while (dy <= 1)
	{
		dx = -1;
		while (dx <= 1)
		{
			if ((dx != 0 || dy != 0)
				&& map->grid[y + dy][x + dx] == ' ')
				return (print_error("Map has open space"));
			dx++;
		}
		dy++;
	}
	return (0);
}

static int	check_map_cells(t_map *map)
{
	int	x;
	int	y;
	char	c;

	y = 0;
	while (y < map->height)
	{
		x = 0;
		while (x < map->width)
		{
			c = map->grid[y][x];
			if (c != '0' && c != '1' && c != ' ')
				return (print_error("Invalid character in map"));
			if (c == '0' && check_neighbors(map, x, y))
				return (1);
			x++;
		}
		y++;
	}
	return (0);
}

static int	validate_config(t_config *cfg)
{
	int	i;

	if (!cfg->floor_set || !cfg->ceiling_set)
		return (print_error("Missing floor or ceiling color"));
	i = 0;
	while (i < TEX_COUNT)
	{
		if (!cfg->has_texture[i])
			return (print_error("Missing texture definition"));
		i++;
	}
	return (0);
}

int	validate_scene(t_game *game)
{
	if (validate_config(&game->config))
		return (1);
	if (setup_player(game))
		return (1);
	if (check_map_cells(&game->map))
		return (1);
	return (0);
}
