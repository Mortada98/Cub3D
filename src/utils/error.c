#include "cub3d.h"

int	print_error(const char *msg)
{
	int	len;

	write(2, "Error\n", 6);
	if (!msg)
		return (1);
	len = ft_strlen(msg);
	if (len > 0)
		write(2, msg, len);
	write(2, "\n", 1);
	return (1);
}
