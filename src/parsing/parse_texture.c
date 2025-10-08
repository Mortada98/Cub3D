#include "cub3d.h"

static int	get_tex_index(const char *line, int *offset)
{
	if (!ft_strncmp(line, "NO", 2) && ft_is_space(line[2]))
	{
		*offset = 2;
		return (TEX_NO);
	}
	if (!ft_strncmp(line, "SO", 2) && ft_is_space(line[2]))
	{
		*offset = 2;
		return (TEX_SO);
	}
	if (!ft_strncmp(line, "WE", 2) && ft_is_space(line[2]))
	{
		*offset = 2;
		return (TEX_WE);
	}
	if (!ft_strncmp(line, "EA", 2) && ft_is_space(line[2]))
	{
		*offset = 2;
		return (TEX_EA);
	}
	return (-1);
}

static int	store_texture(t_config *cfg, int idx, const char *path)
{
	char	*trimmed;
	int		len;

	if (cfg->has_texture[idx])
		return (print_error("Duplicate texture entry"));
	while (*path && ft_is_space(*path))
		path++;
	len = ft_strlen(path);
	while (len > 0 && ft_is_space(path[len - 1]))
		len--;
	if (len == 0)
		return (print_error("Missing texture path"));
	trimmed = ft_strndup(path, len);
	if (!trimmed)
		return (print_error("Allocation failed"));
	cfg->texture[idx] = trimmed;
	cfg->has_texture[idx] = 1;
	return (0);
}

int	parse_texture_line(t_config *cfg, const char *line)
{
	int	idx;
	int	offset;

	offset = 0;
	idx = get_tex_index(line, &offset);
	if (idx < 0)
		return (1);
	line += offset;
	while (*line && ft_is_space(*line))
		line++;
	if (*line == '\0')
		return (print_error("Missing texture path"));
	if (store_texture(cfg, idx, line))
		return (-1);
	return (0);
}
