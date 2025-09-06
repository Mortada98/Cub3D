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

int	get_line(int fd)
{
	char	*line;
	size_t	i;
	char	c;

	line = NULL;
	i = 0;
	while (read(fd, &c, 1) > 0 && c != '\n')
	{
		line = ft_realloc_str(line, i + 1);
		if (!line)
			return (NULL);
		line[i++] = c;
	}
	if (line)
		line[i] = '\0';
	return (line);
}

int get_data(char *map_file, char ***data)
{
	int     fd;
	char    *line;
	size_t  size;
	size_t  i;

	fd = open(map_file, O_RDONLY);
	if (fd < 0)
		return (write(2, "Error\nCould not open file\n", 27), 1);
	*data = NULL;
	size = 0;
	while ((line = get_line(fd)) != NULL)
	{
		char **temp = ft_realloc_map(*data, size, size + 1);
		if (!temp)
			return (write(2, "Error\nMemory allocation failed\n", 32), free(line), free_map(*data), close(fd), 1);
		*data = temp;
		(*data)[size] = line;
		(*data)[size + 1] = NULL;
		free(line);
		size++;
	}
	close(fd);
	return (0);
}

int main(int argc, char **argv)
{
    char  **data;
    t_walls walls;
	t_map_data map;

    if (argc != 2)
        return (write(2, "Error\nUsage: ./cub3d <map_file>\n", 31), 1);
    if (file_format(argv[1], ".cub"))
        return (write(2, "Error\nInvalid file format\n", 26), 1);
    if (get_data(argv[1], &data))
        return (1);
	if (split_data(&walls, &map, data))
		return (1);
	if (parse_data(&walls, &map))
		return (1);
    free_map(data);
    return (0);
}
