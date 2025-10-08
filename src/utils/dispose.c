#include "cub3d.h"

void	free_config(t_config *cfg)
{
	int	i;

	i = 0;
	while (i < TEX_COUNT)
	{
		if (cfg->texture[i])
			free(cfg->texture[i]);
		cfg->texture[i] = NULL;
		cfg->has_texture[i] = 0;
		i++;
	}
	cfg->floor_set = 0;
	cfg->ceiling_set = 0;
}

void	free_map(t_map *map)
{
	int	i;

	i = 0;
	while (map->grid && i < map->height)
	{
		free(map->grid[i]);
		i++;
	}
	free(map->grid);
	map->grid = NULL;
	map->height = 0;
	map->width = 0;
}
