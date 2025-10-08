#include "cub3d.h"

static void	draw_wall(t_game *game, int x, t_ray *ray)
{
	t_img	*tex;
	t_column	col;

	if (ray->perp_dist <= 0)
		ray->perp_dist = 0.0001;
	col.line_height = (int)(WIN_HEIGHT / ray->perp_dist);
	compute_wall_limits(col.line_height, &col.start, &col.end);
	tex = &game->texture[select_texture_index(game, ray)];
	col.tex_x = compute_tex_x(game, ray, tex);
	if (col.tex_x < 0)
		col.tex_x = 0;
	if (col.tex_x >= tex->width)
		col.tex_x = tex->width - 1;
	col.x = x;
	draw_texture_column(game, ray, tex, &col);
}

static void	render_column(t_game *game, int x)
{
	t_ray	ray;

	ray_init(game, &ray, x);
	ray_set_step(game, &ray);
	if (ray_step_until_hit(game, &ray) < 0)
		return ;
	if (ray.perp_dist > 0)
		draw_wall(game, x, &ray);
}

void	render_frame(t_game *game)
{
	int	x;

	render_background(game);
	x = 0;
	while (x < WIN_WIDTH)
	{
		render_column(game, x);
		x++;
	}
	mlx_put_image_to_window(game->mlx, game->win, game->frame.ptr, 0, 0);
}
