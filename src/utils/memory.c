#include "cub3d.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*d;
	const unsigned char	*s;

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dst);
}

void	ft_bzero(void *ptr, size_t size)
{
	size_t			i;
	unsigned char	*p;

	p = (unsigned char *)ptr;
	i = 0;
	while (i < size)
	{
		p[i] = 0;
		i++;
	}
}

void	*safe_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		write(2, "Error\nmalloc failed\n", 21);
		exit(EXIT_FAILURE);
	}
	return (ptr);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;

	ptr = safe_malloc(count * size);
	ft_bzero(ptr, count * size);
	return (ptr);
}
