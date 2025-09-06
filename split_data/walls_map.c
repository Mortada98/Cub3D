/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   walls_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 09:02:49 by mbouizak          #+#    #+#             */
/*   Updated: 2025/09/06 15:42:51 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

int   get_walls_helper(t_walls *walls, char *line)
{
    if (ft_strcmp(line, "F ") == 0 && !walls->F)
        walls->F = strdup(line);
    else if (ft_strcmp(line, "C ") == 0 && !walls->C)
        walls->C = strdup(line);
    else
        return (write(2, "Error\nInvalid data\n", 20), 1);
    return (0);
}

int    get_walls(t_walls *walls, t_map_data *map, char **data)
{
    int     i;

    i = 0;
    while (data[i])
    {
        while (data[i][0] == '\n')
            i++;
        if (ft_strcmp(data[i], "NO ") == 0 && !walls->NO)
            walls->NO = strdup(data[i]);
        else if (ft_strcmp(data[i], "SO ") == 0 && !walls->SO)
            walls->SO = strdup(data[i]);
        else if (ft_strcmp(data[i], "WE ") == 0 && !walls->WE)
            walls->WE = strdup(data[i]);
        else if (ft_strcmp(data[i], "EA ") == 0 && !walls->EA)
            walls->EA = strdup(data[i]);
        if (get_walls_helper(walls, data[i]))
            return (1);
        if (walls->NO && walls->SO && walls->WE && walls->EA && walls->F && walls->C)
            break ;
        i++;
    }
    return (i);
}
int    split_data(t_walls *walls, t_map_data *map, char **data)
{
    int     i;
    size_t  len;

    memset(walls, 0, sizeof(t_walls));
    i = get_walls(walls, map, data);
    if (i < 0)
        return (1);
    map->map = &data[i];
    map->height = 0;
    map->width = 0;
    while (map->map[map->height])
    {
        len = strlen(map->map[map->height]);
        if (len > map->width)
            map->width = len;
        map->height++;
    }
    if (!walls->NO || !walls->SO || !walls->WE || !walls->EA || !walls->F || !walls->C)
        return (write(2, "Error\nIncomplete wall data\n", 28), 1);
    return (0);
}
