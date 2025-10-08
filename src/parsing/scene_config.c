#include "cub3d.h"

int	scene_config_ready(t_config *cfg)
{
	int	i;

	if (!cfg->floor_set || !cfg->ceiling_set)
		return (0);
	i = 0;
	while (i < TEX_COUNT)
	{
		if (!cfg->has_texture[i])
			return (0);
		i++;
	}
	return (1);
}

static int	save_trimmed_config(t_game *game, char *trimmed)
{
	int	status;

	status = parse_texture_line(&game->config, trimmed);
	if (status == -1)
		return (1);
	if (status == 0)
		return (0);
	status = parse_color_line(&game->config, trimmed);
	if (status == -1)
		return (1);
	if (status == 0)
		return (0);
	return (print_error("Invalid configuration entry"));
}

int	scene_process_entry(t_game *game, char *raw)
{
	char	*trimmed;
	int		result;

	trimmed = ft_strdup(raw);
	if (!trimmed)
		return (print_error("Allocation failed"));
	trim_spaces(trimmed);
	if (trimmed[0] == '\0')
	{
		free(trimmed);
		return (0);
	}
	result = save_trimmed_config(game, trimmed);
	free(trimmed);
	return (result);
}
