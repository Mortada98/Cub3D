#include "cub3d.h"

static int	append_char(char **line, int len, char c)
{
	char	*tmp;

	tmp = safe_malloc(len + 2);
	if (*line)
	{
		ft_memcpy(tmp, *line, len);
		free(*line);
	}
	tmp[len] = c;
	tmp[len + 1] = '\0';
	*line = tmp;
	return (1);
}

int	read_line(int fd, char **line)
{
	char	buffer;
	int		bytes;
	int		len;

	if (!line)
		return (-1);
	*line = NULL;
	len = 0;
	while ((bytes = read(fd, &buffer, 1)) > 0)
	{
		if (buffer == '\n')
			break ;
		if (!append_char(line, len, buffer))
			return (-1);
		len++;
	}
	if (bytes < 0)
		return (-1);
	if (!*line)
		*line = ft_strdup("");
	if (bytes == 0 && len == 0)
		return (0);
	return (1);
}

char	**arr_push(char ***arr, int *len, char *line)
{
	char	**tmp;
	int		i;

	tmp = safe_malloc(sizeof(char *) * (*len + 2));
	i = 0;
	while (i < *len)
	{
		tmp[i] = (*arr)[i];
		i++;
	}
	tmp[i] = line;
	tmp[i + 1] = NULL;
	free(*arr);
	*arr = tmp;
	(*len)++;
	return (*arr);
}

void	free_str_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
