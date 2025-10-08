#include "cub3d.h"

static int	row_width(const char *line)
{
	int	i;

	i = 0;
	while (line && line[i])
		i++;
	return (i);
}

static void	fill_row(char *dst, const char *src, int width)
{
	int	i;

	i = 0;
	while (i < width)
	{
		if (src && src[i])
			dst[i] = src[i];
		else
			dst[i] = ' ';
		i++;
	}
	dst[width] = '\0';
}

static int	get_max_width(char **lines, int count)
{
	int	i;
	int	width;
	int	tmp;

	width = 0;
	i = 0;
	while (i < count)
	{
		tmp = row_width(lines[i]);
		if (tmp > width)
			width = tmp;
		i++;
	}
	return (width);
}

static void	build_grid_rows(t_map *map, char **lines, int width, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		map->grid[i] = safe_malloc(width + 1);
		fill_row(map->grid[i], lines[i], width);
		free(lines[i]);
		i++;
	}
}

int	finalize_map(t_map *map, char **lines, int count)
{
	int	width;

	if (count <= 0)
		return (print_error("Map section is empty"));
	width = get_max_width(lines, count);
	if (width == 0)
		return (print_error("Map width is zero"));
	map->grid = safe_malloc(sizeof(char *) * count);
	map->width = width;
	map->height = count;
	build_grid_rows(map, lines, width, count);
	return (0);
}
