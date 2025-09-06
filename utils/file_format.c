#include "../cub3d.h"

int	file_format(const char *name, const char *extension)
{
	int ln;
    int le;
    int i;

    ln = ft_strlen(name);
    le = ft_strlen(extension);
	if (ln < le)
		return (1);
    i = 0;
	while (i < le)
    {
		if (name[ln - le + i] != extension[i])
			return (1);
		i++;
    }
	return (0);
}
