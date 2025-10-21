#include "cub3d.h"

int	ft_strlen(const char *s)
{
	int	len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

char	*ft_strdup(const char *s)
{
	char	*dup;
	int		len;
	int		i;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	dup = safe_malloc(len + 1);
	i = 0;
	while (i < len)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;
	size_t	i;

	dup = safe_malloc(n + 1);
	i = 0;
	while (i < n && s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (s1[i] || s2[i]))
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}

int	ft_is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\v' || c == '\f');
}

int	ft_is_digit(int c)
{
	return (c >= '0' && c <= '9');
}

int	is_all_space(const char *line)
{
	int	i;

	i = 0;
	if (!line)
		return (1);
	while (line[i])
	{
		if (!ft_is_space(line[i]))
			return (0);
		i++;
	}
	return (1);
}

char	*trim_spaces(char *str)
{
	int	start;
	int	end;
	int	i;

	if (!str)
		return (NULL);
	start = 0;
	while (str[start] && ft_is_space(str[start]))
		start++;
	end = ft_strlen(str) - 1;
	while (end >= start && ft_is_space(str[end]))
	{
		str[end] = '\0';
		end--;
	}
	if (start == 0)
		return (str);
	i = 0;
	while (str[start + i])
	{
		str[i] = str[start + i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

int	is_map_line(const char *line)
{
	int	i;
	int	has_map_char;

	i = 0;
	has_map_char = 0;
	if (!line)
		return (0);
	while (line[i])
	{
		if (line[i] == '0' || line[i] == '1')
			has_map_char = 1;
		else if (line[i] == 'N' || line[i] == 'S'
			|| line[i] == 'E' || line[i] == 'W')
			has_map_char = 1;
		else if (line[i] != ' ' && line[i] != '\t')
			return (0);
		i++;
	}
	return (has_map_char);
}
