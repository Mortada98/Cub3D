#include "cub3d.h"
#include <stdio.h>

static void print_color(const char *name, t_color *c, int set)
{
	if (!set)
		printf("%s: (not set)\n", name);
	else
		printf("%s: r=%d g=%d b=%d\n", name, c->r, c->g, c->b);
}

static void print_config(t_config *cfg)
{
	const char *names[] = {"NO","SO","WE","EA"};
	int i;

	printf("--- CONFIG ---\n");
	for (i = 0; i < TEX_COUNT; i++)
	{
		if (cfg->has_texture[i] && cfg->texture[i])
			printf("%s: %s\n", names[i], cfg->texture[i]);
		else
			printf("%s: (not set)\n", names[i]);
	}
	print_color("FLOOR", &cfg->floor, cfg->floor_set);
	print_color("CEILING", &cfg->ceiling, cfg->ceiling_set);
}

static void print_map(t_map *map)
{
	int y;

	printf("--- MAP (width=%d height=%d) ---\n", map->width, map->height);
	for (y = 0; y < map->height; y++)
	{
		printf("%s\n", map->grid[y]);
	}
}

static void print_player(t_player *p)
{
	printf("--- PLAYER ---\n");
	printf("pos: x=%.2f y=%.2f\n", p->x, p->y);
	printf("dir: x=%.2f y=%.2f\n", p->dir_x, p->dir_y);
	printf("plane: x=%.2f y=%.2f\n", p->plane_x, p->plane_y);
}

int main(int argc, char **argv)
{
	t_game game;
	int status;

	if (argc < 2)
	{
		printf("Usage: %s <map.cub>\n", argv[0]);
		return 1;
	}
	/* zero/init minimal fields to avoid garbage */
	ft_bzero(&game, sizeof(game));

	status = parse_scene(&game, argv[1]);
	if (status)
	{
		printf("Failed to parse scene: %d\n", status);
		return 2;
	}

	print_config(&game.config);
	print_map(&game.map);
	print_player(&game.player);

	/* cleanup allocated memory using existing free helpers if available */
	free_map(&game.map);
	free_config(&game.config);

	return 0;
}
