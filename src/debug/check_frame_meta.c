#include "mlx.h"
#include "cub3d.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    void *mlx;
    void *win;
    void *img;
    char *data;
    int bpp, line_len, endian;

    mlx = mlx_init();
    if (!mlx)
    {
        fprintf(stderr, "mlx_init failed\n");
        return 1;
    }
    win = mlx_new_window(mlx, WIN_WIDTH, WIN_HEIGHT, "frame-meta");
    if (!win)
    {
        fprintf(stderr, "mlx_new_window failed\n");
        return 1;
    }
    img = mlx_new_image(mlx, WIN_WIDTH, WIN_HEIGHT);
    if (!img)
    {
        fprintf(stderr, "mlx_new_image failed\n");
        return 1;
    }
    data = mlx_get_data_addr(img, &bpp, &line_len, &endian);
    printf("frame.addr = %p\n", (void *)data);
    printf("frame.bpp = %d\n", bpp);
    printf("frame.line_len = %d\n", line_len);
    printf("frame.endian = %d\n", endian);

    /* cleanup */
    mlx_destroy_image(mlx, img);
    mlx_destroy_window(mlx, win);
#ifdef __linux__
    mlx_destroy_display(mlx);
#endif
    free(mlx);
    return 0;
}
