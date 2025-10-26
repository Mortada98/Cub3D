# Raycasting - Visual Explanation with Diagrams

This document provides visual explanations of the raycasting algorithm using ASCII art and diagrams.

---

## 🎯 The Big Picture: Top-Down View

```
                        MAP (Top-Down View)
    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
    │  1  │  1  │  1  │  1  │  1  │  1  │  1  │  1  │  Y=0
    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    │  1  │  0  │  0  │  0  │  0  │  0  │  0  │  1  │  Y=1
    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    │  1  │  0  │  0  │  0  │  0  │  0  │  0  │  1  │  Y=2
    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    │  1  │  0  │  0  │  P  │  0  │  0  │  0  │  1  │  Y=3
    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    │  1  │  0  │  0  │  0  │  0  │  0  │  0  │  1  │  Y=4
    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    │  1  │  1  │  1  │  1  │  1  │  1  │  1  │  1  │  Y=5
    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
     X=0   X=1   X=2   X=3   X=4   X=5   X=6   X=7

Legend:
  1 = Wall (solid)
  0 = Empty space (walkable)
  P = Player position
```

---

## 📷 Camera and Field of View

### Camera Setup (Looking East)

```
                       Camera Plane (perpendicular)
                              |
                              |
                     Left (-) | (+) Right
                              |
                    ╱         |         ╲
                   ╱          |          ╲
                  ╱           |           ╲
                 ╱            |            ╲        FOV ≈ 66°
                ╱             |             ╲
               ╱              |              ╲
              ╱               |               ╲
             ╱                |                ╲
            ╱                 |                 ╲
           ╱                  |                  ╲
          ╱                   |                   ╲
         •════════════════════•════════════════════•
      Ray -1            Direction (0°)          Ray +1
    (Left edge)        (Straight ahead)      (Right edge)
                            👤
                          PLAYER
```

### Direction and Plane Vectors

```
Example 1: Looking EAST (→)
┌─────────────────────────────────────┐
│  Direction = (1.0, 0.0)   →        │
│  Plane = (0.0, 0.66)      ↑        │
│                           |         │
│              Left edge    |         │
│                    ╲      |         │
│                     ╲     |         │
│                      ╲    |         │
│                       ╲   |         │
│                        ╲  |         │
│                         ╲ |         │
│                          ╲|         │
│         ═════════════════•═════→    │
│                        Player       │
│                          ╱|         │
│                         ╱ |         │
│                        ╱  |         │
│                       ╱   |         │
│                      ╱    |         │
│                     ╱     |         │
│                    ╱      |         │
│              Right edge   |         │
└─────────────────────────────────────┘

Example 2: Looking NORTH (↑)
┌─────────────────────────────────────┐
│  Direction = (0.0, -1.0)  ↑        │
│  Plane = (0.66, 0.0)    ←─┼─→      │
│                            |         │
│                            |         │
│            Left ╱          |          ╲ Right
│                ╱           |           ╲
│               ╱            |            ╲
│              ╱             |             ╲
│             ╱              ↑              ╲
│            ╱               |               ╲
│           •═══════════════👤═══════════════•
│                         Player             │
└────────────────────────────────────────────┘
```

---

## 🌟 Ray Casting Process

### Step 1: Camera X Mapping

```
Screen columns mapped to camera space [-1.0, +1.0]

Screen:          Camera X:
  x=0        →   -1.0  (Left edge)
  x=320      →   -0.5
  x=640      →    0.0  (Center)
  x=960      →   +0.5
  x=1279     →   +1.0  (Right edge)

Formula: camera_x = 2 × (x / 1280) - 1


         SCREEN (1280 pixels wide)
    ┌───┬───┬───┬───┬───┬───┬───┬───┐
    │   │   │   │ • │   │   │   │   │  ← Each column = one ray
    │   │   │   │ • │   │   │   │   │
    │   │   │   │ • │   │   │   │   │
    │   │   │   │ • │   │   │   │   │
    │   │   │   │ • │   │   │   │   │
    └───┴───┴───┴───┴───┴───┴───┴───┘
     0               640            1279
    -1.0             0.0            +1.0
    Left          Center           Right
```

### Step 2: Ray Direction Calculation

```
ray_dir = direction + plane × camera_x

Visual example (Looking East):
  Direction: (1, 0)  →
  Plane: (0, 0.66)   ↑

For center ray (camera_x = 0):
  ray_dir = (1, 0) + (0, 0.66) × 0
          = (1, 0)  ← Straight ahead

For right ray (camera_x = +1):
  ray_dir = (1, 0) + (0, 0.66) × 1
          = (1, 0.66)  ← Northeast

For left ray (camera_x = -1):
  ray_dir = (1, 0) + (0, 0.66) × (-1)
          = (1, -0.66)  ← Southeast


    ╱ Left ray (1, -0.66)
   ╱
  ╱
 ╱
•════════→ Center ray (1, 0)
 ╲
  ╲
   ╲
    ╲ Right ray (1, 0.66)
   
   Player
```

---

## 🔲 DDA Algorithm Visualization

### Grid Traversal

```
DDA steps through grid cells along the ray path

Player at (3.5, 3.2), Ray direction: (0.8, 0.6)

    Y
    ↓
    0   1   2   3   4   5   6   7   X →
  ┌───┬───┬───┬───┬───┬───┬───┬───┐
0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
2 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
3 │ 1 │ 0 │ 0 │ P·│···│···│··•│ 1 │  ← Ray hits wall at (6,3)
  ├───┼───┼───┼───┼───┼───┼───┼───┤     Path: (3,3)→(4,3)→(5,3)→(6,3)
4 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
5 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │
  └───┴───┴───┴───┴───┴───┴───┴───┘

Step-by-step:
  Start: (3, 3) - Player's cell
  Step 1: Jump to x=4 → Check (4, 3): Empty ✗
  Step 2: Jump to x=5 → Check (5, 3): Empty ✗
  Step 3: Jump to x=6 → Check (6, 3): Wall! ✓ STOP
```

### Delta Distance Concept

```
How far the ray travels to cross ONE grid square

Ray direction: (0.6, 0.8)

     ┌────┬────┬────┐
     │    │    │    │
     │    │  •─┼────┼──→ To cross 1 unit in X,
     │   ╱│   │    │    ray travels: 1/0.6 = 1.67
     │  ╱ │   │    │
     │ ╱  │   │    │
     │╱   │   │    │
     •────┴───┴────┘
     ↑
   Start    1.0 →

delta_dist_x = 1.67  (distance ray travels per X grid)
delta_dist_y = 1.25  (distance ray travels per Y grid)
```

### Side Distance - First Step

```
Player at (3.7, 2.3), Ray going right and down

     2.0     3.0     4.0     5.0
      ├───────┼───────┼───────┤  
  2.0 │       │       │       │
      │       │  P    │       │  Player at (3.7, 2.3)
      │       │  •    │       │
  3.0 ├───────┼──│────┼───────┤
      │       │  │    │       │
      │       │  ↓    │       │
      
Distance to next grid lines:
  - Next X line: 4.0 - 3.7 = 0.3
  - Next Y line: 3.0 - 2.3 = 0.7

If ray direction = (0.8, 0.6):
  side_dist_x = 0.3 × (1/0.8) = 0.375
  side_dist_y = 0.7 × (1/0.6) = 1.167

Choose minimum: 0.375 < 1.167
→ Jump to vertical line first (X = 4.0)
```

---

## 📐 Perpendicular Distance (Fisheye Correction)

### The Problem: Euclidean Distance

```
❌ WRONG: Using actual ray length causes distortion

                Camera Plane
                     |
        Ray A        |        Ray B
         (short)     |        (long)
            ╲        |        ╱
             ╲       |       ╱
              ╲      |      ╱
               ╲     |     ╱
                ╲    |    ╱
                 ╲   |   ╱
                  ╲  |  ╱
                   ╲ | ╱
                    ╲|╱
                     P
                  Player

Ray A length = 5.0  → Wall appears tall
Ray B length = 7.0  → Wall appears short

Result: Curved walls (fisheye effect) 😢
```

### The Solution: Perpendicular Distance

```
✓ CORRECT: Project onto camera plane direction

                Camera Plane
                     |
        Ray A        |        Ray B
                     |
            ╲        |        ╱
             ╲       |       ╱
              ╲      |      ╱
         ······╲·····|·····╱······  Perpendicular line
         ↑     ╲    |    ╱      ↑
         5.0    ╲   |   ╱       5.0
         ↑       ╲  |  ╱        ↑
        Same!     ╲ | ╱        Same!
                   ╲|╱
                    P
                  Player

perp_dist(A) = 5.0
perp_dist(B) = 5.0  ← Same distance!

Result: Straight walls 😊
```

### Mathematical Projection

```
             Wall
              |
              |  Actual ray
              | ╱
              |╱ 
    ══════════•══════════  Camera plane
              |╲
              | ╲ perp_dist
              |  ╲
              |   ╲
              |    👤 Player
              
perp_dist = projection of ray onto direction vector
```

---

## 🎨 Screen Projection

### Wall Height Calculation

```
Perspective projection: Farther = Smaller

Close wall (dist = 2):               Far wall (dist = 6):
    line_height = 720/2 = 360           line_height = 720/6 = 120

Screen:                             Screen:
┌────┐                              ┌────┐
│    │ Ceiling                      │████│ Ceiling
│    │                              │████│
├────┤                              ├────┤
│████│                              │    │
│████│                              │    │
│████│ Wall (360 px)                │    │ Wall (120 px)
│████│                              │    │
│████│                              │    │
│████│                              ├────┤
├────┤                              │████│ Floor
│    │ Floor                        │████│
│    │                              │████│
└────┘                              └────┘
```

### Drawing Boundaries

```
line_height = 200
WIN_HEIGHT = 720
center = 360

start = center - line_height/2 = 360 - 100 = 260
end = center + line_height/2 = 360 + 100 = 460

    0 ┌─────────┐
      │         │
      │ Ceiling │  (solid color)
      │         │
  260 ├─────────┤ ← start
      │▓▓▓▓▓▓▓▓▓│
      │▓▓▓▓▓▓▓▓▓│
      │▓▓▓▓▓▓▓▓▓│
      │▓▓▓▓▓▓▓▓▓│  Wall (textured)
  360 │▓▓▓▓▓▓▓▓▓│ ← center
      │▓▓▓▓▓▓▓▓▓│
      │▓▓▓▓▓▓▓▓▓│
      │▓▓▓▓▓▓▓▓▓│
      │▓▓▓▓▓▓▓▓▓│
  460 ├─────────┤ ← end
      │         │
      │  Floor  │  (solid color)
      │         │
  719 └─────────┘
```

---

## 🖼️ Texture Mapping

### Texture Selection by Direction

```
        NORTH (TEX_NO)
              ↑
              |
              |
    WEST ←────•────→ EAST
    (TEX_WE)  |   (TEX_EA)
              |
              |
              ↓
        SOUTH (TEX_SO)

If player looking East hits vertical wall:
  - ray_dir_x > 0: Show WEST texture
  - ray_dir_x < 0: Show EAST texture

If player looking North hits horizontal wall:
  - ray_dir_y < 0: Show NORTH texture
  - ray_dir_y > 0: Show SOUTH texture
```

### Texture X-Coordinate

```
Finding which column of texture to use

Wall (one grid cell):
┌─────────────────┐
│ • Hit point     │  Hit at position 0.73
│   (wall_x=0.73) │  
│                 │
│                 │
└─────────────────┘
0.0              1.0

Texture (64 pixels wide):
├┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┬┤
0         10        20        30        40   ↑    50        60
                                           Column 46

tex_x = 0.73 × 64 = 46.72 → 46

Use column 46 of the texture!
```

### Texture Y-Coordinate Mapping

```
Screen wall (200 pixels):   Texture (64 pixels):
                           
  260 ┌─────────┐             0 ┌─────────┐
      │▓        │ ──────────────│█████████│  Top
      │▓▓       │              4│█████████│
      │▓▓▓      │              8│█████████│
      │▓▓▓▓     │             12│█████████│
      │▓▓▓▓▓    │             16│█████████│
  360 │▓▓▓▓▓▓   │ ───────────32│█████████│  Middle
      │▓▓▓▓▓▓▓  │             48│█████████│
      │▓▓▓▓▓▓▓▓ │             56│█████████│
      │▓▓▓▓▓▓▓▓▓│             60│█████████│
  460 └─────────┘            64 └─────────┘  Bottom

step = 64 / 200 = 0.32 texture pixels per screen pixel

For screen y=260: tex_y = 0
For screen y=360: tex_y = 32  (middle)
For screen y=460: tex_y = 64  (bottom)
```

### Complete Column Rendering

```
One screen column (x = 640):

  ┌─┐
  │ │ y=0    → Ceiling color (from config)
  │ │ y=100  → Ceiling color
  │ │ y=200  → Ceiling color
  ├─┤ y=260  → Wall start (tex_y=0, tex_x=46)
  │█│ y=261  → Wall pixel (tex_y=0, tex_x=46)
  │█│ y=262  → Wall pixel (tex_y=1, tex_x=46)
  │█│ y=263  → Wall pixel (tex_y=1, tex_x=46)
  │█│  ...   → ...
  │█│ y=360  → Wall pixel (tex_y=32, tex_x=46)
  │█│  ...   → ...
  │█│ y=459  → Wall pixel (tex_y=63, tex_x=46)
  ├─┤ y=460  → Wall end
  │ │ y=461  → Floor color (from config)
  │ │ y=600  → Floor color
  │ │ y=719  → Floor color
  └─┘
```

---

## 🔄 Complete Frame Rendering

### Full Process Overview

```
┌─────────────────────────────────────────────────────────┐
│                 RENDERING ONE FRAME                     │
└─────────────────────────────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  For x = 0 to 1279 (each column) │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  1. Calculate ray direction     │
        │     ray_dir = dir + plane × cx  │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  2. Initialize DDA              │
        │     - delta_dist                │
        │     - side_dist                 │
        │     - step direction            │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  3. DDA Loop                    │
        │     Step through grid until hit │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  4. Calculate perpendicular     │
        │     distance (fisheye fix)      │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  5. Calculate wall height       │
        │     height = WIN_HEIGHT / dist  │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  6. Select texture              │
        │     Based on wall side/direction│
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  7. Draw textured column        │
        │     Map texture → screen pixels │
        └─────────────────────────────────┘
                          ↓
        ┌─────────────────────────────────┐
        │  8. Repeat for next column      │
        └─────────────────────────────────┘
```

### Multi-Ray Visualization

```
Top-down view showing multiple rays:

             │   │   │   │   │   │
             │   │   │   │   │   │  Screen
             └───┴───┴───┴───┴───┘
                      ║
                     ╱║╲
                    ╱ ║ ╲
                   ╱  ║  ╲
                  ╱   ║   ╲
                 ╱    ║    ╲
                ╱     ║     ╲
               ╱      ║      ╲
    ─────────•────────║───────•─────────  Camera plane
              ╲       ║      ╱
               ╲      ║     ╱
                ╲     ║    ╱
                 ╲    ║   ╱
     Wall ████████████║██████ Wall
                   ╲  ║  ╱
                    ╲ ║ ╱
                     ╲║╱
                      👤
                    Player

Each ray finds its wall and draws one vertical line
```

---

## 🎬 Animation: Player Movement

### Rotation

```
Before:                      After (rotate right):
Direction: (1, 0) →         Direction: (0.707, 0.707) ↗
Plane: (0, 0.66) ↑          Plane: (-0.466, 0.466) ↖

     ↑                              ╱
     |                             ╱
     |                            ╱↑
═════•═════→                     ╱ |
   Player                    ═══•══╱
                              Player
                              
FOV rotates with player!
```

### Forward Movement

```
Before:                      After (move forward):
Position: (3.5, 3.5)        Position: (3.7, 3.5)

  ┌───┬───┬───┬───┬───┐      ┌───┬───┬───┬───┬───┐
  │ 0 │ 0 │ 0 │ 0 │ 0 │      │ 0 │ 0 │ 0 │ 0 │ 0 │
  ├───┼───┼───┼───┼───┤      ├───┼───┼───┼───┼───┤
  │ 0 │ 0 │ 0 │ 0 │ 0 │      │ 0 │ 0 │ 0 │ 0 │ 0 │
  ├───┼───┼───┼───┼───┤      ├───┼───┼───┼───┼───┤
  │ 0 │ 0 │ 0 │ 0 │ 0 │      │ 0 │ 0 │ 0 │ 0 │ 0 │
  ├───┼───┼───┼───┼───┤      ├───┼───┼───┼───┼───┤
  │ 0 │ 0 │ 0 │👤│ 0 │      │ 0 │ 0 │ 0 │   👤0 │
  ├───┼───┼───┼───┼───┤      ├───┼───┼───┼───┼───┤
  │ 0 │ 0 │ 0 │ 0 │ 0 │      │ 0 │ 0 │ 0 │ 0 │ 0 │
  └───┴───┴───┴───┴───┘      └───┴───┴───┴───┴───┘

New position = Old position + direction × speed
(3.7, 3.5) = (3.5, 3.5) + (1, 0) × 0.2
```

---

## 📊 Data Flow Diagram

```
┌──────────────┐
│  GAME STATE  │
├──────────────┤
│ Player pos   │ (x, y) = (5.5, 3.2)
│ Direction    │ (dir_x, dir_y) = (1.0, 0.0)
│ Camera plane │ (plane_x, plane_y) = (0.0, 0.66)
│ Map grid     │ char **grid
│ Textures[4]  │ NO, SO, WE, EA
└──────┬───────┘
       │
       ↓
┌──────────────────────────────────────┐
│   MAIN RENDER LOOP (render_frame)   │
│   For each column x = 0 to 1279:    │
└──────┬───────────────────────────────┘
       │
       ↓
┌──────────────────┐    ┌──────────────────┐
│  RAY SETUP       │───→│  t_ray struct    │
│  - ray_init()    │    │  dir_x, dir_y    │
│  - ray_set_step()│    │  map_x, map_y    │
└──────┬───────────┘    │  delta_dist_x/y  │
       │                │  side_dist_x/y   │
       ↓                └──────────────────┘
┌──────────────────┐
│  DDA ALGORITHM   │
│  Step until hit  │
└──────┬───────────┘
       │
       ↓
┌──────────────────┐    ┌──────────────────┐
│  WALL FOUND!     │───→│  perp_dist       │
│  Calculate dist  │    │  side (0 or 1)   │
└──────┬───────────┘    │  map_x, map_y    │
       │                └──────────────────┘
       ↓
┌──────────────────┐    ┌──────────────────┐
│  PROJECTION      │───→│  line_height     │
│  Wall height     │    │  draw_start      │
└──────┬───────────┘    │  draw_end        │
       │                └──────────────────┘
       ↓
┌──────────────────┐    ┌──────────────────┐
│  TEXTURE MAP     │───→│  tex_index       │
│  Select & sample │    │  tex_x, tex_y    │
└──────┬───────────┘    │  color           │
       │                └──────────────────┘
       ↓
┌──────────────────┐
│  DRAW COLUMN     │
│  Ceiling         │ ← Solid color
│  Wall pixels     │ ← Textured
│  Floor           │ ← Solid color
└──────────────────┘
```

---

## 🧮 Memory Layout

### Map Grid Structure

```
t_map:
  char **grid ────┐
  int width = 8   │
  int height = 6  │
                  ↓
     ┌───────────────────┐
     │ grid[0] ─────────→│ "11111111\0"
     │ grid[1] ─────────→│ "10000001\0"
     │ grid[2] ─────────→│ "10000001\0"
     │ grid[3] ─────────→│ "10001001\0"
     │ grid[4] ─────────→│ "10000001\0"
     │ grid[5] ─────────→│ "11111111\0"
     │ grid[6] ─────────→│ NULL
     └───────────────────┘

Access: grid[y][x] gives character at position (x, y)
```

### Texture Image Structure

```
t_img texture[4]:
  
  texture[TEX_NO] ─┐
  texture[TEX_SO] ─┤
  texture[TEX_WE] ─┤
  texture[TEX_EA] ─┘
                   ↓
     ┌──────────────────────┐
     │ void *ptr            │ ← MLX image pointer
     │ char *addr ──────────┼→ Pixel data
     │ int bpp = 32         │   (bytes per pixel)
     │ int line_len = 256   │   (bytes per line)
     │ int endian           │
     │ int width = 64       │
     │ int height = 64      │
     └──────────────────────┘

Pixel access:
  offset = y × line_len + x × (bpp/8)
  color = *(int *)(addr + offset)
```

---

## 🎯 Performance Optimization Visual

### Ray Casting vs Ray Tracing

```
RAYCASTING (What we do - FAST):
┌─────────────────────────────────┐
│  Cast one ray per screen column │
│  Only check map grid            │
│  No recursion, no reflections   │
│  O(n) where n = screen width    │
└─────────────────────────────────┘

Player ═══→ Wall
        ═══→ Wall
        ═══→ Wall
        
1280 rays per frame
@ 60 FPS = 76,800 rays/sec


RAY TRACING (Not used - SLOW):
┌─────────────────────────────────┐
│  Cast multiple rays per pixel   │
│  Recursive bounces              │
│  Accurate lighting/shadows      │
│  O(n × m × bounces)             │
└─────────────────────────────────┘

         ╱→ Light ↗ Bounce
Player ═╱══→ Wall ╲ Bounce
        ╲         ↘ Bounce
         ╲→ Reflect

Millions of rays per frame
Too slow for real-time!
```

### Cache Efficiency

```
DDA steps sequentially through memory:

Good: Stepping in X direction
┌───┬───┬───┬───┬───┐
│ • │ →│ →│ →│ →│   Sequential access ✓
└───┴───┴───┴───┴───┘
grid[y][0], grid[y][1], grid[y][2]...

Less ideal: Stepping in Y direction  
┌───┐
│ • │ ↓ grid[0][x]
├───┤
│ • │ ↓ grid[1][x]
├───┤
│ • │ ↓ grid[2][x]
└───┘
Different cache lines
```

---

## 🐛 Common Visual Bugs

### Fisheye Effect

```
❌ WRONG (Euclidean distance):

     ╔════════╗
    ╱          ╲      Walls look curved
   ╱            ╲     (like looking through
  ╱              ╲     a fisheye lens)
 (    PLAYER     )
  ╲              ╱
   ╲            ╱
    ╲          ╱
     ╚════════╝


✓ CORRECT (Perpendicular distance):

     ┌──────────┐
     │          │      Walls look straight
     │          │      (realistic perspective)
     │  PLAYER  │
     │          │
     │          │
     └──────────┘
```

### Texture Bleeding

```
❌ WRONG (No bounds checking):

Wall 1  │  Wall 2
────────┼────────
███████│░░░░░░░   ← Textures from Wall 2
███████│░░░░░░░      appear on Wall 1!
███████│░░░░░░░
███████│████████


✓ CORRECT (Clamp tex_x):

Wall 1  │  Wall 2
────────┼────────
███████│████████   ← Each wall shows
███████│████████      its own texture
███████│████████
███████│████████
```

### Z-Fighting at Distance

```
Very far walls (large perp_dist):

❌ line_height = 720 / 1000 = 0.72 pixels
   → Rounds to 0, wall disappears!

✓ if (perp_dist > 0)
      line_height = max(1, 720 / perp_dist)
   → Always draw at least 1 pixel
```

---

## 🎓 Learning Path

```
┌─────────────────────────────────────────────────┐
│         RAYCASTING MASTERY ROADMAP              │
└─────────────────────────────────────────────────┘

Level 1: Understanding
├─ Read this visual guide
├─ Understand top-down map view
└─ Grasp ray casting concept

Level 2: Math Basics  
├─ Vector addition/subtraction
├─ Dot product for projection
├─ Similar triangles
└─ Trigonometry (FOV calculation)

Level 3: DDA Algorithm
├─ Grid traversal
├─ Delta distance calculation
├─ Side distance tracking
└─ Wall detection

Level 4: Projection
├─ Perspective math
├─ Fisheye correction
├─ Screen space mapping
└─ Clipping

Level 5: Textures
├─ Texture coordinate calculation
├─ Wall hit position
├─ Vertical mapping
└─ Color sampling

Level 6: Optimization
├─ Fixed-point arithmetic
├─ Cache optimization
├─ SIMD vectorization
└─ Multi-threading

Level 7: Extensions
├─ Floor/ceiling textures
├─ Sprites (enemies, items)
├─ Doors and animations
└─ Lighting effects
```

---

## 📚 Quick Reference

### Key Formulas

```
┌──────────────────────────────────────────────────┐
│  camera_x = 2 × (x / WIN_WIDTH) - 1             │
│  ray_dir = direction + plane × camera_x         │
│  delta_dist = |1 / ray_dir|                     │
│  perp_dist = side_dist - delta_dist             │
│  line_height = WIN_HEIGHT / perp_dist           │
│  start = -line_height/2 + WIN_HEIGHT/2          │
│  tex_x = fract(wall_hit_pos) × tex_width        │
│  step = tex_height / line_height               │
└──────────────────────────────────────────────────┘
```

### Data Structures

```
┌─────────────────────────────────────────────────┐
│  t_player: x, y, dir_x, dir_y, plane_x, plane_y│
│  t_ray: dir_x/y, map_x/y, delta_dist_x/y,      │
│         side_dist_x/y, step_x/y, side          │
│  t_map: **grid, width, height                  │
│  t_img: *ptr, *addr, width, height, bpp        │
└─────────────────────────────────────────────────┘
```

---

**Made with ❤️ for Cub3D learners**  
**Date:** October 24, 2025  
**Version:** 1.0 Visual Edition
