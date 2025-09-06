#include "cub3d.h"

static char	**ft_realloc_map(char **old, size_t old_size, size_t new_size)
{
	char	**new;
	size_t	i;

	new = malloc((new_size + 1) * sizeof(char *));
	if (!new)
		return (NULL);
	new[new_size] = NULL;
	i = 0;
	while (old && old[i] && i < old_size && i < new_size)
	{
		new[i] = old[i];
		i++;
	}
	while (i < new_size)
		new[i++] = NULL;
	free(old);
	return (new);
}

static char	*ft_realloc_str(char *old, size_t new_size)
{
	char	*new;
	size_t	i;

	new = malloc(new_size + 1);
	if (!new)
	{
		free(old);
		return (NULL);
	}
	i = 0;
	while (old && old[i] && i < new_size)
	{
		new[i] = old[i];
		i++;
	}
	while (i <= new_size)
		new[i++] = '\0';
	free(old);
	return (new);
}

int	get_line(int fd, char **line)
{
	char	*temp;
	ssize_t	bytes_read;
	size_t	i;
	char	c;

	*line = NULL;
	i = 0;
	while (1)
	{
		bytes_read = read(fd, &c, 1);
		if (bytes_read <= 0)
		{
			if (i == 0)
				return (0);
			break;
		}
		if (c == '\n')
			break;
		temp = ft_realloc_str(*line, i + 1);
		if (!temp)
			return (-1);
		*line = temp;
		(*line)[i] = c;
		i++;
	}
	if (*line)
		(*line)[i] = '\0';
	return (1);
}

int get_data(char *map_file, char ***data)
{
	int		fd;
	char	*line;
	char	**temp;
	size_t	line_count;
	int		result;

	fd = open(map_file, O_RDONLY);
	if (fd < 0)
		return (perror("Error\n"), 1);
	*data = NULL;
	line_count = 0;
	while ((result = get_line(fd, &line)) > 0)
	{
		temp = ft_realloc_map(*data, line_count, line_count + 1);
		if (!temp)
		{
			free(line);
			close(fd);
			return (perror("Error\n"), 1);
		}
		*data = temp;
		(*data)[line_count] = line;
		line_count++;
	}
	close(fd);
	if (result < 0)
		return (perror("Error\n"), 1);
	return (0);
}

int main(int argc, char **argv)
{
    char  **data;
    t_walls walls;
    t_map_data map;

    if (argc != 2)
        return (write(2, "Usage: ./cub3d <map_file>\n", 25), 1);
    if (file_format(argv[1], ".cub"))
        return (write(2, "Error\nInvalid file format\n", 26), 1);
    if (get_data(argv[1], &data))
        return (1);
    if (split_data(&walls, &map, data))
        return (1);
    if (get_map(data))
        return (1);
    if (parse_map(data))
        return (1);
    free_map(data);
    return (0);
}
