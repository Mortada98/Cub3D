# Raycasting — Code + Visual Guide

This guide blends your actual code with visuals and math so you can see exactly how each piece works together.

- Code references: `src/render/raycast.c`, `src/render/ray_setup.c`, `src/render/ray_texture.c`, `includes/cub3d.h`
- Screen: WIN_WIDTH × WIN_HEIGHT = 1280 × 720
- Textures: TEX_NO, TEX_SO, TEX_WE, TEX_EA

---

## 1) Big picture: per-column raycast

For each screen column x, we cast a ray, find the wall, compute its height, pick the texture, and draw one vertical slice.

```
      SCREEN (1280 columns)
┌─┬─┬─┬─┬─┬─┬─┬─┬─⋯─────────────────────────┐
│ │ │ │ │ │ │ │ │                           │
│ │ │ │ │ │ │ │ │  one ray per column  →   │
│ │ │ │ │ │ │ │ │                           │
└─┴─┴─┴─┴─┴─┴─┴─┴─⋯─────────────────────────┘
 0                 640                     1279
```

Code: `render_frame` and `render_column` in `raycast.c`

```c
void render_frame(t_game *game)
{
    int x;

    render_background(game); // floor + ceiling colors
    x = 0;
    while (x < WIN_WIDTH)
    {
        render_column(game, x);
        x++;
    }
    mlx_put_image_to_window(game->mlx, game->win, game->frame.ptr, 0, 0);
}

static void render_column(t_game *game, int x)
{
    t_ray ray;

    ray_init(game, &ray, x);           // 1) ray direction for column x
    ray_set_step(game, &ray);          // 2) precompute stepping distances
    if (ray_step_until_hit(game, &ray) < 0)
        return;                        // 3) DDA march to wall
    if (ray.perp_dist > 0)
        draw_wall(game, x, &ray);      // 4) project + texture one column
}
```

---

## 2) Ray setup — camera → ray direction

We map the screen column to camera space `camera_x ∈ [-1, 1]`, then build a direction from the player’s facing vector and the camera plane.

Math
- Camera mapping: $camera\_x = 2\,x/\text{WIN\_WIDTH} - 1$
- Ray direction: $\vec{r} = \vec{dir} + camera\_x\,\vec{plane}$

Visual
```
                         Camera Plane (perpendicular to dir)
                                |
                     left (-)   |   (+) right
                                |
                           ╱    |    ╲
                          ╱     |     ╲
                         ╱      |      ╲
                        ╱       |       ╲
                       •════════•════════•   ← rays across FOV
                             Player→dir
```

Code: `ray_init` in `ray_setup.c`

```c
void ray_init(t_game *game, t_ray *ray, int x)
{
    double camera_x = 2.0 * x / (double)WIN_WIDTH - 1.0;

    ray->dir_x = game->player.dir_x + game->player.plane_x * camera_x;
    ray->dir_y = game->player.dir_y + game->player.plane_y * camera_x;

    ray->map_x = (int)game->player.x;
    ray->map_y = (int)game->player.y;

    // delta distance = |1 / ray_dir_component|, guard for 0
    ray->delta_dist_x = (ray->dir_x == 0) ? 1e30 : fabs(1.0 / ray->dir_x);
    ray->delta_dist_y = (ray->dir_y == 0) ? 1e30 : fabs(1.0 / ray->dir_y);
}
```

Intuition
- `camera_x = -1` → far left ray; `0` → center; `+1` → far right
- `delta_dist_x/y` = how far along the ray to cross one gridline in X/Y

---

## 3) First step to gridlines — side distances and steps

We determine which way we move in the grid (`step_x/y`) and how far to the first vertical/horizontal gridline from the player (`side_dist_x/y`).

Visual
```
Player (3.7, 2.3)

   2.0     3.0     4.0
    ├───────┼───────┼──→ X
        P • │
           ││  next X line at 4.0:  dx = 4.0 - 3.7 = 0.3
           │└─ next Y line at 3.0:  dy = 3.0 - 2.3 = 0.7

side_dist_x = dx * delta_dist_x
side_dist_y = dy * delta_dist_y
```

Code: `ray_set_step` in `ray_setup.c`

```c
void ray_set_step(t_game *game, t_ray *ray)
{
    if (ray->dir_x < 0) {
        ray->step_x = -1;
        ray->side_dist_x = (game->player.x - ray->map_x) * ray->delta_dist_x;
    } else {
        ray->step_x = 1;
        ray->side_dist_x = (ray->map_x + 1.0 - game->player.x) * ray->delta_dist_x;
    }
    if (ray->dir_y < 0) {
        ray->step_y = -1;
        ray->side_dist_y = (game->player.y - ray->map_y) * ray->delta_dist_y;
    } else {
        ray->step_y = 1;
        ray->side_dist_y = (ray->map_y + 1.0 - game->player.y) * ray->delta_dist_y;
    }
}
```

---

## 4) DDA — walk the grid until a wall

At each iteration, advance to whichever boundary is nearer: if `side_dist_x < side_dist_y` cross a vertical gridline (move in X), else cross a horizontal gridline (move in Y). Stop on a `'1'` cell.

Top-down visual
```
Y↓  X→   3   4   5   6
     ┌───┬───┬───┬───┐
 3   │ P │ → │ → │ ■ │  P = start cell, arrows = DDA steps,
     └───┴───┴───┴───┘  ■ = wall hit at (6,3)
```

Code: `ray_step_until_hit` in `ray_setup.c`

```c
static int ray_advance_cell(t_game *game, t_ray *ray)
{
    if (ray->side_dist_x < ray->side_dist_y) {
        ray->side_dist_x += ray->delta_dist_x; // jump to next vertical line
        ray->map_x += ray->step_x;
        ray->side = 0; // vertical wall (NS)
    } else {
        ray->side_dist_y += ray->delta_dist_y; // jump to next horizontal line
        ray->map_y += ray->step_y;
        ray->side = 1; // horizontal wall (EW)
    }
    if (ray->map_y < 0 || ray->map_y >= game->map.height
     || ray->map_x < 0 || ray->map_x >= game->map.width)
        return (-1);
    if (game->map.grid[ray->map_y][ray->map_x] == '1')
        return (1);
    return (0);
}

int ray_step_until_hit(t_game *game, t_ray *ray)
{
    int status = ray_advance_cell(game, ray);
    while (status == 0)
        status = ray_advance_cell(game, ray);
    if (status < 0)
        return (-1);

    // perpendicular distance to wall plane (fisheye fix)
    ray->perp_dist = (ray->side == 0)
        ? ray->side_dist_x - ray->delta_dist_x
        : ray->side_dist_y - ray->delta_dist_y;
    return (0);
}
```

Why subtract one `delta_dist`?
- The last jump advanced to the next gridline; subtract the step to get distance to the wall plane.

---

## 5) Projection — screen height from distance

Wall height on screen is inversely proportional to perpendicular distance.

Math
- $line\_height = \dfrac{\text{WIN\_HEIGHT}}{perp\_dist}$
- $start = -\tfrac{line\_height}{2} + \tfrac{\text{WIN\_HEIGHT}}{2}$, $\; end = +\tfrac{line\_height}{2} + \tfrac{\text{WIN\_HEIGHT}}{2}$, clamped

Visual
```
Close (dist=2) → tall          Far (dist=6) → short

  ┌────┐                        ┌────┐
  │    │ Ceiling                │████│ Ceiling
  ├────┤                        ├────┤
  │████│ Wall 360px             │    │
  │████│                         │    │ Wall 120px
  ├────┤                        ├────┤
  │    │ Floor                  │████│ Floor
  └────┘                        └────┘
```

Code: `compute_wall_limits` in `ray_texture.c`

```c
void compute_wall_limits(int lh, int *start, int *end)
{
    *start = -lh / 2 + WIN_HEIGHT / 2;
    if (*start < 0) *start = 0;
    *end = lh / 2 + WIN_HEIGHT / 2;
    if (*end >= WIN_HEIGHT) *end = WIN_HEIGHT - 1;
}
```

---

## 6) Texture face — pick NO/SO/WE/EA

We decide which texture to use from the hit side (vertical/horizontal) and the sign of the ray direction.

Compass visual
```
        NORTH (TEX_NO)
              ↑
    WEST ←────•────→ EAST
    (TEX_WE)       (TEX_EA)
              ↓
        SOUTH (TEX_SO)
```

Code: `select_texture_index` in `ray_texture.c`

```c
int select_texture_index(t_game *game, t_ray *ray)
{
    (void)game;
    if (ray->side == 0) {
        if (ray->dir_x > 0) return (TEX_WE);
        return (TEX_EA);
    }
    if (ray->dir_y > 0) return (TEX_SO);
    return (TEX_NO);
}
```

---

## 7) Texture coordinates — sample the image

Horizontal coordinate within the wall (fractional position along the hit) maps to texture column `tex_x`. Vertical sampling uses a constant step.

Math
- Hit coordinate along wall:  
  if side==0: $wall\_x = y\_{player} + perp\_dist \cdot dir\_y$  
  else: $wall\_x = x\_{player} + perp\_dist \cdot dir\_x$  
  $wall\_x \leftarrow wall\_x - \lfloor wall\_x \rfloor$ (fraction)
- Texture column: $tex\_x = \lfloor wall\_x \cdot tex\_w \rfloor$
- Vertical step: $step = tex\_h / line\_height$

Code: `compute_tex_x` and `draw_texture_column` in `ray_texture.c`

```c
int compute_tex_x(t_game *game, t_ray *ray, t_img *tex)
{
    double wall_x;
    int    tex_x;

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

void draw_texture_column(t_game *game, t_ray *ray, t_img *tex, t_column *col)
{
    double step   = (double)tex->height / col->line_height;
    double tex_pos = (col->start - WIN_HEIGHT / 2 + col->line_height / 2) * step;
    int    y = col->start;

    while (y <= col->end)
    {
        int color = get_color_from_tex(tex, col->tex_x, (int)tex_pos);
        if (ray->side == 1) // darken horizontal faces
            color = (color >> 1) & 0x7F7F7F;
        draw_pixel(&game->frame, col->x, y, color);
        tex_pos += step;
        y++;
    }
}
```

Visual — mapping screen to texture
```
Screen wall (200px)     Texture (64px)

start
  │  █               0  ┌████████──┐
  │  █                   │        ┆ │
  │  █                   │        ┆ │
  │  █                   │   tex_x ┆ │ ← same column
  │  █                   │        ┆ │
end│  █              64 └──────────┘

step = 64 / 200 = 0.32;  tex_y accumulates by step each screen pixel
```

---

## 8) Putting it together — draw one column

Code: `draw_wall` in `raycast.c`

```c
static void draw_wall(t_game *game, int x, t_ray *ray)
{
    t_img    *tex;
    t_column  col;

    if (ray->perp_dist <= 0) ray->perp_dist = 0.0001;
    col.line_height = (int)(WIN_HEIGHT / ray->perp_dist);
    compute_wall_limits(col.line_height, &col.start, &col.end);

    tex = &game->texture[select_texture_index(game, ray)];
    col.tex_x = compute_tex_x(game, ray, tex);
    if (col.tex_x < 0) col.tex_x = 0;
    if (col.tex_x >= tex->width) col.tex_x = tex->width - 1;

    col.x = x;
    draw_texture_column(game, ray, tex, &col);
}
```

Flow summary
1) Clamp tiny distances, compute wall height and draw window
2) Pick texture face, compute `tex_x` column
3) Sample texture vertically with constant step and draw

---

## 9) Data structures (in `includes/cub3d.h`)

```c
typedef struct s_ray {
    double dir_x, dir_y;
    int    map_x, map_y;
    double side_dist_x, side_dist_y;
    double delta_dist_x, delta_dist_y;
    double step_x, step_y;
    int    side;      // 0 = vertical, 1 = horizontal
    double perp_dist; // perpendicular distance to wall
} t_ray;

typedef struct s_column {
    int x;
    int start, end;
    int tex_x;
    int line_height;
} t_column;
```

---

## 10) Pitfalls and safeguards

- Division by zero: when `ray_dir_x` or `ray_dir_y` is 0, set `delta_dist_* = 1e30`
- Fisheye fix: always use perpendicular distance (not raw ray length)
- Distance clamp: `if (perp_dist <= 0) perp_dist = 0.0001` to avoid infinite height
- Screen clamp: start/end clamped to `[0, WIN_HEIGHT-1]`
- Texture clamp: `tex_x` clamped to `[0, width-1]`; flip to avoid mirroring
- Simple shading: darken horizontal faces for depth cue `(color >> 1) & 0x7F7F7F`

Quick formulas
```
camera_x   = 2*(x/WIN_WIDTH) - 1
ray_dir    = dir + camera_x * plane
delta_dist = |1 / ray_dir_component|
perp_dist  = (hit side_dist) - (delta_dist of that side)
line_height= WIN_HEIGHT / perp_dist
tex_x      = fract(wall_hit_pos) * tex_width
step       = tex_height / line_height
```

---

With these visuals, equations, and the exact code paths, you can trace any column from screen space → raycast → DDA → projection → texture sampling, line by line.