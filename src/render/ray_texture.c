#include "cub3d.h"

int	select_texture_index(t_game *game, t_ray *ray)
{
	(void)game;
	if (ray->side == 0)
	{
		if (ray->dir_x > 0)
			return (TEX_WE);
		return (TEX_EA);
	}
	if (ray->dir_y > 0)
		return (TEX_SO);
	return (TEX_NO);
}

void	compute_wall_limits(int line_height, int *start, int *end)
{
	*start = -line_height / 2 + WIN_HEIGHT / 2;
	if (*start < 0)
		*start = 0;
	*end = line_height / 2 + WIN_HEIGHT / 2;
	if (*end >= WIN_HEIGHT)
		*end = WIN_HEIGHT - 1;
}

int	compute_tex_x(t_game *game, t_ray *ray, t_img *tex)
{
	double	wall_x;
	int		tex_x;

	if (ray->side == 0)
		wall_x = game->player.y + ray->perp_dist * ray->dir_y;
	else
		wall_x = game->player.x + ray->perp_dist * ray->dir_x;
	wall_x -= floor(wall_x);
	tex_x = (int)(wall_x * tex->width);
	if (ray->side == 0 && ray->dir_x > 0)
		tex_x = tex->width - tex_x - 1;
	if (ray->side == 1 && ray->dir_y < 0)
		tex_x = tex->width - tex_x - 1;
	return (tex_x);
}

void	draw_texture_column(t_game *game, t_ray *ray, t_img *tex, t_column *col)
{
	double	step;
	double	tex_pos;
	int		y;
	int		color;

	step = (double)tex->height / col->line_height;
	tex_pos = (col->start - WIN_HEIGHT / 2 + col->line_height / 2) * step;
	y = col->start;
	while (y <= col->end)
	{
		color = get_color_from_tex(tex, col->tex_x, (int)tex_pos);
		if (ray->side == 1)
			color = (color >> 1) & 0x7F7F7F;
		draw_pixel(&game->frame, col->x, y, color);
		tex_pos += step;
		y++;
	}
}
