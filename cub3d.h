#ifndef CUB3D_H
# define CUB3D_H

# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef struct s_walls
{
    char    *NO;
    char    *SO;
    char    *WE;
    char    *EA;
    char    *F;
    char    *C;
    int     width;
    int     height;
}               t_walls;

typedef struct s_map_data
{
    char        **map;
    size_t      width;
    size_t      height;
}               t_map_data;

typedef struct s_player
{
    double      x;
    double      y;
    char        direction;
}               t_player;

/*--------------------------------file format-------------------------------*/
int	    file_format(const char *name, const char *extension);

/*-------------------------data reading and parsing--------------------------*/
int     get_data(char *map_file, char ***data);
int     get_line(int fd, char **line);
int     get_walls(t_walls *walls, char **data);
int     split_data(t_walls *walls, t_map_data *map, char **data);
int     get_map(char **data);
int     parse_map(char **data);

/*----------------------------------utils----------------------------------*/
void    free_map(char **data);
int     ft_strlen(const char *str);
int     ft_strcmp(const char *s1, const char *s2);

#endif