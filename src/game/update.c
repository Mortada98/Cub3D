#include "cub3d.h"

static int	is_blocked(t_game *game, double x, double y)
{
	int	grid_x;
	int	grid_y;

	grid_x = (int)x;
	grid_y = (int)y;
	if (grid_x < 0 || grid_y < 0 || grid_x >= game->map.width
		|| grid_y >= game->map.height)
		return (1);
	if (game->map.grid[grid_y][grid_x] == '1'
		|| game->map.grid[grid_y][grid_x] == ' ')
		return (1);
	return (0);
}

static void	move_player(t_game *game, double offset_x, double offset_y)
{
	double	new_x;
	double	new_y;

	new_x = game->player.x + offset_x;
	new_y = game->player.y + offset_y;
	if (!is_blocked(game, new_x, game->player.y))
		game->player.x = new_x;
	if (!is_blocked(game, game->player.x, new_y))
		game->player.y = new_y;
}

static void	update_position(t_game *game)
{
	double	speed;

	speed = MOVE_SPEED;
	if (game->input.forward)
		move_player(game, game->player.dir_x * speed,
			game->player.dir_y * speed);
	if (game->input.backward)
		move_player(game, -game->player.dir_x * speed,
			-game->player.dir_y * speed);
	if (game->input.right)
		move_player(game, game->player.plane_x * speed,
			game->player.plane_y * speed);
	if (game->input.left)
		move_player(game, -game->player.plane_x * speed,
			-game->player.plane_y * speed);
}

static void	apply_rotation(t_game *game)
{
	double	angle;
	double	old_dir_x;
	double	old_plane_x;

	angle = 0.0;
	if (game->input.turn_left)
		angle -= ROT_SPEED;
	if (game->input.turn_right)
		angle += ROT_SPEED;
	if (angle == 0.0)
		return ;
	old_dir_x = game->player.dir_x;
	old_plane_x = game->player.plane_x;
	game->player.dir_x = game->player.dir_x * cos(angle)
		- game->player.dir_y * sin(angle);
	game->player.dir_y = old_dir_x * sin(angle)
		+ game->player.dir_y * cos(angle);
	game->player.plane_x = game->player.plane_x * cos(angle)
		- game->player.plane_y * sin(angle);
	game->player.plane_y = old_plane_x * sin(angle)
		+ game->player.plane_y * cos(angle);
}

void	update_game_state(t_game *game)
{
	update_position(game);
	apply_rotation(game);
}
