/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 10:44:46 by mbouizak          #+#    #+#             */
/*   Updated: 2025/09/05 21:32:25 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

int	ft_strlen(const char *str)
{
	int i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *s)
{
	char	*c;
	size_t	slen;
	size_t	i;

	slen = ft_strlen(s);
	i = 0;
	c = (char *)malloc((slen + 1) * sizeof(char));
	if (c == 0)
		return (0);
	while (s[i] != '\0')
	{
		c[i] = s[i];
		i++;
	}
	c[i] = '\0';
	return (c);
}

char	*ft_strjoin(const char *s1, const char *s2)
{
	size_t	i;
	size_t	j;
	size_t	s1len;
	size_t	s2len;
	char	*s;

	if (s1 == 0 && s2 == 0)
		return (0);
	s1len = ft_strlen(s1);
	s2len = ft_strlen(s2);
	i = 0;
	j = 0;
	s = (char *)malloc((s1len + s2len + 1) * sizeof(char));
	if (s == 0)
		return (0);
	while (s1[i] != '\0')
		s[j++] = s1[i++];
	i = 0;
	while (s2[i] != '\0')
		s[j++] = s2[i++];
	s[j] = '\0';
	return (s);
}

int	my_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == (char)c)
			return (i);
		i++;
	}
	if (s[i] == (char)c)
		return (i);
	return (-1);
}

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*c;
	size_t	i;
	size_t	slen;

	i = 0;
	slen = ft_strlen(s);
	if (start > slen)
		return (ft_strdup(""));
	if (len > slen - start)
		len = slen - start;
	c = (char *)malloc((len + 1) * sizeof(char));
	if (c == 0)
		return (0);
	while (i < len && s[start] != '\0')
	{
		c[i] = s[start + i];
		i++;
	}
	c[i] = '\0';
	return (c);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}