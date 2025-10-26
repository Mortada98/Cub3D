# Raycasting Algorithm - Mathematical Explanation

## Table of Contents
1. [Introduction](#introduction)
2. [Coordinate Systems](#coordinate-systems)
3. [Camera Mathematics](#camera-mathematics)
4. [Ray Direction Calculation](#ray-direction-calculation)
5. [DDA Algorithm](#dda-algorithm)
6. [Distance Calculation](#distance-calculation)
7. [Wall Height Projection](#wall-height-projection)
8. [Texture Mapping](#texture-mapping)
9. [Complete Example](#complete-example)

---

## Introduction

Raycasting is a rendering technique that creates a 3D perspective in a 2D map by casting rays from the player's position and calculating where they intersect with walls. This document explains the mathematics behind our Cub3D implementation.

**Core Principle:** For each vertical column on the screen, cast one ray into the scene and determine what to draw.

---

## Coordinate Systems

### World Space (Map Grid)
- The map is a 2D grid where each cell can be empty (`'0'`) or a wall (`'1'`)
- Coordinates are measured in grid units
- Origin `(0, 0)` is at the top-left corner
- X-axis goes right, Y-axis goes down

### Player Position
```
Player position: (x, y)
  - x, y are floating-point values (e.g., 5.5, 3.2)
  - Integer part: which grid cell the player is in
  - Fractional part: position within that cell
```

### Screen Space
```
Screen dimensions: WIN_WIDTH × WIN_HEIGHT (1280 × 720)
  - x ∈ [0, 1279]: horizontal screen coordinate
  - y ∈ [0, 719]: vertical screen coordinate
```

---

## Camera Mathematics

### Player Viewing Direction

The player has a **direction vector** `(dir_x, dir_y)` and a **camera plane** `(plane_x, plane_y)`.

```
Direction vector: (dir_x, dir_y)
  - Unit vector pointing where player is looking
  - Length = 1.0
  - Example: Looking east = (1, 0), north = (0, -1)

Camera plane: (plane_x, plane_y)
  - Perpendicular to direction vector
  - Defines field of view (FOV)
  - Length determines FOV width
  - Typical length: 0.66 (gives ~66° FOV)
```

**Mathematical Relationship:**
```
If direction = (dir_x, dir_y), then:
  Plane = (-dir_y, dir_x) × FOV_factor

Perpendicularity proof:
  dot_product = dir_x × plane_x + dir_y × plane_y
              = dir_x × (-dir_y × k) + dir_y × (dir_x × k)
              = -k × dir_x × dir_y + k × dir_y × dir_x
              = 0  ✓
```

### Field of View (FOV)

The FOV angle θ is determined by the ratio of plane length to direction length:

```
tan(θ/2) = |plane| / |direction|

For |plane| = 0.66 and |direction| = 1.0:
  θ/2 = atan(0.66) ≈ 33.4°
  θ ≈ 66.8°
```

**Visual Representation:**
```
                    Camera Plane
                        |
        Left edge       |        Right edge
             \          |          /
              \         |         /
               \    plane_vec    /
                \       |       /
                 \      |      /
                  \     |     /     θ = FOV
                   \    |    /
                    \   |   /
                     \  |  /
                      \ | /
                       \|/
                    Direction
                    (Player)
```

---

## Ray Direction Calculation

For each screen column `x` (from 0 to WIN_WIDTH-1), we calculate a ray direction.

### Step 1: Map Screen X to Camera Space

```c
camera_x = 2.0 × x / WIN_WIDTH - 1.0
```

**Mathematical Breakdown:**
```
x ∈ [0, WIN_WIDTH-1]
x / WIN_WIDTH ∈ [0, 1)
2.0 × (x / WIN_WIDTH) ∈ [0, 2)
2.0 × (x / WIN_WIDTH) - 1.0 ∈ [-1, 1)

Results:
  x = 0           → camera_x = -1.0  (left edge)
  x = WIN_WIDTH/2 → camera_x = 0.0   (center)
  x = WIN_WIDTH-1 → camera_x ≈ 1.0   (right edge)
```

### Step 2: Calculate Ray Direction Vector

The ray direction is a linear combination of the direction and plane vectors:

```c
ray_dir_x = dir_x + plane_x × camera_x
ray_dir_y = dir_y + plane_y × camera_x
```

**Mathematical Interpretation:**
```
ray_dir = direction + camera_plane × camera_x

This creates a "fan" of rays:
  - Center ray (camera_x = 0): points straight ahead (= direction)
  - Left ray (camera_x = -1): direction + plane × (-1) = direction - plane
  - Right ray (camera_x = +1): direction + plane × (+1) = direction + plane
```

**Example:**
```
Given:
  Player looking North: dir = (0, -1)
  Camera plane (perpendicular): plane = (0.66, 0)
  Screen column: x = 960 (3/4 to the right)

Calculate:
  camera_x = 2.0 × 960/1280 - 1.0 = 1.5 - 1.0 = 0.5
  
  ray_dir_x = 0 + 0.66 × 0.5 = 0.33
  ray_dir_y = -1 + 0 × 0.5 = -1.0
  
  ray_dir = (0.33, -1.0)  ← Points north-northeast
```

---

## DDA Algorithm

DDA (Digital Differential Analysis) steps through the grid one cell at a time along the ray.

### Delta Distance

**Definition:** The distance the ray travels to cross one grid line.

```
If ray direction = (ray_dir_x, ray_dir_y):

delta_dist_x = |1 / ray_dir_x|
delta_dist_y = |1 / ray_dir_y|
```

**Geometric Meaning:**
```
Consider a ray with direction (ray_dir_x, ray_dir_y).
To move 1 unit in X-direction, the ray must travel:
  distance = 1 / |ray_dir_x|

Why? By similar triangles:
  ray_length / 1 = 1 / |ray_dir_x|
  ray_length = 1 / |ray_dir_x|
```

**Visual Proof:**
```
Ray direction: (0.6, 0.8)
      
      ↑ Y
      |
    1 |     •  ← Ray travels this much...
      |    /|
  0.8 |   / |
      |  /  |
      | /   |
    0 |/____|___→ X
      0   0.6  1

To go 1 unit in X (0 to 1):
  Ray length = √(1² + (0.8/0.6)²) = √(1 + 1.78) = 1.67
  Or: 1 / |0.6| = 1.67  ✓
```

**Special Cases:**
```
If ray_dir_x = 0 (ray is vertical):
  delta_dist_x = ∞ (or 1e30 in code)
  Ray never crosses vertical grid lines

If ray_dir_y = 0 (ray is horizontal):
  delta_dist_y = ∞
  Ray never crosses horizontal grid lines
```

### Side Distance

**Definition:** Distance from current position to the next grid line.

```c
if (ray_dir_x < 0)  // Ray going left
{
    step_x = -1;
    side_dist_x = (player_x - map_x) × delta_dist_x;
}
else  // Ray going right
{
    step_x = 1;
    side_dist_x = (map_x + 1.0 - player_x) × delta_dist_x;
}
```

**Mathematical Explanation:**
```
Player at position (player_x, player_y) = (5.7, 3.2)
Current map cell: (map_x, map_y) = (5, 3)

Ray going RIGHT (ray_dir_x > 0):
  Next vertical grid line is at x = 6.0
  Distance in X: 6.0 - 5.7 = 0.3
  Ray distance: 0.3 × delta_dist_x
  
Ray going LEFT (ray_dir_x < 0):
  Next vertical grid line is at x = 5.0
  Distance in X: 5.7 - 5.0 = 0.7
  Ray distance: 0.7 × delta_dist_x
```

### DDA Stepping Algorithm

**Pseudocode:**
```
while (not hit wall):
    if (side_dist_x < side_dist_y):
        // Next grid line is vertical
        side_dist_x += delta_dist_x
        map_x += step_x
        side = 0  // Hit a vertical wall (North/South)
    else:
        // Next grid line is horizontal
        side_dist_y += delta_dist_y
        map_y += step_y
        side = 1  // Hit a horizontal wall (East/West)
    
    if (map[map_y][map_x] == '1'):
        break  // Wall hit!
```

**Why it works:**
- Always jump to the nearest grid line (minimum of side_dist_x and side_dist_y)
- Increment that distance by delta_dist for the next iteration
- Check if the new cell is a wall

**Example Trace:**
```
Initial state:
  Player: (5.7, 3.2)
  Direction: (0.8, 0.6) normalized
  delta_dist_x = 1.25, delta_dist_y = 1.67
  side_dist_x = 0.3 × 1.25 = 0.375
  side_dist_y = 0.8 × 1.67 = 1.336

Step 1: side_dist_x < side_dist_y
  → Jump to vertical line
  → map_x = 6, side_dist_x = 0.375 + 1.25 = 1.625
  → Check (6, 3): empty

Step 2: side_dist_y < side_dist_x
  → Jump to horizontal line
  → map_y = 4, side_dist_y = 1.336 + 1.67 = 3.006
  → Check (6, 4): empty

Step 3: side_dist_x < side_dist_y
  → Jump to vertical line
  → map_x = 7, side_dist_x = 1.625 + 1.25 = 2.875
  → Check (7, 4): wall! STOP
  → side = 0 (vertical wall hit)
```

---

## Distance Calculation

### Perpendicular Distance (Fisheye Correction)

**Problem:** Using Euclidean distance creates a fisheye effect.

```
Euclidean distance = √((wall_x - player_x)² + (wall_y - player_y)²)

Issues:
  - Rays at edges travel further
  - Creates curved walls (fisheye distortion)
```

**Solution:** Use perpendicular distance to camera plane.

```c
if (side == 0)  // Vertical wall
    perp_dist = side_dist_x - delta_dist_x;
else  // Horizontal wall
    perp_dist = side_dist_y - delta_dist_y;
```

**Mathematical Derivation:**

When we hit a wall, `side_dist_x` or `side_dist_y` contains the total distance traveled plus one delta_dist too much (from the last step). We subtract it to get the actual distance.

```
Visual representation:

         Camera Plane
              |
              |    Ray 1 (center)
              |   /
              |  /
              | /
              |/
         Player •
               |\
               | \
               |  \
               |   Ray 2 (edge)
               |
        
Euclidean:    
  dist(Ray 1) < dist(Ray 2)  ← Fisheye!

Perpendicular:
  perp_dist(Ray 1) = perp_dist(Ray 2)  ✓ Correct
```

**Projection Formula:**
```
The perpendicular distance is the projection of the ray onto the direction vector:

perp_dist = (wall_pos - player_pos) · direction / |direction|

Since |direction| = 1:
  perp_dist = (wall_pos - player_pos) · direction
```

**Why subtract delta_dist?**
```
After the last DDA step:
  side_dist contains the distance to the grid line AFTER the wall
  
We need the distance to the grid line WHERE the wall is:
  actual_dist = side_dist - delta_dist
```

---

## Wall Height Projection

### Perspective Projection

The height of the wall on screen depends on its distance from the camera.

```c
line_height = WIN_HEIGHT / perp_dist
```

**Mathematical Derivation:**

Using similar triangles in 3D projection:

```
         Screen
           |
           |  h (line_height)
           |  |
           | \|/
    Player •--•-------• Wall
           |    d     |
           |          | H (actual wall height)
           
By similar triangles:
  h / d_screen = H / d_wall
  
Assume:
  - Actual wall height H = 1 (one grid unit)
  - Screen distance d_screen = WIN_HEIGHT / 2 (for 90° FOV approximation)
  
Then:
  h = (WIN_HEIGHT / 2) × (1 / perp_dist)
  h = WIN_HEIGHT / (2 × perp_dist)
  
But for FOV ≈ 66°, the factor becomes 1:
  h = WIN_HEIGHT / perp_dist
```

**Example:**
```
Given:
  WIN_HEIGHT = 720
  perp_dist = 3.5
  
Calculate:
  line_height = 720 / 3.5 ≈ 206 pixels
```

### Drawing Boundaries

```c
start = -line_height / 2 + WIN_HEIGHT / 2
end = line_height / 2 + WIN_HEIGHT / 2
```

**Explanation:**
```
WIN_HEIGHT / 2 = vertical center of screen (y = 360 for 720p)

Wall extends:
  - Up by line_height/2
  - Down by line_height/2

Clamping:
  if (start < 0) start = 0;
  if (end >= WIN_HEIGHT) end = WIN_HEIGHT - 1;
```

**Visual:**
```
Screen (720 pixels high):
     0 ┌────────────┐
       │  Ceiling   │
       ├────────────┤ ← start = 360 - 103 = 257
       │████████████│
       │████████████│ Wall (line_height = 206)
       │████████████│
       ├────────────┤ ← end = 360 + 103 = 463
       │   Floor    │
   719 └────────────┘
```

---

## Texture Mapping

### Texture Selection

```c
if (side == 0)  // Vertical wall (North/South)
{
    if (ray_dir_x > 0)
        texture = WEST;   // Looking east, see west-facing wall
    else
        texture = EAST;   // Looking west, see east-facing wall
}
else  // Horizontal wall (East/West)
{
    if (ray_dir_y > 0)
        texture = SOUTH;  // Looking south
    else
        texture = NORTH;  // Looking north
}
```

**Logic:**
- Vertical walls (side = 0) run North-South
- Horizontal walls (side = 1) run East-West
- Texture depends on which direction you're looking from

### Texture X-Coordinate

**Goal:** Find which vertical strip of the texture to draw.

```c
// Calculate exact hit position on wall
if (side == 0)  // Vertical wall
    wall_x = player_y + perp_dist × ray_dir_y;
else  // Horizontal wall
    wall_x = player_x + perp_dist × ray_dir_x;

// Get fractional part (0.0 to 1.0)
wall_x = wall_x - floor(wall_x);

// Map to texture width
tex_x = (int)(wall_x × tex_width);
```

**Mathematical Explanation:**

For a vertical wall hit:
```
Ray equation: (x, y) = (player_x, player_y) + t × (ray_dir_x, ray_dir_y)

At wall intersection:
  x_wall is integer (grid line)
  y_wall = player_y + perp_dist × ray_dir_y
  
The fractional part of y_wall tells us where on the wall we hit:
  wall_x = fract(y_wall) ∈ [0, 1)
  
Map to texture:
  tex_x = wall_x × tex_width
  
Example:
  wall_x = 0.73, tex_width = 64
  tex_x = 0.73 × 64 = 46 (use column 46 of texture)
```

**Texture Flipping:**
```c
if (side == 0 && ray_dir_x > 0)
    tex_x = tex_width - tex_x - 1;
if (side == 1 && ray_dir_y < 0)
    tex_x = tex_width - tex_x - 1;
```

This prevents texture mirroring on opposite-facing walls.

### Texture Y-Coordinate (Vertical Mapping)

```c
step = (double)tex_height / line_height;
tex_pos = (start - WIN_HEIGHT/2 + line_height/2) × step;

for (y = start; y <= end; y++)
{
    tex_y = (int)tex_pos;
    color = texture[tex_y][tex_x];
    draw_pixel(x, y, color);
    tex_pos += step;
}
```

**Mathematical Breakdown:**

```
Problem: Map screen pixels [start, end] to texture pixels [0, tex_height-1]

Step size (texture pixels per screen pixel):
  step = tex_height / line_height

Starting position in texture:
  The first pixel we draw (y = start) should map to the top of the visible wall
  
  Offset from screen center: start - WIN_HEIGHT/2
  Offset from wall center: start - WIN_HEIGHT/2 + line_height/2
  Texture position: offset × step

Example:
  tex_height = 64, line_height = 200
  start = 260, end = 460
  
  step = 64 / 200 = 0.32
  
  At y = 260:
    offset = 260 - 360 + 100 = 0
    tex_pos = 0 × 0.32 = 0  ← Top of texture
  
  At y = 360 (screen center):
    offset = 360 - 360 + 100 = 100
    tex_pos = 100 × 0.32 = 32  ← Middle of texture
  
  At y = 460:
    offset = 460 - 360 + 100 = 200
    tex_pos = 200 × 0.32 = 64  ← Bottom of texture
```

### Shading

```c
if (side == 1)  // Horizontal walls
    color = (color >> 1) & 0x7F7F7F;
```

**Bit manipulation explanation:**
```
Original color: RRGGBB (24-bit RGB)
  R = bits 16-23
  G = bits 8-15
  B = bits 0-7

Right shift by 1: color >> 1
  Divides each component by 2
  
Mask with 0x7F7F7F:
  0x7F = 0b01111111 (ensures bit 7 is 0)
  Prevents overflow/wrapping

Example:
  color = 0xFF8040 (R=255, G=128, B=64)
  color >> 1 = 0x7FC020 (R=127, G=64, B=32)
  & 0x7F7F7F = 0x7F4020 (R=127, G=64, B=32)  ← 50% darker
```

---

## Complete Example

Let's trace a complete frame for one column.

### Setup
```
Player:
  Position: (5.5, 3.2)
  Direction: (1, 0)  ← Looking East
  Plane: (0, 0.66)   ← FOV ≈ 66°

Screen:
  x = 640 (center column)
  WIN_WIDTH = 1280
  WIN_HEIGHT = 720

Map (5x5 grid):
  1 1 1 1 1
  1 0 0 0 1
  1 0 0 0 1
  1 0 0 0 1
  1 1 1 1 1
```

### Step 1: Ray Direction
```
camera_x = 2.0 × 640/1280 - 1.0 = 1.0 - 1.0 = 0.0

ray_dir_x = 1 + 0 × 0.0 = 1.0
ray_dir_y = 0 + 0.66 × 0.0 = 0.0

Ray direction: (1.0, 0.0)  ← Straight east
```

### Step 2: Initialize DDA
```
map_x = (int)5.5 = 5
map_y = (int)3.2 = 3

delta_dist_x = |1/1.0| = 1.0
delta_dist_y = |1/0.0| = ∞  ← Ray is horizontal

step_x = 1 (going right)
step_y = 0 (not moving in Y)

side_dist_x = (6.0 - 5.5) × 1.0 = 0.5
side_dist_y = ∞
```

### Step 3: DDA Loop
```
Iteration 1:
  side_dist_x (0.5) < side_dist_y (∞) ✓
  → side_dist_x = 0.5 + 1.0 = 1.5
  → map_x = 5 + 1 = 6
  → side = 0
  → Check map[3][6]: Out of bounds or wall? → '1' WALL HIT!

perp_dist = 1.5 - 1.0 = 0.5
```

### Step 4: Wall Height
```
line_height = 720 / 0.5 = 1440 pixels

start = -1440/2 + 720/2 = -720 + 360 = -360
→ Clamped to 0

end = 1440/2 + 720/2 = 720 + 360 = 1080
→ Clamped to 719

Draw from y=0 to y=719 (full screen!)
```

### Step 5: Texture Mapping
```
Texture selection:
  side = 0 (vertical wall)
  ray_dir_x = 1.0 > 0
  → Use WEST texture

Texture X:
  wall_x = player_y + perp_dist × ray_dir_y
        = 3.2 + 0.5 × 0.0 = 3.2
  wall_x = 3.2 - floor(3.2) = 0.2
  
  tex_x = 0.2 × 64 = 12  ← Use column 12

Texture Y:
  step = 64 / 1440 ≈ 0.044
  
  For each screen pixel y ∈ [0, 719]:
    tex_y = (int)(y × 0.044)
    Draw texture_pixel[tex_y][12]
```

### Step 6: Draw
```
For y = 0 to 719:
  color = texture[WEST][tex_y][12]
  frame_buffer[640][y] = color
```

---

## Performance Considerations

### Time Complexity
```
For each frame:
  - Loop through WIN_WIDTH columns: O(WIN_WIDTH)
  - For each column, DDA steps until wall hit: O(max_ray_distance)
  - Draw wall: O(WIN_HEIGHT)
  
Total: O(WIN_WIDTH × (ray_steps + WIN_HEIGHT))
      ≈ O(1280 × (10 + 720))
      ≈ O(934,400) operations per frame
      
At 60 FPS: ~56 million operations/second
```

### Optimization Techniques
1. **Integer-only DDA** - Use fixed-point arithmetic instead of floats
2. **Lookup tables** - Pre-compute delta_dist values
3. **SIMD** - Process multiple rays in parallel
4. **Dirty rectangles** - Only update changed portions

---

## Common Pitfalls

### 1. Fisheye Effect
**Problem:** Using Euclidean distance instead of perpendicular distance.
```
❌ dist = sqrt((x2-x1)² + (y2-y1)²)
✓ perp_dist = side_dist - delta_dist
```

### 2. Division by Zero
**Problem:** Ray direction components can be zero.
```
❌ delta_dist_x = 1.0 / ray_dir_x
✓ delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1.0 / ray_dir_x)
```

### 3. Texture Mirroring
**Problem:** Opposite walls show mirrored textures.
```
✓ Flip texture X-coordinate for certain directions:
  if (side == 0 && ray_dir_x > 0)
      tex_x = tex_width - tex_x - 1;
```

### 4. Off-by-One Errors
**Problem:** Array bounds in texture sampling.
```
❌ tex_y = (int)(tex_pos)  // Can equal tex_height
✓ tex_y = (int)(tex_pos) % tex_height
  or clamp: if (tex_y >= tex_height) tex_y = tex_height - 1
```

---

## References

- Lode's Computer Graphics Tutorial: https://lodev.org/cgtutor/raycasting.html
- Wolfenstein 3D Source Code Analysis
- "Digital Differential Analyzer" (DDA) line algorithm
- Perspective projection in computer graphics

---

**Author:** Cub3D Project  
**Date:** October 24, 2025  
**Version:** 1.0
