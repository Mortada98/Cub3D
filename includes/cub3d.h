#ifndef CUB3D_H
# define CUB3D_H

# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <math.h>
# include <mlx.h>

# define WIN_WIDTH 1280
# define WIN_HEIGHT 720

# define TEX_NO 0
# define TEX_SO 1
# define TEX_WE 2
# define TEX_EA 3
# define TEX_COUNT 4

# define KEY_ESC 65307
# define KEY_LEFT 65361
# define KEY_RIGHT 65363
# define KEY_W 119
# define KEY_A 97
# define KEY_S 115
# define KEY_D 100

# define MOVE_SPEED 0.08
# define ROT_SPEED 0.045

# define EVENT_KEY_PRESS 2
# define EVENT_KEY_RELEASE 3
# define EVENT_DESTROY 17
# define MASK_KEY_PRESS (1L << 0)
# define MASK_KEY_RELEASE (1L << 1)

typedef struct s_color
{
	int	r;
	int	g;
	int	b;
}	t_color;

typedef struct s_img
{
	void	*ptr;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
	int		width;
	int		height;
}	t_img;

typedef struct s_config
{
	char	*texture[TEX_COUNT];
	int		has_texture[TEX_COUNT];
	t_color	floor;
	t_color	ceiling;
	int		floor_set;
	int		ceiling_set;
}	t_config;

typedef struct s_map
{
	char	**grid;
	int		width;
	int		height;
}	t_map;

typedef struct s_player
{
	double	x;
	double	y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
}	t_player;

typedef struct s_input
{
	int	forward;
	int	backward;
	int	left;
	int	right;
	int	turn_left;
	int	turn_right;
}	t_input;

typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_img		frame;
	t_img		texture[TEX_COUNT];
	t_config	config;
	t_map		map;
	t_player	player;
	t_input		input;
	int			running;
}	t_game;

typedef struct s_ray
{
	double	dir_x;
	double	dir_y;
	int		map_x;
	int		map_y;
	double	side_dist_x;
	double	side_dist_y;
	double	delta_dist_x;
	double	delta_dist_y;
	double	step_x;
	double	step_y;
	int		side;
	double	perp_dist;
}	t_ray;

typedef struct s_column
{
	int	x;
	int	start;
	int	end;
	int	tex_x;
	int	line_height;
}	t_column;

/* ----------------------------- parsing ---------------------------------- */
int		parse_scene(t_game *game, const char *path);
void	free_config(t_config *cfg);
void	free_map(t_map *map);

/* ------------------------------ game ------------------------------------ */
int		init_game(t_game *game);
void	destroy_game(t_game *game);
int		game_loop(void *param);
int		handle_close(void *param);
int		handle_key_press(int keycode, void *param);
int		handle_key_release(int keycode, void *param);
void	update_game_state(t_game *game);

/* ------------------------------ render ---------------------------------- */
void	render_frame(t_game *game);
void	clear_frame(t_img *img, int color);
void	draw_pixel(t_img *img, int x, int y, int color);
int		get_color_from_tex(t_img *tex, int x, int y);
int		rgb_to_int(t_color color);
void	render_background(t_game *game);
void	ray_init(t_game *game, t_ray *ray, int x);
void	ray_set_step(t_game *game, t_ray *ray);
int		ray_step_until_hit(t_game *game, t_ray *ray);
int		select_texture_index(t_game *game, t_ray *ray);
void	compute_wall_limits(int line_height, int *start, int *end);
int		compute_tex_x(t_game *game, t_ray *ray, t_img *tex);
void	draw_texture_column(t_game *game, t_ray *ray, t_img *tex, t_column *col);

/* ------------------------------ utils ----------------------------------- */
int		ft_strlen(const char *s);
char	*ft_strdup(const char *s);
char	*ft_strndup(const char *s, size_t n);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_is_space(char c);
int		ft_is_digit(int c);
int		ft_atoi_strict(const char *str, int *out);
void	*ft_calloc(size_t count, size_t size);
void	*safe_malloc(size_t size);
void	ft_bzero(void *ptr, size_t size);
void	*ft_memcpy(void *dst, const void *src, size_t n);
int		print_error(const char *msg);
int		read_line(int fd, char **line);
char	**arr_push(char ***arr, int *len, char *line);
void	free_str_array(char **arr);
char	*trim_spaces(char *str);
int		is_map_line(const char *line);
int		is_all_space(const char *line);

/* --------------------------- parsing utils ------------------------------ */
int		parse_texture_line(t_config *cfg, const char *line);
int		parse_color_line(t_config *cfg, const char *line);
int		finalize_map(t_map *map, char **lines, int count);
int		validate_scene(t_game *game);
int		setup_player(t_game *game);
int		scene_config_ready(t_config *cfg);
int		scene_process_entry(t_game *game, char *raw);

#endif
