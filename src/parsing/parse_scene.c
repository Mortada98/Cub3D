#include "cub3d.h"

static int	handle_config_line(t_game *game, char **line,
			char ***map_lines, int *map_started)
{
	if (*map_started || is_map_line(*line))
	{
		if (!scene_config_ready(&game->config))
			return (print_error("Map before textures/colors"));
		if (is_all_space(*line))
			return (print_error("Empty line inside map"));
		*map_started = 1;
		arr_push(map_lines, &game->map.height, *line);
		*line = NULL;
		return (0);
	}
	return (scene_process_entry(game, *line));
}

static int	read_config_and_map(int fd, t_game *game,
			char ***map_lines, int *map_started)
{
	int		status;
	char	*line;

	while ((status = read_line(fd, &line)) >= 0)
	{
		if (status == 0)
		{
			free(line);
			break ;
		}
		if (handle_config_line(game, &line, map_lines, map_started))
		{
			free(line);
			return (-1);
		}
		if (line)
			free(line);
	}
	return (status);
}

static int	load_scene_lines(const char *path, t_game *game,
			char ***map_lines, int *map_started)
{
	int		status;
	int		fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (print_error("Unable to open .cub file"));
	status = read_config_and_map(fd, game, map_lines, map_started);
	close(fd);
	if (status < 0)
	{
		free_str_array(*map_lines);
		return (print_error("Failed to read file"));
	}
	return (0);
}

static int	finalize_scene_data(t_game *game, char **map_lines, int map_started)
{
	if (!map_started)
	{
		free_str_array(map_lines);
		return (print_error("Missing map section"));
	}
	if (finalize_map(&game->map, map_lines, game->map.height))
	{
		free_str_array(map_lines);
		return (1);
	}
	free(map_lines);
	if (validate_scene(game))
		return (1);
	return (0);
}

int	parse_scene(t_game *game, const char *path)
{
	char	**map_lines;
	int		map_started;

	map_lines = NULL;
	map_started = 0;
	game->map.height = 0;
	if (load_scene_lines(path, game, &map_lines, &map_started))
		return (1);
	return (finalize_scene_data(game, map_lines, map_started));
}
