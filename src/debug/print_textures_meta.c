#include "cub3d.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    t_game game;
    int i;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <map.cub>\n", argv[0]);
        return 1;
    }
    ft_bzero(&game, sizeof(game));
    if (parse_scene(&game, argv[1]))
    {
        fprintf(stderr, "parse_scene failed\n");
        return 2;
    }
    if (init_game(&game))
    {
        fprintf(stderr, "init_game failed\n");
        destroy_game(&game);
        return 3;
    }

    printf("--- TEXTURES META ---\n");
    for (i = 0; i < TEX_COUNT; i++)
    {
        t_img *tex = &game.texture[i];
        printf("TEX %d:\n", i);
        printf("  ptr: %p\n", tex->ptr);
        printf("  addr: %p\n", tex->addr);
        printf("  width: %d height: %d\n", tex->width, tex->height);
        printf("  bpp: %d\n", tex->bpp);
        printf("  line_len: %d\n", tex->line_len);
        printf("  endian: %d\n", tex->endian);
        printf("\n");
    }

    destroy_game(&game);
    return 0;
}
