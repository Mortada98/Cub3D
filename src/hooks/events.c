#include "cub3d.h"

static void	set_movement_flag(t_input *input, int keycode, int value)
{
	if (keycode == KEY_W)
		input->forward = value;
	else if (keycode == KEY_S)
		input->backward = value;
	else if (keycode == KEY_A)
		input->left = value;
	else if (keycode == KEY_D)
		input->right = value;
	else if (keycode == KEY_LEFT)
		input->turn_left = value;
	else if (keycode == KEY_RIGHT)
		input->turn_right = value;
}

int	handle_key_press(int keycode, void *param)
{
	t_game	*game;

	game = (t_game *)param;
	if (keycode == KEY_ESC)
		return (handle_close(param));
	set_movement_flag(&game->input, keycode, 1);
	return (0);
}

int	handle_key_release(int keycode, void *param)
{
	t_game	*game;

	game = (t_game *)param;
	set_movement_flag(&game->input, keycode, 0);
	return (0);
}

int	handle_close(void *param)
{
	t_game	*game;

	game = (t_game *)param;
	game->running = 0;
	destroy_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}

int	game_loop(void *param)
{
	t_game	*game;

	game = (t_game *)param;
	if (!game->running)
		return (0);
	update_game_state(game);
	render_frame(game);
	return (0);
}
