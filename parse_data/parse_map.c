#include "../cub3d.h"

int is_valid(t_map_data *map, int i, int j)
{
    if (i == 0 || j == 0 || i == map->height - 1 || j == map->width - 1)
        return (write(2, "Error\nMap is not enclosed\n", 26), 1);
    if (map->map[i-1][j] == ' ' || map->map[i+1][j] == ' ' ||
        map->map[i][j-1] == ' ' || map->map[i][j+1] == ' ')
        return (write(2, "Error\nMap is not enclosed\n", 26), 1);
    return 0;
}

int parse_maps(t_map_data *map)
{
    int i;
    int j;

    i = 0;
    while (map->map[i][0] == '\n')
        i++;
    while (map->map[i])
    {
        j = 0;
        while (map->map[i][j])
        {
            while (map->map[i][j] == ' ')
                j++;
            if (map->map[i][j] != '0')
            {
                if (is_valid(map, i, j))
                    return (1);
            }
        }
        i++;
    }
    return 0;
}

int parse_data(t_walls *walls, t_map_data *map)
{
    if (parse_walls(walls))
        return (1);
    if (parse_maps(map))
        return (1);
    return (0);
}
