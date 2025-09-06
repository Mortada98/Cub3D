#include "../cub3d.h"

void free_map(char **data)
{
    int i;

    if (!data)
        return;
    i = 0;
    while (data[i])
        free(data[i++]);
    free(data);
}