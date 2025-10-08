#include "cub3d.h"

static int	parse_component(const char *line, int *index, int *value)
{
	int		start;
	char	*fragment;
	int		val;

	while (line[*index] && ft_is_space(line[*index]))
		(*index)++;
	start = *index;
	while (line[*index] && ft_is_digit(line[*index]))
		(*index)++;
	if (start == *index)
		return (print_error("Invalid color value"));
	fragment = ft_strndup(line + start, *index - start);
	if (ft_atoi_strict(fragment, &val) || val < 0 || val > 255)
	{
		free(fragment);
		return (print_error("Color out of range"));
	}
	free(fragment);
	while (line[*index] && ft_is_space(line[*index]))
		(*index)++;
	*value = val;
	return (0);
}

static int	assign_color(t_config *cfg, char type, t_color color)
{
	if (type == 'F')
	{
		if (cfg->floor_set)
			return (print_error("Duplicate floor color"));
		cfg->floor = color;
		cfg->floor_set = 1;
	}
	else
	{
		if (cfg->ceiling_set)
			return (print_error("Duplicate ceiling color"));
		cfg->ceiling = color;
		cfg->ceiling_set = 1;
	}
	return (0);
}

static int	require_comma(const char *line, int *index)
{
	if (line[*index] != ',')
		return (print_error("Missing comma in color"));
	(*index)++;
	return (0);
}

static int	parse_rgb_values(const char *line, int *idx, t_color *color)
{
	if (parse_component(line, idx, &color->r))
		return (-1);
	if (require_comma(line, idx))
		return (-1);
	if (parse_component(line, idx, &color->g))
		return (-1);
	if (require_comma(line, idx))
		return (-1);
	if (parse_component(line, idx, &color->b))
		return (-1);
	return (0);
}

static int	check_trailing_spaces(const char *line, int idx)
{
	while (line[idx])
	{
		if (!ft_is_space(line[idx]))
			return (print_error("Trailing characters in color"));
		idx++;
	}
	return (0);
}

int	parse_color_line(t_config *cfg, const char *line)
{
	t_color	color;
	int		idx;
	char	type;

	if ((line[0] != 'F' && line[0] != 'C') || !ft_is_space(line[1]))
		return (1);
	type = line[0];
	idx = 1;
	if (parse_rgb_values(line, &idx, &color))
		return (-1);
	if (check_trailing_spaces(line, idx))
		return (-1);
	if (assign_color(cfg, type, color))
		return (-1);
	return (0);
}
