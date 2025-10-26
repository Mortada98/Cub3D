# Cub3D - Complete Code Explanation

**A comprehensive, line-by-line guide to understanding the Cub3D raycasting engine**

---

## Table of Contents

1. [Program Overview](#program-overview)
2. [Entry Point: main()](#entry-point-main)
3. [Helper Functions in main.c](#helper-functions-in-mainc)
4. [Parsing System](#parsing-system)
5. [Game Initialization](#game-initialization)
6. [Event Handling System](#event-handling-system)
7. [Game Loop & Updates](#game-loop--updates)
8. [Rendering Pipeline](#rendering-pipeline)
9. [Utility Functions](#utility-functions)
10. [Memory Management](#memory-management)

---

## Program Overview

### What is Cub3D?

Cub3D is a **raycasting-based 3D game engine** inspired by classic games like Wolfenstein 3D. It renders a 3D perspective from a 2D map using raycasting techniques.

### High-Level Flow

```
┌─────────────────────────────────────────────────────────────┐
│                     Program Startup                         │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
            ┌───────────────────────────────┐
            │   Validate CLI Arguments      │
            │   (argc, .cub extension)      │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │   Initialize Game Struct      │
            │   (Zero all memory)           │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │   Parse Scene File (.cub)     │
            │   - Textures (NO/SO/EA/WE)    │
            │   - Colors (F/C)              │
            │   - Map Grid                  │
            │   - Player Position           │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │   Initialize Game Engine      │
            │   - MLX library               │
            │   - Window creation           │
            │   - Load textures             │
            │   - Create frame buffer       │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │   Setup Event Hooks           │
            │   - Keyboard input            │
            │   - Window close              │
            │   - Game loop tick            │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │   Enter MLX Event Loop        │
            │   (Blocking - runs until exit)│
            └───────────┬───────────────────┘
                        │
                        ▼
        ┌───────────────────────────────────────┐
        │         GAME LOOP (per frame)         │
        │                                       │
        │  ┌──────────────────────────────────┐ │
        │  │  1. Process Input State          │ │
        │  │     - Update player movement     │ │
        │  │     - Apply rotation             │ │
        │  └──────────┬───────────────────────┘ │
        │             ▼                         │
        │  ┌──────────────────────────────────┐ │
        │  │  2. Render Frame                 │ │
        │  │     - Draw background (F/C)      │ │
        │  │     - Raycast each column        │ │
        │  │     - Draw textured walls        │ │
        │  └──────────┬───────────────────────┘ │
        │             ▼                         │
        │  ┌──────────────────────────────────┐ │
        │  │  3. Display Frame to Window      │ │
        │  └──────────────────────────────────┘ │
        │                                       │
        └────────────────┬──────────────────────┘
                         │
                         │ (Loop continues)
                         │
                         ▼
            ┌───────────────────────────────┐
            │   User Closes Window / ESC    │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │   Cleanup & Resource Freeing  │
            │   - Destroy images/textures   │
            │   - Destroy window/display    │
            │   - Free map & config data    │
            └───────────┬───────────────────┘
                        │
                        ▼
            ┌───────────────────────────────┐
            │      Exit Program (0)         │
            └───────────────────────────────┘
```

### Key Data Structures

The program revolves around these main structures defined in `cub3d.h`:

| Structure | Purpose                                   |
|-----------|-------------------------------------------|
| `t_game`  | Central game state holding all subsystems |
| `t_config`| Scene configuration (textures, colors)    |
| `t_map`   | 2D grid map data                          |
| `t_player`| Player position and orientation           |
| `t_input` | Current keyboard input state              |
| `t_ray`   | Raycasting calculation data               |
| `t_img`   | MLX image wrapper with pixel data         |

---

## Entry Point: main()

**File:** `src/main.c`

### Complete Function Code

```c
int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
		return (print_error("Usage: ./cub3D <map.cub>"));
	if (!check_extension(argv[1]))
		return (print_error("Invalid file extension"));
	init_game_struct(&game);
	if (parse_scene(&game, argv[1]))
	{
		destroy_game(&game);
		return (1);
	}
	if (init_game(&game))
	{
		destroy_game(&game);
		return (1);
	}
	setup_hooks(&game);
	mlx_loop(game.mlx);
	destroy_game(&game);
	return (0);
}
```

### Line-by-Line Breakdown

#### Line 1: Function Signature
```c
int	main(int argc, char **argv)
```
- **Return type:** `int` - Exit status code (0 = success, 1 = error)
- **Parameters:**
  - `argc`: Argument count (number of command-line arguments)
  - `argv`: Argument vector (array of string pointers)

**Expected usage:** `./cub3D maps/example.cub`

---

#### Line 3: Declare Game State
```c
t_game	game;
```
- Declares the main game structure on the **stack**
- This structure contains:
  - MLX pointers (`mlx`, `win`)
  - Frame buffer and textures
  - Configuration (colors, texture paths)
  - Map grid
  - Player state
  - Input state

**Memory layout:** Stack allocation (~1-2 KB depending on architecture)

```
Stack Memory:
┌────────────────────┐
│    t_game game     │ ← Local variable
│  ┌──────────────┐  │
│  │ mlx: NULL    │  │
│  │ win: NULL    │  │
│  │ frame: {...} │  │
│  │ textures[4]  │  │
│  │ config: {...}│  │
│  │ map: {...}   │  │
│  │ player: {...}│  │
│  │ input: {...} │  │
│  └──────────────┘  │
└────────────────────┘
```

---

#### Lines 5-6: Validate Argument Count
```c
if (argc != 2)
    return (print_error("Usage: ./cub3D <map.cub>"));
```

**Purpose:** Ensure exactly one argument (the map file path) is provided.

**Flow:**
- `argc` should be **2**: program name + map path
- If not, call `print_error()` which:
  1. Writes "Error\n" to stderr
  2. Writes the usage message to stderr
  3. Returns `1` (error code)

**Example scenarios:**
```bash
# ❌ Too few arguments (argc = 1)
./cub3D
# Output: Error\nUsage: ./cub3D <map.cub>

# ✅ Correct (argc = 2)
./cub3D maps/map1.cub

# ❌ Too many arguments (argc = 3)
./cub3D maps/map1.cub extra_arg
# Output: Error\nUsage: ./cub3D <map.cub>
```

---

#### Lines 7-8: Validate File Extension
```c
if (!check_extension(argv[1]))
    return (print_error("Invalid file extension"));
```

**Purpose:** Ensure the file ends with `.cub` extension.

**Flow:**
1. Calls `check_extension(argv[1])` → returns 1 if valid, 0 if invalid
2. The `!` negates it: if invalid (0), condition is true
3. Returns early with error message

**What `check_extension()` does:**
- Gets string length
- Checks if length ≥ 4
- Compares last 4 characters to ".cub"

**Example scenarios:**
```bash
# ✅ Valid extensions
./cub3D map.cub          # OK
./cub3D path/to/file.cub # OK

# ❌ Invalid extensions
./cub3D map.txt          # Error: Invalid file extension
./cub3D map.CUB          # Error: Case-sensitive check
./cub3D map              # Error: Too short
./cub3D .cub             # OK but unusual (exactly 4 chars)
```

---

#### Line 9: Initialize Game Structure
```c
init_game_struct(&game);
```

**Purpose:** Zero-initialize all fields in the game structure to a safe state.

**What happens:**
```c
ft_bzero(&game, sizeof(t_game));
```

**Memory before:**
```
game.mlx         = ??? (garbage)
game.win         = ??? (garbage)
game.frame.ptr   = ??? (garbage)
game.map.grid    = ??? (garbage)
game.running     = ??? (garbage)
```

**Memory after:**
```
game.mlx         = NULL (0x0)
game.win         = NULL (0x0)
game.frame.ptr   = NULL (0x0)
game.map.grid    = NULL (0x0)
game.map.height  = 0
game.map.width   = 0
game.running     = 0
game.input.forward = 0
... (all bytes set to 0)
```

**Why this matters:**
- Prevents undefined behavior
- Makes `destroy_game()` safe to call even if initialization fails partway
- Ensures pointers are NULL (safe to check before freeing)

---

#### Lines 10-14: Parse Scene File
```c
if (parse_scene(&game, argv[1]))
{
    destroy_game(&game);
    return (1);
}
```

**Purpose:** Read and parse the `.cub` file to extract:
- Texture paths (NO, SO, EA, WE)
- Floor and ceiling colors (F, C)
- Map grid
- Player starting position and orientation

**Flow:**

```
parse_scene() returns:
  0 = Success
  1 = Error (with message already printed)

If error (non-zero):
  ├─ Call destroy_game() to free any partial allocations
  └─ Return 1 (exit with error)
```

**What gets populated:**
```c
// Before parse_scene:
game.config.texture[0-3] = NULL
game.config.floor_set = 0
game.config.ceiling_set = 0
game.map.grid = NULL
game.player.x = 0.0

// After successful parse_scene:
game.config.texture[TEX_NO] = "./textures/north.xpm"
game.config.texture[TEX_SO] = "./textures/south.xpm"
game.config.texture[TEX_EA] = "./textures/east.xpm"
game.config.texture[TEX_WE] = "./textures/west.xpm"
game.config.floor = {r:220, g:100, b:0}
game.config.ceiling = {r:135, g:206, b:250}
game.map.grid = [
  "111111",
  "100001",
  "101N01",  // N = player facing north
  "100001",
  "111111"
]
game.map.width = 6
game.map.height = 5
game.player.x = 3.5
game.player.y = 2.5
game.player.dir_x = 0
game.player.dir_y = -1  // Facing north
```

**Error handling:**
- If parsing fails, any allocated memory is freed by `destroy_game()`
- Common errors:
  - File not found
  - Missing texture/color definitions
  - Invalid map (not enclosed by walls)
  - Missing player start position
  - Multiple player positions

---

#### Lines 15-19: Initialize Game Engine
```c
if (init_game(&game))
{
    destroy_game(&game);
    return (1);
}
```

**Purpose:** Initialize the MiniLibX graphics system and load resources.

**What `init_game()` does:**

1. **Initialize MLX**
   ```c
   game->mlx = mlx_init();
   ```
   - Connects to X11 display
   - Creates MLX context
   - Returns NULL on failure

2. **Create Window**
   ```c
   game->win = mlx_new_window(game->mlx, 1280, 720, "cub3D");
   ```
   - Creates a 1280x720 window
   - Title: "cub3D"

3. **Create Frame Buffer**
   ```c
   game->frame.ptr = mlx_new_image(game->mlx, 1280, 720);
   game->frame.addr = mlx_get_data_addr(game->frame.ptr, ...);
   ```
   - Allocates off-screen image buffer
   - Gets direct pixel access pointer

4. **Load Textures**
   ```c
   for (i = 0; i < 4; i++)
       load_texture(game, i, game->config.texture[i]);
   ```
   - Loads each wall texture from XPM file
   - Stores pixel data for raycasting

5. **Set Running Flag**
   ```c
   game->running = 1;
   ```

**Visual representation:**
```
┌─────────────────────────────────────┐
│          X11 Display                │
│  ┌───────────────────────────────┐  │
│  │  MLX Window (1280x720)        │  │
│  │  Title: "cub3D"               │  │
│  │                               │  │
│  │   ┌─────────────────────┐     │  │
│  │   │  Frame Buffer       │     │  │
│  │   │  (off-screen image) │     │  │
│  │   │  1280x720 pixels    │     │  │
│  │   └─────────────────────┘     │  │
│  │                               │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘

Memory (Heap):
┌──────────────────────┐
│ Texture[0] - North   │ 64x64 pixels
├──────────────────────┤
│ Texture[1] - South   │ 64x64 pixels
├──────────────────────┤
│ Texture[2] - West    │ 64x64 pixels
├──────────────────────┤
│ Texture[3] - East    │ 64x64 pixels
└──────────────────────┘
```

**Error handling:**
- If any step fails, cleanup and return 1
- Common errors:
  - MLX initialization failure (no X11 display)
  - Window creation failure
  - Texture file not found or invalid format

---

#### Line 20: Setup Event Hooks
```c
setup_hooks(&game);
```

**Purpose:** Register callback functions for user input and game loop.

**What gets registered:**

```c
// Keyboard press events
mlx_hook(game->win, EVENT_KEY_PRESS, MASK_KEY_PRESS, 
         handle_key_press, game);

// Keyboard release events
mlx_hook(game->win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE, 
         handle_key_release, game);

// Window close event (X button)
mlx_hook(game->win, EVENT_DESTROY, 0, 
         handle_close, game);

// Game loop tick (called continuously)
mlx_loop_hook(game->mlx, game_loop, game);
```

**Event flow diagram:**

```
User Input Events:
┌─────────────┐
│  Keyboard   │
└──────┬──────┘
       │
       ├─ Key Press (W/A/S/D/Arrows) ──→ handle_key_press()
       │                                   └─ Sets input flags
       │
       └─ Key Release ────────────────→ handle_key_release()
                                          └─ Clears input flags

┌─────────────┐
│ Window (X)  │ ──→ handle_close()
└─────────────┘      └─ Cleanup & exit

MLX Loop:
┌───────────────────────────────────────┐
│  Every frame (continuous loop)        │
│                                       │
│  game_loop() ──→ update_game_state()  │
│                  └─ Read input flags  │
│                  └─ Move player       │
│                  └─ Rotate camera     │
│                                       │
│               ──→ render_frame()      │
│                  └─ Raycast scene     │
│                  └─ Draw to buffer    │
│                  └─ Display frame     │
└───────────────────────────────────────┘
```

**No errors:** This function doesn't fail; it just registers callbacks.

---

#### Line 21: Enter Main Loop
```c
mlx_loop(game.mlx);
```

**Purpose:** Start the MLX event loop - this **blocks** until the window is closed.

**What happens:**

```
┌────────────────────────────────────┐
│      MLX Event Loop (Infinite)     │
│                                    │
│  while (window_is_open) {          │
│                                    │
│    1. Poll X11 events              │
│       - Keyboard events            │
│       - Mouse events               │
│       - Window events              │
│                                    │
│    2. Call registered callbacks    │
│       - Key press/release handlers │
│       - Loop hook (game_loop)      │
│                                    │
│    3. Handle window refresh        │
│  }                                 │
└────────────────────────────────────┘
```

**Frame-by-frame execution:**

```
Frame 1:
  ├─ Check events (no input)
  ├─ Call game_loop()
  │   ├─ update_game_state() (no movement)
  │   └─ render_frame() (draw initial view)
  └─ Display to window

Frame 2:
  ├─ Check events (W key pressed)
  │   └─ handle_key_press(KEY_W) → game.input.forward = 1
  ├─ Call game_loop()
  │   ├─ update_game_state()
  │   │   └─ Move player forward (player.x += dir_x * speed)
  │   └─ render_frame() (draw new view)
  └─ Display to window

Frame 3:
  ├─ Check events (W key released)
  │   └─ handle_key_release(KEY_W) → game.input.forward = 0
  ├─ Call game_loop()
  │   ├─ update_game_state() (stop moving)
  │   └─ render_frame()
  └─ Display to window

... (loop continues) ...

User clicks X or presses ESC:
  ├─ handle_close() called
  │   ├─ game.running = 0
  │   ├─ destroy_game()
  │   └─ exit(0)
  └─ Loop terminates
```

**This line doesn't return** until the window is closed!

---

#### Line 22: Cleanup Resources
```c
destroy_game(&game);
```

**Purpose:** Free all allocated resources before program exit.

**What gets freed:**

```c
// 1. Destroy frame buffer image
if (game->frame.ptr)
    mlx_destroy_image(game->mlx, game->frame.ptr);

// 2. Destroy all 4 texture images
for (i = 0; i < 4; i++)
    if (game->texture[i].ptr)
        mlx_destroy_image(game->mlx, game->texture[i].ptr);

// 3. Destroy window
if (game->win)
    mlx_destroy_window(game->mlx, game->win);

// 4. Destroy MLX display connection
if (game->mlx)
{
    mlx_destroy_display(game->mlx);
    free(game->mlx);
}

// 5. Free map grid
for (i = 0; i < game->map.height; i++)
    free(game->map.grid[i]);
free(game->map.grid);

// 6. Free config texture paths
for (i = 0; i < 4; i++)
    free(game->config.texture[i]);
```

**Memory cleanup visualization:**

```
Before destroy_game():
Heap Memory:
├─ game.mlx ───────→ [MLX Context]
├─ game.frame.ptr ─→ [Frame Image 1280x720]
├─ game.texture[0]→  [North Texture 64x64]
├─ game.texture[1]→  [South Texture 64x64]
├─ game.texture[2]→  [West Texture 64x64]
├─ game.texture[3]→  [East Texture 64x64]
├─ game.map.grid ──→ [Array of row pointers]
│   ├─ grid[0] ────→ ["111111"]
│   ├─ grid[1] ────→ ["100001"]
│   └─ grid[2] ────→ ["100001"]
└─ config.texture[0-3] → [Path strings]

After destroy_game():
Heap Memory: (all freed)
├─ game.mlx = NULL
├─ game.frame.ptr = NULL
├─ game.texture[*] = NULL
├─ game.map.grid = NULL
└─ config.texture[*] = NULL
```

**Note:** This is called even in error paths (after parse/init failures).

---

#### Line 23: Return Success
```c
return (0);
```

**Purpose:** Exit program with success code.

**Exit codes:**
- `0` = Success (normal exit after window close)
- `1` = Error (returned earlier in error paths)

**Note:** In practice, this line is rarely reached because `handle_close()` calls `exit(0)` directly when the window is closed.

---

## Helper Functions in main.c

### 1. check_extension()

**File:** `src/main.c`

```c
static int	check_extension(const char *path)
{
	int	len;

	len = ft_strlen(path);
	if (len < 4)
		return (0);
	return (!ft_strncmp(path + len - 4, ".cub", 4));
}
```

**Purpose:** Validate that the file path ends with `.cub` extension.

**Parameters:**
- `path`: C-string containing the file path

**Return value:**
- `1` if path ends with `.cub`
- `0` otherwise

#### Line-by-Line Analysis

**Line 1: Function signature**
```c
static int	check_extension(const char *path)
```
- `static`: Internal linkage - only visible within `main.c`
- `const char *path`: Read-only string pointer (won't modify the path)

**Line 3: Declare length variable**
```c
int	len;
```

**Line 5: Get string length**
```c
len = ft_strlen(path);
```
- Calls custom `ft_strlen()` (equivalent to standard `strlen()`)
- Returns number of characters before null terminator

**Lines 6-7: Guard against short paths**
```c
if (len < 4)
    return (0);
```
- `.cub` is 4 characters
- If path has fewer than 4 chars, it can't be valid
- Early return optimization

**Visual examples:**
```
Valid paths (len >= 4):
"a.cub"          len=5  ✓
"map.cub"        len=7  ✓
"maps/test.cub"  len=14 ✓

Invalid paths (len < 4):
""               len=0  ✗ (returns 0 immediately)
"a"              len=1  ✗ (returns 0 immediately)
"ab"             len=2  ✗ (returns 0 immediately)
"abc"            len=3  ✗ (returns 0 immediately)
```

**Line 8: Compare extension**
```c
return (!ft_strncmp(path + len - 4, ".cub", 4));
```

**Breaking this down:**

1. **`path + len - 4`**: Pointer arithmetic to get last 4 characters
   ```
   path = "maps/test.cub"
   len = 13
   
   path + len - 4 = path + 9
                  = pointer to "t.cub"
                  
   Visual:
   m a p s / t e s t . c u b \0
   0 1 2 3 4 5 6 7 8 9 10 11 12 13
                     ^
                     |
                  len-4 = 9
   ```

2. **`ft_strncmp(path + len - 4, ".cub", 4)`**: Compare 4 characters
   - Returns `0` if strings match
   - Returns non-zero if they differ

3. **`!` (NOT operator)**: Convert to boolean
   - `!0` = `1` (true - strings match)
   - `!non-zero` = `0` (false - strings differ)

**Truth table:**
```
ft_strncmp result  →  !result  →  Meaning
─────────────────────────────────────────
0 (match)         →  1 (true) →  Valid extension
!= 0 (no match)   →  0 (false)→  Invalid extension
```

**Test cases:**
```c
check_extension("map.cub")     → ft_strncmp(".cub", ".cub", 4) → 0 → !0 → 1 ✓
check_extension("map.txt")     → ft_strncmp(".txt", ".cub", 4) → ≠0 → !≠0 → 0 ✗
check_extension("map.CUB")     → ft_strncmp(".CUB", ".cub", 4) → ≠0 → !≠0 → 0 ✗ (case-sensitive!)
check_extension("test.cub.bak")→ ft_strncmp(".bak", ".cub", 4) → ≠0 → !≠0 → 0 ✗
check_extension(".cub")        → ft_strncmp(".cub", ".cub", 4) → 0 → !0 → 1 ✓
```

**Edge cases:**
- Empty string: `len=0 < 4` → returns `0` ✓
- Exactly 4 chars: `".cub"` → valid ✓
- Case sensitivity: `".CUB"` → invalid ✗
- Extension in middle: `"file.cub.txt"` → checks last 4 chars → invalid ✗

---

### 2. init_game_struct()

**File:** `src/main.c`

```c
static void	init_game_struct(t_game *game)
{
	ft_bzero(game, sizeof(t_game));
}
```

**Purpose:** Zero-initialize the entire game structure to a clean, safe state.

**Parameters:**
- `game`: Pointer to the game structure to initialize

**Return value:** `void` (no return)

#### Line-by-Line Analysis

**Line 1: Function signature**
```c
static void	init_game_struct(t_game *game)
```
- `static`: Internal to `main.c`
- `void`: No return value
- `t_game *game`: Pointer to structure (allows modification)

**Line 3: Zero all bytes**
```c
ft_bzero(game, sizeof(t_game));
```

**What this does:**
- Sets every byte in the structure to `0`
- `sizeof(t_game)`: Total size of structure in bytes (probably ~200-500 bytes)
- Custom implementation of standard `bzero()`

**Memory transformation:**

```
Before ft_bzero (uninitialized - garbage values):
┌─────────────────────────────────────┐
│ t_game structure                    │
├─────────────────────────────────────┤
│ mlx:         0x????????  (garbage)  │
│ win:         0x????????  (garbage)  │
│ frame.ptr:   0x????????  (garbage)  │
│ frame.addr:  0x????????  (garbage)  │
│ frame.width: 0x????????  (garbage)  │
│ ...                                 │
│ map.grid:    0x????????  (garbage)  │
│ map.height:  ????????    (garbage)  │
│ player.x:    ?.??????    (garbage)  │
│ input.forward: ?         (garbage)  │
│ running:     ????????    (garbage)  │
└─────────────────────────────────────┘

After ft_bzero (all zeros):
┌─────────────────────────────────────┐
│ t_game structure                    │
├─────────────────────────────────────┤
│ mlx:         NULL (0x0000000000)    │
│ win:         NULL (0x0000000000)    │
│ frame.ptr:   NULL (0x0000000000)    │
│ frame.addr:  NULL (0x0000000000)    │
│ frame.width: 0                      │
│ frame.height: 0                     │
│ ...                                 │
│ texture[0-3].ptr: NULL              │
│ config.texture[0-3]: NULL           │
│ config.has_texture[0-3]: 0          │
│ config.floor_set: 0                 │
│ config.ceiling_set: 0               │
│ map.grid:    NULL (0x0000000000)    │
│ map.height:  0                      │
│ map.width:   0                      │
│ player.x:    0.0                    │
│ player.y:    0.0                    │
│ player.dir_x: 0.0                   │
│ player.dir_y: 0.0                   │
│ player.plane_x: 0.0                 │
│ player.plane_y: 0.0                 │
│ input.forward: 0                    │
│ input.backward: 0                   │
│ input.left: 0                       │
│ input.right: 0                      │
│ input.turn_left: 0                  │
│ input.turn_right: 0                 │
│ running:     0                      │
└─────────────────────────────────────┘
```

**Why this is critical:**

1. **Safe cleanup:** If initialization fails partway, `destroy_game()` can safely check `if (ptr != NULL)` before freeing
   ```c
   // Safe because NULL pointers are guaranteed
   if (game->frame.ptr)  // NULL check works!
       mlx_destroy_image(game->mlx, game->frame.ptr);
   ```

2. **Prevents undefined behavior:** Reading uninitialized memory is undefined behavior
   ```c
   // Without zeroing:
   if (game->running)  // Could be 0 or garbage (unpredictable!)
       do_something();
   
   // With zeroing:
   if (game->running)  // Guaranteed to be 0 (false)
       do_something();  // Won't execute until explicitly set to 1
   ```

3. **Predictable state:** All flags and counters start at known values
   ```c
   game->map.height = 0     // Known starting point
   game->input.forward = 0  // No movement initially
   ```

**Cost:** Minimal (~500 bytes to zero, very fast operation)

**Best practice:** Always initialize structures before use, especially those with pointers!

---

### 3. setup_hooks()

**File:** `src/main.c`

```c
static void	setup_hooks(t_game *game)
{
	mlx_hook(game->win, EVENT_KEY_PRESS, MASK_KEY_PRESS, handle_key_press, game);
	mlx_hook(game->win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE, handle_key_release, game);
	mlx_hook(game->win, EVENT_DESTROY, 0, handle_close, game);
	mlx_loop_hook(game->mlx, game_loop, game);
}
```

**Purpose:** Register event callback functions with the MiniLibX event system.

**Parameters:**
- `game`: Pointer to fully initialized game structure

**Return value:** `void`

**Prerequisites:** 
- `game->mlx` must be initialized (from `mlx_init()`)
- `game->win` must be created (from `mlx_new_window()`)

#### Line-by-Line Analysis

**Line 1: Function signature**
```c
static void	setup_hooks(t_game *game)
```

**Lines 3-6: Register event hooks**

Each `mlx_hook()` call has this signature:
```c
int mlx_hook(void *win_ptr, int x_event, int x_mask, 
             int (*f)(int, void*), void *param);
```

**Parameters explained:**
- `win_ptr`: Window to attach the hook to
- `x_event`: X11 event type code
- `x_mask`: X11 event mask (filter for events)
- `f`: Callback function pointer
- `param`: User data passed to callback (our `game` pointer)

---

#### Hook 1: Key Press Events

**Line 3:**
```c
mlx_hook(game->win, EVENT_KEY_PRESS, MASK_KEY_PRESS, handle_key_press, game);
```

**Constants (from `cub3d.h`):**
```c
#define EVENT_KEY_PRESS 2          // X11 KeyPress event
#define MASK_KEY_PRESS (1L << 0)   // KeyPressMask
```

**What this does:**
- Listens for keyboard key press events
- When a key is pressed, calls `handle_key_press(keycode, game)`
- Callback signature: `int handle_key_press(int keycode, void *param)`

**Event flow:**
```
User presses 'W' key
        ↓
X11 generates KeyPress event (code=119)
        ↓
MLX receives event
        ↓
MLX checks registered hooks for EVENT_KEY_PRESS
        ↓
MLX calls: handle_key_press(119, game)
        ↓
Function sets: game->input.forward = 1
```

---

#### Hook 2: Key Release Events

**Line 4:**
```c
mlx_hook(game->win, EVENT_KEY_RELEASE, MASK_KEY_RELEASE, handle_key_release, game);
```

**Constants:**
```c
#define EVENT_KEY_RELEASE 3        // X11 KeyRelease event
#define MASK_KEY_RELEASE (1L << 1) // KeyReleaseMask
```

**What this does:**
- Listens for keyboard key release events
- When a key is released, calls `handle_key_release(keycode, game)`
- Callback signature: `int handle_key_release(int keycode, void *param)`

**Why both press AND release?**

This enables **smooth continuous movement**:

```
Without separate press/release:
Press W → move once
Hold W → nothing (no repeat)
Release W → nothing

With separate press/release:
Press W → game.input.forward = 1
Hold W → (input flag stays 1)
         → game_loop() keeps moving player each frame
Release W → game.input.forward = 0
         → game_loop() stops moving player
```

**State tracking example:**
```
Time  | Event        | input.forward | Player movement
─────────────────────────────────────────────────────
t=0   | W pressed    | 0 → 1         | Start moving
t=1   | (holding)    | 1             | Still moving
t=2   | (holding)    | 1             | Still moving
t=3   | W released   | 1 → 0         | Stop moving
t=4   | (no input)   | 0             | Stationary
```

---

#### Hook 3: Window Close Event

**Line 5:**
```c
mlx_hook(game->win, EVENT_DESTROY, 0, handle_close, game);
```

**Constants:**
```c
#define EVENT_DESTROY 17  // X11 DestroyNotify event
```

**What this does:**
- Listens for window close events (clicking X button)
- Mask is `0` (DestroyNotify doesn't need a mask)
- Calls `handle_close(game)` when window is destroyed

**Event flow:**
```
User clicks [X] button on window
        ↓
X11 sends DestroyNotify event
        ↓
MLX intercepts before actual destruction
        ↓
MLX calls: handle_close(0, game)
        ↓
handle_close() performs cleanup:
  - Sets game->running = 0
  - Calls destroy_game()
  - Calls exit(EXIT_SUCCESS)
        ↓
Program terminates cleanly
```

**Why this matters:**
- Prevents resource leaks (textures, window, memory)
- Ensures graceful shutdown
- Matches user expectation (X button closes program)

---

#### Hook 4: Game Loop Tick

**Line 6:**
```c
mlx_loop_hook(game->mlx, game_loop, game);
```

**Different function:** `mlx_loop_hook()` instead of `mlx_hook()`

**What this does:**
- Registers a function to be called **every iteration** of the MLX event loop
- No specific event needed - runs continuously
- Callback signature: `int game_loop(void *param)`

**Execution pattern:**
```
mlx_loop() starts:
  ┌──────────────────────────────┐
  │  MLX Event Loop (infinite)   │
  │                              │
  │  while (1) {                 │
  │    process_x11_events();     │◄── Check keyboard, mouse, etc.
  │    call_registered_hooks();  │◄── Call handle_key_press/release if events
  │    game_loop(game);          │◄── Called EVERY iteration
  │    refresh_display();        │
  │  }                           │
  └──────────────────────────────┘
```

**Typical frame rate:**
- Runs as fast as possible (no built-in frame limiting)
- Usually 60-1000+ FPS depending on CPU
- Each call updates physics and renders a new frame

**What `game_loop()` does:**
```c
int game_loop(void *param)
{
    t_game *game = (t_game *)param;
    
    if (!game->running)
        return (0);
    
    update_game_state(game);  // Move player based on input flags
    render_frame(game);       // Raycast and draw scene
    
    return (0);
}
```

---

#### Visual Summary: Hook System

```
┌───────────────────────────────────────────────────────────┐
│                    MLX Event System                       │
├───────────────────────────────────────────────────────────┤
│                                                           │
│  Window Events:                                           │
│  ┌──────────┐                                             │
│  │ Key Down │──→ EVENT_KEY_PRESS ──→ handle_key_press()   │
│  └──────────┘                              ↓              │
│                                     Set input flags       │
│                                                           │
│  ┌──────────┐                                             │
│  │  Key Up  │──→ EVENT_KEY_RELEASE ─→ handle_key_release()│
│  └──────────┘                              ↓              │
│                                    Clear input flags      │
│                                                           │
│  ┌──────────┐                                             │
│  │ Click [X]│──→ EVENT_DESTROY ──→ handle_close()         │
│  └──────────┘                              ↓              │
│                                    Cleanup & exit         │
│                                                           │
│  Loop Tick:                                               │
│  ┌──────────┐                                             │
│  │Every Frame│─→ mlx_loop_hook ──→ game_loop()            │
│  └──────────┘                              ↓              │
│                              Update physics & render      │
│                                                           │
└───────────────────────────────────────────────────────────┘

All callbacks receive the 'game' pointer as their parameter!
```

**Why pass `game` to all callbacks?**
- Callbacks need access to game state
- C doesn't have closures/lambda captures
- Manual parameter passing achieves the same effect

---

## Parsing System

The parsing system reads `.cub` files and populates the game configuration and map data. This is a multi-stage process involving several files.

### Parsing Overview

**Files involved:**
- `src/parsing/parse_scene.c` - Main parsing coordinator
- `src/parsing/scene_config.c` - Configuration entry processing
- `src/parsing/parse_texture.c` - Texture path parsing
- `src/parsing/parse_color.c` - RGB color parsing
- `src/parsing/parse_map.c` - Map grid construction
- `src/parsing/player.c` - Player position extraction
- `src/parsing/validate.c` - Scene validation

### .cub File Format

Example valid `.cub` file:
```
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm

F 220,100,0
C 135,206,250

        1111111111111111111111111
        1000000000110000000000001
        1011000001110000000000001
        1001000000000000000000001
111111111011000001110000000000001
100000000011000001110111111111111
11110111111111011100000010001
11110111111111011101010010001
11000000110101011100000010001
10000000000000001100000010001
10000000000000001101010010001
11000001110101011111011110N0111
11110111 1110101 101111010001
11111111 1111111 111111111111
```

**Format rules:**
1. Texture paths: `NO`, `SO`, `WE`, `EA` followed by path
2. Colors: `F` (floor), `C` (ceiling) followed by `R,G,B`
3. Empty lines allowed before map
4. Map must be last
5. Map must be enclosed by walls (`1` or space)
6. Player start: `N`, `S`, `E`, or `W` (exactly one)

---

### parse_scene() - Main Entry Point

**File:** `src/parsing/parse_scene.c`

```c
int	parse_scene(t_game *game, const char *path)
{
	char	**map_lines;
	int		map_started;

	map_lines = NULL;
	map_started = 0;
	game->map.height = 0;
	if (load_scene_lines(path, game, &map_lines, &map_started))
		return (1);
	return (finalize_scene_data(game, map_lines, map_started));
}
```

**Purpose:** Top-level parser that orchestrates reading and validating the scene file.

**Parameters:**
- `game`: Game structure to populate
- `path`: Path to `.cub` file

**Return:** `0` on success, `1` on error

#### Parsing Flow Diagram

```
parse_scene()
    ├─ Initialize locals (map_lines=NULL, map_started=0)
    ├─ game->map.height = 0
    │
    ├─ load_scene_lines()
    │   ├─ Open file
    │   ├─ Read line by line
    │   │   ├─ Before map: Process config entries (NO/SO/WE/EA/F/C)
    │   │   └─ After map starts: Collect map lines
    │   └─ Close file
    │
    └─ finalize_scene_data()
        ├─ Check map exists
        ├─ finalize_map() - Build rectangular grid
        └─ validate_scene()
            ├─ validate_config() - Check all textures/colors set
            ├─ setup_player() - Find player, set orientation
            └─ check_map_cells() - Verify walls, no holes
```

#### Line-by-Line Breakdown

**Lines 3-4: Declare locals**
```c
char	**map_lines;
int		map_started;
```
- `map_lines`: Dynamic array to collect map rows during reading
- `map_started`: Flag to track when we've encountered the first map line

**Lines 6-8: Initialize**
```c
map_lines = NULL;
map_started = 0;
game->map.height = 0;
```
- Start with no map lines
- Haven't seen map yet
- Height counter at 0 (will increment as we collect lines)

**Lines 9-10: Load file**
```c
if (load_scene_lines(path, game, &map_lines, &map_started))
    return (1);
```
- Reads the entire file
- Populates `game->config` with textures/colors
- Collects map lines into `map_lines` array
- Sets `map_started` to 1 when first map line encountered
- Returns `1` on error (file not found, read error, invalid config)

**Line 11: Finalize**
```c
return (finalize_scene_data(game, map_lines, map_started));
```
- Processes collected map lines into final grid
- Validates the complete scene
- Frees temporary `map_lines` array
- Returns validation result

---

### load_scene_lines() - File Reading

```c
static int	load_scene_lines(const char *path, t_game *game,
			char ***map_lines, int *map_started)
{
	int		status;
	int		fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (print_error("Unable to open .cub file"));
	status = read_config_and_map(fd, game, map_lines, map_started);
	close(fd);
	if (status < 0)
	{
		free_str_array(*map_lines);
		return (print_error("Failed to read file"));
	}
	return (0);
}
```

**Purpose:** Open the file and delegate to line-by-line reader.

**Flow:**
1. Open file for reading (`O_RDONLY`)
2. If open fails → error (file doesn't exist or no permission)
3. Read all lines
4. Close file descriptor
5. Check for read errors
6. Cleanup on error

**Error cases:**
```c
// File doesn't exist
open("missing.cub", O_RDONLY) → returns -1
→ Error: Unable to open .cub file

// Permission denied
open("noperm.cub", O_RDONLY) → returns -1 (errno=EACCES)
→ Error: Unable to open .cub file

// Read error (disk failure, etc.)
read_config_and_map() → returns -1
→ Free partial map lines
→ Error: Failed to read file
```

---

### read_config_and_map() - Line Processing

```c
static int	read_config_and_map(int fd, t_game *game,
			char ***map_lines, int *map_started)
{
	int		status;
	char	*line;

	while ((status = read_line(fd, &line)) >= 0)
	{
		if (status == 0)
		{
			free(line);
			break ;
		}
		if (handle_config_line(game, &line, map_lines, map_started))
		{
			free(line);
			return (-1);
		}
		if (line)
			free(line);
	}
	return (status);
}
```

**Purpose:** Read file line-by-line and route each line to appropriate handler.

**`read_line()` return values:**
- `1` = Line successfully read (stored in `*line`)
- `0` = End of file reached
- `-1` = Read error

#### Loop Breakdown

**Line 7: Read loop**
```c
while ((status = read_line(fd, &line)) >= 0)
```
- Continues while not EOF and no read error
- `read_line()` allocates memory for `line`
- Caller must free `line`

**Lines 9-12: Handle EOF**
```c
if (status == 0)
{
    free(line);
    break;
}
```
- `status == 0` means end of file
- Free the line buffer
- Exit loop normally

**Lines 13-17: Process line**
```c
if (handle_config_line(game, &line, map_lines, map_started))
{
    free(line);
    return (-1);
}
```
- `handle_config_line()` determines if it's config or map
- Returns `1` on error (invalid syntax, duplicate, etc.)
- Frees line and propagates error up

**Lines 18-19: Cleanup**
```c
if (line)
    free(line);
```
- Free line if it wasn't consumed by `handle_config_line()`
- Some handlers set `*line = NULL` after taking ownership

**Example execution:**

```
File content:
───────────────
NO ./north.xpm
F 220,100,0
              ← empty line
1111
1001
───────────────

Iteration 1: read_line() → "NO ./north.xpm"
  ├─ handle_config_line() → parses texture
  ├─ game->config.texture[TEX_NO] = "./north.xpm"
  └─ free(line)

Iteration 2: read_line() → "F 220,100,0"
  ├─ handle_config_line() → parses floor color
  ├─ game->config.floor = {220, 100, 0}
  └─ free(line)

Iteration 3: read_line() → ""
  ├─ handle_config_line() → empty, ignored
  └─ free(line)

Iteration 4: read_line() → "1111"
  ├─ is_map_line() → TRUE
  ├─ map_started = 1
  ├─ arr_push(&map_lines, "1111")
  └─ line set to NULL (ownership transferred)

Iteration 5: read_line() → "1001"
  ├─ map_started already 1
  ├─ arr_push(&map_lines, "1001")
  └─ line set to NULL

Iteration 6: read_line() → EOF (status = 0)
  ├─ free(line)
  └─ break

Result:
  map_lines = ["1111", "1001", NULL]
  map_started = 1
```

---

### handle_config_line() - Line Routing

```c
static int	handle_config_line(t_game *game, char **line,
			char ***map_lines, int *map_started)
{
	if (*map_started || is_map_line(*line))
	{
		if (!scene_config_ready(&game->config))
			return (print_error("Map before textures/colors"));
		if (is_all_space(*line))
			return (print_error("Empty line inside map"));
		*map_started = 1;
		arr_push(map_lines, &game->map.height, *line);
		*line = NULL;
		return (0);
	}
	return (scene_process_entry(game, *line));
}
```

**Purpose:** Determine if line is config or map, and route accordingly.

**Decision tree:**

```
Line read from file
        │
        ├─ Is map started? OR is_map_line()?
        │         │
        │        YES ─→ MAP SECTION
        │                 │
        │                 ├─ Config complete? (all textures/colors set)
        │                 │     NO → ERROR: "Map before textures/colors"
        │                 │
        │                 ├─ Is line all spaces?
        │                 │     YES → ERROR: "Empty line inside map"
        │                 │
        │                 ├─ Set map_started = 1
        │                 ├─ Add line to map_lines array
        │                 ├─ Set *line = NULL (transfer ownership)
        │                 └─ Return 0 (success)
        │
        └─ NO ──→ CONFIG SECTION
                  │
                  └─ scene_process_entry() → parse_texture/color
```

**Key validations:**

1. **Config must be complete before map:**
   ```c
   if (!scene_config_ready(&game->config))
       return (print_error("Map before textures/colors"));
   ```
   - All 4 textures must be defined
   - Floor and ceiling colors must be defined
   - Prevents: map appearing before `NO` directive

2. **No empty lines inside map:**
   ```c
   if (is_all_space(*line))
       return (print_error("Empty line inside map"));
   ```
   - Once map starts, every line must have content
   - Prevents holes/gaps in map grid

**Example scenarios:**

```
✓ Valid:
NO ./north.xpm
SO ./south.xpm
WE ./west.xpm
EA ./east.xpm
F 220,100,0
C 135,206,250
              ← empty lines OK before map
111
101

✗ Invalid: Map before config complete
NO ./north.xpm
111  ← ERROR: Map before textures/colors
SO ./south.xpm

✗ Invalid: Empty line in map
111
    ← ERROR: Empty line inside map
101
```

---

### scene_config_ready() - Check Completeness

**File:** `src/parsing/scene_config.c`

```c
int	scene_config_ready(t_config *cfg)
{
	int	i;

	if (!cfg->floor_set || !cfg->ceiling_set)
		return (0);
	i = 0;
	while (i < TEX_COUNT)
	{
		if (!cfg->has_texture[i])
			return (0);
		i++;
	}
	return (1);
}
```

**Purpose:** Verify all required configuration is present.

**Checks:**
1. Floor color set
2. Ceiling color set
3. All 4 textures set (NO, SO, WE, EA)

**Return:**
- `1` if complete
- `0` if missing anything

**Configuration state tracking:**

```
Initial state:
config.floor_set = 0
config.ceiling_set = 0
config.has_texture[0-3] = {0, 0, 0, 0}

After "F 220,100,0":
config.floor_set = 1  ✓
config.ceiling_set = 0
config.has_texture = {0, 0, 0, 0}
→ scene_config_ready() = 0 (not ready)

After "C 135,206,250":
config.floor_set = 1  ✓
config.ceiling_set = 1  ✓
config.has_texture = {0, 0, 0, 0}
→ scene_config_ready() = 0 (not ready)

After all texture directives:
config.floor_set = 1  ✓
config.ceiling_set = 1  ✓
config.has_texture = {1, 1, 1, 1}  ✓
→ scene_config_ready() = 1 (READY!)
```

---

### finalize_map() - Building the Grid

**File:** `src/parsing/parse_map.c`

```c
int	finalize_map(t_map *map, char **lines, int count)
{
	int	width;

	if (count <= 0)
		return (print_error("Map section is empty"));
	width = get_max_width(lines, count);
	if (width == 0)
		return (print_error("Map width is zero"));
	map->grid = safe_malloc(sizeof(char *) * (count + 1));
	map->width = width;
	map->height = count;
	build_grid_rows(map, lines, width, count);
	map->grid[count] = NULL;
	return (0);
}
```

**Purpose:** Convert array of variable-length lines into rectangular grid.

**Problem to solve:**

Map lines from file often have different lengths:
```
Input lines:
"1111111"       (length 7)
"100001"        (length 6)
"10N1"          (length 4)
"1111111"       (length 7)
```

Need to pad to rectangle for consistent indexing:
```
Output grid (width=7):
"1111111"
"100001 "  ← padded with space
"10N1   "  ← padded with spaces
"1111111"
```

#### Step-by-Step Process

**1. Find maximum width**
```c
width = get_max_width(lines, count);
```

```c
static int	get_max_width(char **lines, int count)
{
	int	i;
	int	width;
	int	tmp;

	width = 0;
	i = 0;
	while (i < count)
	{
		tmp = row_width(lines[i]);
		if (tmp > width)
			width = tmp;
		i++;
	}
	return (width);
}
```

**Visual example:**
```
lines[0] = "1111111"  → width = 7
lines[1] = "100001"   → width = 6
lines[2] = "10N1"     → width = 4
lines[3] = "1111111"  → width = 7

max_width = 7
```

**2. Allocate grid array**
```c
map->grid = safe_malloc(sizeof(char *) * (count + 1));
```
- Allocate array of `count + 1` pointers (extra for NULL terminator)
- Each pointer will point to a row string

**3. Set dimensions**
```c
map->width = width;
map->height = count;
```

**4. Build and pad rows**
```c
build_grid_rows(map, lines, width, count);
```

```c
static void	build_grid_rows(t_map *map, char **lines, int width, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		map->grid[i] = safe_malloc(width + 1);
		fill_row(map->grid[i], lines[i], width);
		free(lines[i]);
		i++;
	}
}
```

**Row filling:**
```c
static void	fill_row(char *dst, const char *src, int width)
{
	int	i;
	int	src_len;

	src_len = 0;
	if (src)
	{
		while (src[src_len])
			src_len++;
	}
	i = 0;
	while (i < width)
	{
		if (i < src_len)
			dst[i] = src[i];
		else
			dst[i] = ' ';
		i++;
	}
	dst[width] = '\0';
}
```

**Visual transformation:**

```
Input:
lines[0] = "1111"
lines[1] = "10N1"
lines[2] = "11"

After get_max_width(): width = 4

After build_grid_rows():

map->grid[0] = ['1']['1']['1']['1']['\0']  ← exact fit
map->grid[1] = ['1']['0']['N']['1']['\0']  ← exact fit
map->grid[2] = ['1']['1'][' '][' ']['\0']  ← padded
map->grid[3] = NULL

Memory layout:
┌──────────────┐
│ grid ────┐   │
├──────────┼───┤
│ width: 4 │   │
│ height: 3│   │
└──────────┼───┘
           │
           ▼
    ┌───┬───┬───┬───┐
    │ * │ * │ * │ * │  ← array of 4 pointers
    └─┼─┴─┼─┴─┼─┴─┼─┘
      │   │   │   └──→ NULL
      │   │   │
      │   │   └──→ "11  \0"
      │   │
      │   └──→ "10N1\0"
      │
      └──→ "1111\0"
```

**5. NULL-terminate**
```c
map->grid[count] = NULL;
```
- Allows iteration: `while (map->grid[i]) { ... }`

---

### Player Setup - Finding Start Position

**File:** `src/parsing/player.c`

```c
int	setup_player(t_game *game)
{
	int	x;
	int	y;
	int	has_player;

	has_player = 0;
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (handle_cell(game, x, y, &has_player))
				return (1);
			x++;
		}
		y++;
	}
	if (!has_player)
		return (print_error("Missing player start"));
	return (0);
}
```

**Purpose:** 
1. Find player start position (`N`, `S`, `E`, `W`)
2. Set player position and orientation
3. Replace player character with `'0'` (walkable floor)

**Grid scan:**
```
Grid:                  Scan order:
1 1 1 1 1              (0,0) (1,0) (2,0) (3,0) (4,0)
1 0 0 0 1              (0,1) (1,1) (2,1) (3,1) (4,1)
1 0 N 0 1   →   →   →  (0,2) (1,2) (2,2) (3,2) (4,2)
1 0 0 0 1                           ↑
1 1 1 1 1              Found 'N' at (2,2)
```

#### handle_cell() - Process Each Character

```c
static int	handle_cell(t_game *game, int x, int y, int *has_player)
{
	char	c;

	c = game->map.grid[y][x];
	if (c == '0' || c == '1' || c == ' ')
		return (0);
	if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
	{
		if (*has_player)
			return (print_error("Multiple player positions"));
		*has_player = 1;
		game->player.x = x + 0.5;
		game->player.y = y + 0.5;
		set_orientation(&game->player, c);
		game->map.grid[y][x] = '0';
		return (0);
	}
	return (print_error("Invalid character in map"));
}
```

**Character handling:**

| Character | Action |
|-----------|--------|
| `'0'` | Walkable floor - ignore |
| `'1'` | Wall - ignore |
| `' '` | Void/space - ignore |
| `'N'/'S'/'E'/'W'` | Player start - set position |
| Anything else | Error: invalid character |

**Player position calculation:**
```c
game->player.x = x + 0.5;
game->player.y = y + 0.5;
```
- Center the player in the grid cell
- Grid uses integer coordinates
- Player uses floating-point for smooth movement

**Example:**
```
Grid cell (2, 3) contains 'N'

Cell boundaries:
  x: [2.0 to 3.0]
  y: [3.0 to 4.0]

Player position:
  x = 2 + 0.5 = 2.5  ← center
  y = 3 + 0.5 = 3.5  ← center

Visual:
     2.0         3.0
      ├───────────┤
3.0 ──┤           │
      │     •     │  ← player at (2.5, 3.5)
      │    N→     │
4.0 ──┤           │
      └───────────┘
```

**Multiple player check:**
```c
if (*has_player)
    return (print_error("Multiple player positions"));
```
- Prevents: `"1N01W01"` (two start positions)
- Ensures: exactly one player start

#### set_orientation() - Direction Vectors

```c
static void	set_orientation(t_player *player, char dir)
{
	if (dir == 'N')
	{
		player->dir_x = 0;
		player->dir_y = -1;
		player->plane_x = 0.66;
		player->plane_y = 0;
	}
	else if (dir == 'S')
	{
		player->dir_x = 0;
		player->dir_y = 1;
		player->plane_x = -0.66;
		player->plane_y = 0;
	}
	else if (dir == 'E')
	{
		player->dir_x = 1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = 0.66;
	}
	else if (dir == 'W')
	{
		player->dir_x = -1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = -0.66;
	}
}
```

**Direction vectors explained:**

The player has two vectors:
1. **Direction (`dir_x`, `dir_y`)**: Where the player faces
2. **Plane (`plane_x`, `plane_y`)**: Camera field of view (FOV)

**Coordinate system:**
```
        North (N)
      dir_y = -1
           ↑
           │
West ──────┼────── East (E)
(W)        │       dir_x = 1
dir_x = -1 │
           │
           ↓
      dir_y = 1
       South (S)
```

**North (N):**
```c
dir_x = 0, dir_y = -1      // Looking up/north
plane_x = 0.66, plane_y = 0 // FOV perpendicular (west-east)
```
```
     Plane (FOV)
    ←─────────→
    ↑         ↑
    │         │
    │    •    │  ← Player
    │    ↑    │
    │  Dir    │
    │         │
```

**East (E):**
```c
dir_x = 1, dir_y = 0
plane_x = 0, plane_y = 0.66
```
```
      Plane
        ↑
        │
        │
    •───→  ← Player & Direction
        │
        │
        ↓
```

**Why 0.66 for plane?**
- Creates ~66° field of view
- Standard for raycasting engines
- Matches human peripheral vision reasonably
- Formula: `tan(FOV/2) ≈ 0.66` for FOV ≈ 66°

**Grid replacement:**
```c
game->map.grid[y][x] = '0';
```
- Replaces `'N'` with `'0'` (floor)
- Player can walk over their start position
- Map becomes pure `0`/`1`/` ` grid

**Example transformation:**
```
Before setup_player():
1 1 1 1 1
1 0 0 0 1
1 0 N 0 1  ← 'N' at position (2,2)
1 0 0 0 1
1 1 1 1 1

After setup_player():
1 1 1 1 1
1 0 0 0 1
1 0 0 0 1  ← 'N' replaced with '0'
1 0 0 0 1
1 1 1 1 1

Player state:
  player.x = 2.5
  player.y = 2.5
  player.dir_x = 0
  player.dir_y = -1    (facing north)
  player.plane_x = 0.66
  player.plane_y = 0
```

---

### Scene Validation

**File:** `src/parsing/validate.c`

```c
int	validate_scene(t_game *game)
{
	if (validate_config(&game->config))
		return (1);
	if (setup_player(game))
		return (1);
	if (check_map_cells(&game->map))
		return (1);
	return (0);
}
```

**Purpose:** Final validation after parsing completes.

**Three-stage validation:**

```
1. validate_config()
   ├─ Check floor color set
   ├─ Check ceiling color set
   └─ Check all 4 textures set

2. setup_player()
   ├─ Find player start (N/S/E/W)
   ├─ Ensure exactly one player
   └─ Set position and orientation

3. check_map_cells()
   ├─ Validate all characters (0/1/space only after player removal)
   └─ Ensure map is enclosed (no holes in walls)
```

#### check_map_cells() - Wall Validation

```c
static int	check_map_cells(t_map *map)
{
	int	x;
	int	y;
	char	c;

	y = 0;
	while (y < map->height)
	{
		x = 0;
		while (x < map->width)
		{
			c = map->grid[y][x];
			if (c != '0' && c != '1' && c != ' ')
			{
				printf("Invalid character in map: '%c' (0x%x) at position [y=%d, x=%d]\n", 
					c, (unsigned char)c, y, x);
				return (print_error("Invalid character in map"));
			}
			if (c == '0' && check_neighbors(map, x, y))
				return (1);
			x++;
		}
		y++;
	}
	return (0);
}
```

**Checks:**
1. Only valid characters: `'0'`, `'1'`, `' '`
2. Every `'0'` (floor) must be enclosed by walls

**Invalid character detection:**
```c
if (c != '0' && c != '1' && c != ' ')
    return (print_error("Invalid character in map"));
```
- Catches: `'X'`, `'2'`, `'N'` (should have been replaced), etc.
- Debug printf shows exact position and hex value

#### check_neighbors() - Enclosure Validation

```c
static int	check_neighbors(t_map *map, int x, int y)
{
	int	dx;
	int	dy;

	if (x == 0 || y == 0 || x == map->width - 1 || y == map->height - 1)
	{
		printf("DEBUG: '0' at [y=%d, x=%d] is at map edge\n", y, x);
		return (print_error("Map not enclosed by walls"));
	}
	dy = -1;
	while (dy <= 1)
	{
		dx = -1;
		while (dx <= 1)
		{
			if ((dx != 0 || dy != 0)
				&& map->grid[y + dy][x + dx] == ' ')
			{
				printf("DEBUG: Open space found! '0' at [y=%d, x=%d] has space neighbor at [y=%d, x=%d]\n",
					y, x, y + dy, x + dx);
				return (print_error("Map has open space"));
			}
			dx++;
		}
		dy++;
	}
	return (0);
}
```

**Rule:** Every walkable cell (`'0'`) must be fully enclosed.

**Edge check:**
```c
if (x == 0 || y == 0 || x == map->width - 1 || y == map->height - 1)
    return (print_error("Map not enclosed by walls"));
```
- Floor can't be on map boundary
- Boundary must be walls or space

**Neighbor check:**
```
For cell at (x, y), check all 8 neighbors:

    (x-1,y-1) (x,y-1) (x+1,y-1)
    (x-1,y)   [CELL]  (x+1,y)
    (x-1,y+1) (x,y+1) (x+1,y+1)

If any neighbor is ' ' (void/space):
  → ERROR: "Map has open space"
```

**Valid map:**
```
1 1 1 1 1
1 0 0 0 1  ← '0' cells surrounded by '1'
1 0 0 0 1
1 0 0 0 1
1 1 1 1 1  ← Fully enclosed
```

**Invalid map - edge floor:**
```
0 1 1 1 1  ← ERROR: '0' at edge (x=0, y=0)
1 0 0 0 1
1 1 1 1 1
```

**Invalid map - open space:**
```
1 1 1 1 1
1 0 0 0 1
1 0 0 0    ← ERROR: '0' at (3,2) has space neighbor
1 1 1 1 1
```

**Why spaces are dangerous:**
- Space (`' '`) represents void (outside map)
- Player could "escape" through a hole
- Raycasting might shoot rays into undefined area

---

## Game Initialization

After successful parsing, the game engine must be initialized with graphics and resources.

**File:** `src/game/init.c`

### init_game() - Main Initialization

```c
int	init_game(t_game *game)
{
	int	i;

	ft_bzero(&game->input, sizeof(t_input));
	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error("mlx_init failed"));
	game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "cub3D");
	if (!game->win)
		return (print_error("Failed to create window"));
	if (create_frame(game))
		return (1);
	i = 0;
	while (i < TEX_COUNT)
	{
		if (load_texture(game, i, game->config.texture[i]))
			return (1);
		i++;
	}
	game->running = 1;
	return (0);
}
```

**Purpose:** Set up graphics system and load all resources.

**Initialization sequence:**

```
┌─────────────────────────────────────┐
│     init_game() sequence            │
├─────────────────────────────────────┤
│                                     │
│  1. Zero input state                │
│     └─ All movement flags = 0       │
│                                     │
│  2. Initialize MLX                  │
│     └─ Connect to X11 display       │
│                                     │
│  3. Create window                   │
│     └─ 1280x720 titled "cub3D"      │
│                                     │
│  4. Create frame buffer             │
│     └─ Off-screen drawing surface   │
│                                     │
│  5. Load textures (loop 0-3)        │
│     ├─ North wall texture           │
│     ├─ South wall texture           │
│     ├─ West wall texture            │
│     └─ East wall texture            │
│                                     │
│  6. Set running flag                │
│     └─ game->running = 1            │
│                                     │
│  Return 0 (success)                 │
└─────────────────────────────────────┘
```

#### Line-by-Line Breakdown

**Line 3: Declare counter**
```c
int	i;
```

**Line 5: Zero input state**
```c
ft_bzero(&game->input, sizeof(t_input));
```
- Ensures all movement flags start at 0
- `forward`, `backward`, `left`, `right`, `turn_left`, `turn_right` = 0
- Player won't move until keys pressed

**Lines 6-8: Initialize MLX**
```c
game->mlx = mlx_init();
if (!game->mlx)
    return (print_error("mlx_init failed"));
```

**What `mlx_init()` does:**
- Opens connection to X11 display server
- Allocates MLX context structure
- Sets up event handling infrastructure
- Returns pointer to context, or NULL on failure

**Failure cases:**
```bash
# No X11 display (SSH without X forwarding)
export DISPLAY=""
./cub3D map.cub
→ Error: mlx_init failed

# X11 server not running
./cub3D map.cub  # on console without X
→ Error: mlx_init failed
```

**Lines 9-11: Create window**
```c
game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "cub3D");
if (!game->win)
    return (print_error("Failed to create window"));
```

**Constants (from `cub3d.h`):**
```c
#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
```

**What `mlx_new_window()` does:**
- Creates X11 window with specified dimensions
- Sets window title to "cub3D"
- Returns window pointer, or NULL on failure

**Window characteristics:**
- Width: 1280 pixels
- Height: 720 pixels (720p resolution)
- Title: "cub3D"
- No fullscreen (windowed mode)
- Has standard window decorations (title bar, close button)

**Visual:**
```
┌──────────────────────────────────────────────┐
│ cub3D                              [_][□][X] │  ← Title bar
├──────────────────────────────────────────────┤
│                                              │
│                                              │
│            1280 x 720 pixels                 │
│         (rendering area)                     │
│                                              │
│                                              │
│                                              │
└──────────────────────────────────────────────┘
```

**Lines 12-13: Create frame buffer**
```c
if (create_frame(game))
    return (1);
```

Delegates to helper function (explained next).

**Lines 14-20: Load all textures**
```c
i = 0;
while (i < TEX_COUNT)
{
    if (load_texture(game, i, game->config.texture[i]))
        return (1);
    i++;
}
```

**Texture loading loop:**
```
TEX_COUNT = 4

i=0: load_texture(game, 0, config.texture[TEX_NO])
     → Load north wall texture

i=1: load_texture(game, 1, config.texture[TEX_SO])
     → Load south wall texture

i=2: load_texture(game, 2, config.texture[TEX_WE])
     → Load west wall texture

i=3: load_texture(game, 3, config.texture[TEX_EA])
     → Load east wall texture
```

**Texture indices (from `cub3d.h`):**
```c
#define TEX_NO 0  // North
#define TEX_SO 1  // South
#define TEX_WE 2  // West
#define TEX_EA 3  // East
#define TEX_COUNT 4
```

**Line 21: Set running flag**
```c
game->running = 1;
```
- Indicates initialization successful
- Game loop will process frames
- Can be set to 0 to trigger shutdown

**Line 22: Return success**
```c
return (0);
```

---

### create_frame() - Frame Buffer Setup

```c
static int	create_frame(t_game *game)
{
	game->frame.ptr = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
	if (!game->frame.ptr)
		return (print_error("Failed to create frame"));
	game->frame.addr = mlx_get_data_addr(game->frame.ptr, &game->frame.bpp,
			&game->frame.line_len, &game->frame.endian);
	if (!game->frame.addr)
		return (print_error("Frame data error"));
	game->frame.width = WIN_WIDTH;
	game->frame.height = WIN_HEIGHT;
	return (0);
}
```

**Purpose:** Create off-screen image buffer for rendering.

#### Why an off-screen buffer?

**Without buffer (direct drawing):**
```
Draw pixel 1 → visible immediately
Draw pixel 2 → visible immediately
...
Draw pixel 921,600 → visible immediately

Result: Player sees partial frames (tearing/flickering)
```

**With buffer (double buffering):**
```
1. Draw entire frame to off-screen buffer
2. When complete, copy buffer to window
3. Smooth, tear-free display
```

#### Line-by-Line Analysis

**Line 3: Create image**
```c
game->frame.ptr = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
```

**What `mlx_new_image()` does:**
- Allocates memory for 1280×720 image
- Each pixel: 32 bits (4 bytes: ARGB or BGRA)
- Total size: 1280 × 720 × 4 = 3,686,400 bytes (~3.5 MB)
- Returns opaque pointer to image structure

**Lines 4-5: Error check**
```c
if (!game->frame.ptr)
    return (print_error("Failed to create frame"));
```

**Lines 6-7: Get pixel data pointer**
```c
game->frame.addr = mlx_get_data_addr(game->frame.ptr, &game->frame.bpp,
        &game->frame.line_len, &game->frame.endian);
```

**What `mlx_get_data_addr()` returns:**
- `addr`: Direct pointer to pixel data array
- `bpp` (bits per pixel): Usually 32
- `line_len`: Bytes per row (may include padding)
- `endian`: Byte order (0=little, 1=big)

**Example values:**
```c
frame.addr = 0x7f8a4c000000  // Pointer to pixel buffer
frame.bpp = 32               // 32 bits = 4 bytes per pixel
frame.line_len = 5120        // 1280 pixels × 4 bytes = 5120 bytes/row
frame.endian = 0             // Little-endian (x86/x64)
```

**Memory layout:**
```
frame.addr points to:
┌────────────────────────────────────────┐
│ Row 0: [pixel 0][pixel 1]...[pixel 1279]│  5120 bytes
├────────────────────────────────────────┤
│ Row 1: [pixel 0][pixel 1]...[pixel 1279]│  5120 bytes
├────────────────────────────────────────┤
│ ...                                    │
├────────────────────────────────────────┤
│ Row 719: [pixel 0]...[pixel 1279]      │  5120 bytes
└────────────────────────────────────────┘

Each pixel: [B][G][R][A] (4 bytes on little-endian)
            or  [A][R][G][B] (depends on system)
```

**Lines 8-9: Error check**
```c
if (!game->frame.addr)
    return (print_error("Frame data error"));
```

**Lines 10-11: Store dimensions**
```c
game->frame.width = WIN_WIDTH;
game->frame.height = WIN_HEIGHT;
```
- Redundant with constants, but convenient for bounds checking
- Used in `draw_pixel()` to prevent out-of-bounds writes

---

### load_texture() - XPM File Loading

```c
static int	load_texture(t_game *game, int idx, char *path)
{
	t_img	*tex;

	tex = &game->texture[idx];
	tex->ptr = mlx_xpm_file_to_image(game->mlx, path,
			&tex->width, &tex->height);
	if (!tex->ptr)
		return (print_error("Failed to load texture"));
	tex->addr = mlx_get_data_addr(tex->ptr, &tex->bpp,
			&tex->line_len, &tex->endian);
	if (!tex->addr)
		return (print_error("Texture data error"));
	return (0);
}
```

**Purpose:** Load wall texture from XPM file.

**Parameters:**
- `game`: Game state
- `idx`: Texture slot (0=NO, 1=SO, 2=WE, 3=EA)
- `path`: File path from config (e.g., `"./textures/north.xpm"`)

#### XPM Format

**XPM (X PixMap)** is a simple text-based image format.

**Example XPM file:**
```c
/* north.xpm - 4x4 texture */
static char *xpm[] = {
"4 4 2 1",         // width height colors chars_per_pixel
". c #FF0000",     // '.' = red
"# c #0000FF",     // '#' = blue
"....",            // Row 0
"#..#",            // Row 1
"#..#",            // Row 2
"####"             // Row 3
};
```

**Why XPM?**
- Simple format (no external libraries needed)
- MiniLibX has built-in XPM support
- Easy to create/edit
- Transparency support

#### Line-by-Line Analysis

**Line 3: Get texture slot**
```c
t_img	*tex;
```

**Line 5: Point to correct slot**
```c
tex = &game->texture[idx];
```

**Example:**
```c
idx = 0 (TEX_NO)
tex = &game->texture[0]  // North wall texture slot
```

**Lines 6-7: Load XPM file**
```c
tex->ptr = mlx_xpm_file_to_image(game->mlx, path,
        &tex->width, &tex->height);
```

**What `mlx_xpm_file_to_image()` does:**
1. Opens file at `path`
2. Parses XPM format
3. Allocates image structure
4. Converts to pixel data
5. Returns image pointer
6. Writes image dimensions to `width` and `height`

**Example:**
```c
path = "./textures/north.xpm"
→ Opens and parses file
→ Creates 64×64 texture (typical size)
→ tex->width = 64
→ tex->height = 64
```

**Lines 8-9: Error check**
```c
if (!tex->ptr)
    return (print_error("Failed to load texture"));
```

**Common errors:**
- File not found
- Invalid XPM format
- Permission denied
- Out of memory

**Lines 10-11: Get pixel data**
```c
tex->addr = mlx_get_data_addr(tex->ptr, &tex->bpp,
        &tex->line_len, &tex->endian);
```

Same as frame buffer - gets direct pointer to pixel array.

**Typical texture values:**
```c
tex->width = 64
tex->height = 64
tex->bpp = 32
tex->line_len = 256  // 64 pixels × 4 bytes
tex->addr = 0x7f8a4d000000
```

**Lines 12-13: Error check**
```c
if (!tex->addr)
    return (print_error("Texture data error"));
```

**Line 14: Return success**
```c
return (0);
```

#### Texture Memory Layout

```
After loading all 4 textures:

game->texture[0] (North):
  ptr → MLX image
  addr → [64×64 pixel array]
  width = 64, height = 64

game->texture[1] (South):
  ptr → MLX image
  addr → [64×64 pixel array]
  width = 64, height = 64

game->texture[2] (West):
  ptr → MLX image
  addr → [64×64 pixel array]
  width = 64, height = 64

game->texture[3] (East):
  ptr → MLX image
  addr → [64×64 pixel array]
  width = 64, height = 64

Total texture memory: 4 × 64 × 64 × 4 = 65,536 bytes (~64 KB)
```

---

### destroy_game() - Cleanup

```c
void	destroy_game(t_game *game)
{
	if (game->frame.ptr)
		mlx_destroy_image(game->mlx, game->frame.ptr);
	destroy_textures(game);
	if (game->win)
		mlx_destroy_window(game->mlx, game->win);
	if (game->mlx)
	{
#ifdef __linux__
		mlx_destroy_display(game->mlx);
#endif
		free(game->mlx);
	}
	free_map(&game->map);
	free_config(&game->config);
}
```

**Purpose:** Free all allocated resources in reverse order of creation.

**Cleanup order:**

```
┌─────────────────────────────────────┐
│   destroy_game() sequence           │
├─────────────────────────────────────┤
│                                     │
│  1. Destroy frame buffer            │
│     └─ Free 3.5 MB image            │
│                                     │
│  2. Destroy textures (×4)           │
│     └─ Free ~64 KB total            │
│                                     │
│  3. Destroy window                  │
│     └─ Close X11 window             │
│                                     │
│  4. Destroy MLX display/context     │
│     ├─ Linux: mlx_destroy_display() │
│     └─ Free MLX structure           │
│                                     │
│  5. Free map grid                   │
│     └─ Free each row + array        │
│                                     │
│  6. Free config data                │
│     └─ Free texture path strings    │
│                                     │
└─────────────────────────────────────┘
```

**Why check before freeing?**
```c
if (game->frame.ptr)
    mlx_destroy_image(game->mlx, game->frame.ptr);
```

- Partial initialization might have failed
- Some pointers might still be NULL
- Safe to check: `if (NULL)` is false, skips free
- Prevents double-free or freeing invalid pointers

**Platform-specific cleanup:**
```c
#ifdef __linux__
    mlx_destroy_display(game->mlx);
#endif
```
- Only needed on Linux (X11 display connection)
- macOS version doesn't need this
- Conditional compilation for portability

#### destroy_textures() Helper

```c
static void	destroy_textures(t_game *game)
{
	int	i;

	i = 0;
	while (i < TEX_COUNT)
	{
		if (game->texture[i].ptr)
			mlx_destroy_image(game->mlx, game->texture[i].ptr);
		game->texture[i].ptr = NULL;
		i++;
	}
}
```

**Purpose:** Free all 4 texture images.

**Loop through textures:**
```
i=0: Destroy texture[0] (North)
i=1: Destroy texture[1] (South)
i=2: Destroy texture[2] (West)
i=3: Destroy texture[3] (East)
```

**Set to NULL after freeing:**
```c
game->texture[i].ptr = NULL;
```
- Prevents accidental reuse
- Safe if `destroy_game()` called multiple times
- Good defensive programming

---

## Event Handling System

The event system handles user input and the main game loop. All event handlers are in `src/hooks/events.c`.

### Input State Management

**Key concept:** Use **state flags** instead of direct actions.

**Why flags?**
```
❌ Direct action (bad):
KeyPress W → move player immediately
Problem: Only moves once per key press

✓ State flags (good):
KeyPress W → set input.forward = 1
Game loop → check input.forward
           → if 1, move player
           → repeat every frame
KeyRelease W → set input.forward = 0
```

**Input structure:**
```c
typedef struct s_input
{
    int	forward;    // W key
    int	backward;   // S key
    int	left;       // A key
    int	right;      // D key
    int	turn_left;  // Left arrow
    int	turn_right; // Right arrow
}	t_input;
```

**State machine:**
```
Initial: All flags = 0 (no movement)

W pressed  → forward = 1
           → backward, left, right = 0

W+D pressed → forward = 1, right = 1
            → Diagonal movement

W released  → forward = 0
            → right still 1 (only D pressed)
```

---

### handle_key_press() - Key Down Events

**File:** `src/hooks/events.c`

```c
int	handle_key_press(int keycode, void *param)
{
	t_game	*game;

	game = (t_game *)param;
	if (keycode == KEY_ESC)
		return (handle_close(param));
	set_movement_flag(&game->input, keycode, 1);
	return (0);
}
```

**Purpose:** Process keyboard key press events.

**Parameters:**
- `keycode`: X11 key code (integer representing which key)
- `param`: User data (our `game` pointer)

**Return:** Always `0` (event handled)

#### Line-by-Line Analysis

**Line 3: Cast parameter**
```c
t_game	*game;
```

**Line 5: Extract game pointer**
```c
game = (t_game *)param;
```
- MLX passes `void *` to maintain generic callback signature
- Cast back to `t_game *` to access our data

**Lines 6-7: Handle ESC key**
```c
if (keycode == KEY_ESC)
    return (handle_close(param));
```

**Key codes (from `cub3d.h`):**
```c
#define KEY_ESC 65307  // X11 code for Escape
```

- ESC triggers immediate shutdown
- Calls `handle_close()` which exits program
- Return value doesn't matter (program exits)

**Line 8: Set movement flag**
```c
set_movement_flag(&game->input, keycode, 1);
```
- Sets appropriate flag to `1` (active)
- Helper function handles key mapping

**Line 9: Return success**
```c
return (0);
```

---

### handle_key_release() - Key Up Events

```c
int	handle_key_release(int keycode, void *param)
{
	t_game	*game;

	game = (t_game *)param;
	set_movement_flag(&game->input, keycode, 0);
	return (0);
}
```

**Purpose:** Process keyboard key release events.

**Difference from press:**
- No ESC check (only matters on press)
- Sets flag to `0` (inactive) instead of `1`

**Flow:**
```
Key press → handle_key_press() → set_movement_flag(..., 1)
           → flag = 1

Key held → (no events)
          → flag stays 1
          → game loop keeps moving

Key release → handle_key_release() → set_movement_flag(..., 0)
             → flag = 0
             → movement stops
```

---

### set_movement_flag() - Key Mapping

```c
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
```

**Purpose:** Map key codes to input flags.

**Key mappings (from `cub3d.h`):**
```c
#define KEY_W 119      // Forward
#define KEY_A 97       // Strafe left
#define KEY_S 115      // Backward
#define KEY_D 100      // Strafe right
#define KEY_LEFT 65361 // Turn left
#define KEY_RIGHT 65363// Turn right
```

**Movement controls:**

| Key | Code | Flag        | Action        |
|-----|------|-------------|---------------|
| W | 119    | `forward`   | Move forward  |
| S | 115    | `backward`  | Move backward |
| A | 97     | `left`      | Strafe left   |
| D | 100    | `right`     | Strafe right  |
| ← | 65361  | `turn_left` | Rotate left   |
| → | 65363  | `turn_right`| Rotate right  |

**Example execution:**

```c
// User presses W
set_movement_flag(&game->input, 119, 1)
→ keycode == KEY_W (119)
→ input->forward = 1

// User presses D (while holding W)
set_movement_flag(&game->input, 100, 1)
→ keycode == KEY_D (100)
→ input->right = 1
→ input->forward still = 1

// User releases W
set_movement_flag(&game->input, 119, 0)
→ input->forward = 0
→ input->right still = 1 (still holding D)
```

**Unrecognized keys:**
```c
// User presses Space (keycode = 32)
set_movement_flag(&game->input, 32, 1)
→ No matching if statement
→ Function does nothing
→ No flag changed
```

---

### handle_close() - Window Close

```c
int	handle_close(void *param)
{
	t_game	*game;

	game = (t_game *)param;
	game->running = 0;
	destroy_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}
```

**Purpose:** Handle window close events (X button or ESC key).

**Triggered by:**
1. User clicks window X button
2. User presses ESC key
3. Window manager sends close request

#### Shutdown Sequence

**Line 5: Extract game pointer**
```c
game = (t_game *)param;
```

**Line 6: Set running flag**
```c
game->running = 0;
```
- Signals that game should stop
- Game loop checks this and can skip rendering

**Line 7: Free resources**
```c
destroy_game(game);
```
- Destroys images and textures
- Closes window
- Frees MLX context
- Frees map and config data

**Line 8: Exit program**
```c
exit(EXIT_SUCCESS);
```
- `EXIT_SUCCESS` = 0
- Terminates entire program immediately
- No return to `mlx_loop()` in `main()`

**Line 9: Never reached**
```c
return (0);
```
- Required by function signature
- Never executed (exit() doesn't return)

**Why `exit()` instead of returning?**

```c
// If we just returned:
int handle_close() {
    destroy_game(game);
    return (0);
}
// Problem: Control returns to mlx_loop()
// → Loop continues running
// → Tries to access destroyed window
// → Crash or undefined behavior

// With exit():
int handle_close() {
    destroy_game(game);
    exit(0);
}
// → Program terminates cleanly
// → No chance of use-after-free
```

---

## Game Loop & Updates

The game loop is the heart of the engine, running continuously to update game state and render frames.

**File:** `src/hooks/events.c` and `src/game/update.c`

### game_loop() - Main Loop Tick

**File:** `src/hooks/events.c`

```c
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
```

**Purpose:** Called every frame by MLX to update and render.

**Execution frequency:**
- Called by `mlx_loop_hook()`
- Runs continuously (no frame limit)
- Typical: 60-1000+ times per second

#### Frame Timeline

```
Frame N:
  ┌─────────────────────┐
  │ game_loop() called  │
  ├─────────────────────┤
  │ Check running flag  │
  │ Update game state   │
  │ Render frame        │
  └─────────────────────┘
  
Frame N+1:
  ┌─────────────────────┐
  │ game_loop() called  │
  ├─────────────────────┤
  │ Check running flag  │
  │ Update game state   │
  │ Render frame        │
  └─────────────────────┘
  
... (continues until window closed)
```

#### Line-by-Line Analysis

**Lines 3-5: Extract game pointer**
```c
t_game	*game;

game = (t_game *)param;
```

**Lines 6-7: Check running flag**
```c
if (!game->running)
    return (0);
```
- Early exit if game should stop
- Set by `handle_close()`
- Prevents rendering after cleanup (defensive)

**Line 8: Update physics/logic**
```c
update_game_state(game);
```
- Reads input flags
- Moves player
- Rotates camera
- Collision detection

**Line 9: Render graphics**
```c
render_frame(game);
```
- Draws background (floor/ceiling)
- Raycasts 1280 vertical slices
- Draws textured walls
- Displays frame to window

**Line 10: Return**
```c
return (0);
```
- MLX ignores return value
- Must return int for callback signature

---

### update_game_state() - Physics Update

**File:** `src/game/update.c`

```c
void	update_game_state(t_game *game)
{
	update_position(game);
	apply_rotation(game);
}
```

**Purpose:** Update player position and orientation based on input.

**Two-stage update:**
1. **Translation:** Move player forward/backward/strafe
2. **Rotation:** Turn camera left/right

**Why separate?**
- Movement and rotation are independent
- Can move and rotate simultaneously
- Cleaner code organization

---

### update_position() - Player Movement

```c
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
```

**Purpose:** Calculate movement offsets and call collision-checked movement.

**Movement speed (from `cub3d.h`):**
```c
#define MOVE_SPEED 0.01
```
- Small value for smooth, frame-independent movement
- 0.01 units per frame
- At 60 FPS: 0.6 units/second

#### Movement Directions

**Forward/Backward:**
```c
if (game->input.forward)
    move_player(game, dir_x * speed, dir_y * speed);
if (game->input.backward)
    move_player(game, -dir_x * speed, -dir_y * speed);
```

**Direction vector:**
```
Player facing North:
  dir_x = 0, dir_y = -1
  
Forward movement (W):
  offset_x = 0 * 0.01 = 0
  offset_y = -1 * 0.01 = -0.01
  → Move north (y decreases)

Backward movement (S):
  offset_x = -0 * 0.01 = 0
  offset_y = -(-1) * 0.01 = 0.01
  → Move south (y increases)
```

**Strafe Left/Right:**
```c
if (game->input.right)
    move_player(game, plane_x * speed, plane_y * speed);
if (game->input.left)
    move_player(game, -plane_x * speed, -plane_y * speed);
```

**Plane vector (perpendicular to direction):**
```
Player facing North:
  dir = (0, -1)      ← Looking north
  plane = (0.66, 0)  ← Perpendicular (west-east)

Strafe right (D):
  offset_x = 0.66 * 0.01 = 0.0066
  offset_y = 0 * 0.01 = 0
  → Move east (x increases)

Strafe left (A):
  offset_x = -0.66 * 0.01 = -0.0066
  offset_y = 0 * 0.01 = 0
  → Move west (x decreases)
```

**Diagonal movement example:**
```
W + D pressed:
  1. Check forward: move by (dir_x * 0.01, dir_y * 0.01)
  2. Check right: move by (plane_x * 0.01, plane_y * 0.01)
  
Result: Player moves forward-right at ~1.41× speed
(This is normal; realistic diagonal movement)
```

---

### move_player() - Collision Detection

```c
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
```

**Purpose:** Move player with wall collision.

**Key feature: Separate X and Y checks**

**Why check separately?**
```
Wall sliding example:

Player at (2.5, 2.5) wants to move diagonally:
  offset_x = 0.01
  offset_y = 0.01

Scenario: Wall to the north, open to the east

Check X movement (2.5 + 0.01, 2.5):
  → Not blocked
  → player.x = 2.51

Check Y movement (2.51, 2.5 + 0.01):
  → Blocked by wall
  → player.y stays 2.5

Result: Player slides along the wall horizontally
```

**Sliding visualization:**
```
Before:          After:
1 1 1 1 1        1 1 1 1 1
1 0 0 0 1        1 0 0 0 1
1 0 • 0 1   →    1 0 0 • 1  ← Slid east
  Player           Player moved despite
  tries NE         north being blocked
```

---

### is_blocked() - Wall Detection

```c
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
```

**Purpose:** Check if position is inside a wall or out of bounds.

**Process:**
1. Convert float coordinates to grid indices
2. Check bounds
3. Check map cell type

**Coordinate conversion:**
```c
grid_x = (int)x;
grid_y = (int)y;
```

**Example:**
```
Player at (2.75, 3.21)
→ grid_x = 2
→ grid_y = 3
→ Check map.grid[3][2]
```

**Bounds check:**
```c
if (grid_x < 0 || grid_y < 0 || 
    grid_x >= game->map.width || 
    grid_y >= game->map.height)
    return (1);
```
- Prevents array out-of-bounds
- Treats outside map as blocked

**Wall check:**
```c
if (game->map.grid[grid_y][grid_x] == '1' || 
    game->map.grid[grid_y][grid_x] == ' ')
    return (1);
```
- `'1'` = wall (solid)
- `' '` = void/space (treat as wall)
- `'0'` = floor (walkable)

**Return:**
- `1` = blocked (wall or out of bounds)
- `0` = free (can move)

---

### apply_rotation() - Camera Rotation

**File:** `src/game/update.c`

```c
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
```

**Purpose:** Rotate camera based on arrow key input.

**Rotation speed (from `cub3d.h`):**
```c
#define ROT_SPEED 0.02  // ~1.15° per frame
```

#### 2D Rotation Matrix

**Mathematical basis:**
```
To rotate vector (x, y) by angle θ:

x' = x·cos(θ) - y·sin(θ)
y' = x·sin(θ) + y·cos(θ)
```

**Why this works:**
- Standard 2D rotation transformation
- Preserves vector length
- Rotates around origin

#### Line-by-Line Analysis

**Line 3: Calculate rotation angle**
```c
double	angle;
```

**Lines 7-10: Determine rotation direction**
```c
angle = 0.0;
if (game->input.turn_left)
    angle -= ROT_SPEED;  // Negative = counter-clockwise
if (game->input.turn_right)
    angle += ROT_SPEED;  // Positive = clockwise
```

**Possible values:**
- `0.0` = no rotation (neither arrow pressed)
- `-0.02` = turning left
- `+0.02` = turning right
- `0.0` = both arrows pressed (cancel out - shouldn't happen but safe)

**Lines 11-12: Early exit optimization**
```c
if (angle == 0.0)
    return ;
```
- Skip expensive trig calculations if not rotating
- Common case (player not always turning)

**Lines 13-14: Save original values**
```c
old_dir_x = game->player.dir_x;
old_plane_x = game->player.plane_x;
```
- Need original values for rotation formula
- Will be overwritten during calculation

**Lines 15-18: Rotate direction vector**
```c
game->player.dir_x = game->player.dir_x * cos(angle)
    - game->player.dir_y * sin(angle);
game->player.dir_y = old_dir_x * sin(angle)
    + game->player.dir_y * cos(angle);
```

**Example rotation (facing North, turn right):**
```
Before:
  dir_x = 0.0
  dir_y = -1.0
  angle = +0.02 (right turn)

Calculate:
  cos(0.02) ≈ 0.9998
  sin(0.02) ≈ 0.02

New dir_x:
  = 0.0 * 0.9998 - (-1.0) * 0.02
  = 0 + 0.02
  = 0.02

New dir_y:
  = 0.0 * 0.02 + (-1.0) * 0.9998
  = 0 - 0.9998
  = -0.9998

After:
  dir_x = 0.02   (slightly east)
  dir_y = -0.9998 (mostly north)
  → Facing slightly northeast
```

**Lines 19-22: Rotate plane vector**
```c
game->player.plane_x = game->player.plane_x * cos(angle)
    - game->player.plane_y * sin(angle);
game->player.plane_y = old_plane_x * sin(angle)
    + game->player.plane_y * cos(angle);
```

**Why rotate plane too?**
- Plane defines field of view direction
- Must stay perpendicular to direction vector
- Both rotate together to maintain camera orientation

**Visual:**
```
Before rotation (facing North):
        ↑ dir
        │
   ─────┼───── plane
        │
        •
      player

After rotation (turned right):
        ┌ dir
       ╱
   ───╱────── plane rotated too
     ╱
    •
  player
```

---

## Rendering Pipeline

The rendering system uses **raycasting** to create a 3D perspective from the 2D map.

### Raycasting Overview

**Core concept:** Cast one ray per screen column to determine what to draw.

```
Screen (1280 columns):
┌────────────────────────────┐
│║║║║║║║║║║║║║║║║║║║║║║║║║║║║│
│║║║║║║║║║║║║║║║║║║║║║║║║║║║║│  Each vertical line is one ray
│║║║║║║║║║║║║║║║║║║║║║║║║║║║║│
└────────────────────────────┘
 ↑               ↑           ↑
Ray 0         Ray 640    Ray 1279
```

**Process per ray:**
1. Calculate ray direction
2. Step through grid until hitting wall
3. Calculate wall distance
4. Determine wall height on screen
5. Select texture and draw column

---

### render_frame() - Main Rendering

**File:** `src/render/raycast.c`

```c
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
```

**Purpose:** Render complete frame and display it.

**Three stages:**

```
1. render_background()
   └─ Draw solid floor and ceiling colors

2. Loop through columns (x = 0 to 1279)
   └─ render_column() for each

3. mlx_put_image_to_window()
   └─ Copy frame buffer to window
```

**Why background first?**
- Walls only partially fill screen (vertical slices)
- Background fills areas not covered by walls
- Simpler than drawing floor/ceiling per-pixel

**Line 5: Draw background**
```c
render_background(game);
```

**Lines 6-11: Draw walls**
```c
x = 0;
while (x < WIN_WIDTH)
{
    render_column(game, x);
    x++;
}
```
- 1280 iterations (one per pixel column)
- Each column is independent raycast

**Line 12: Display frame**
```c
mlx_put_image_to_window(game->mlx, game->win, game->frame.ptr, 0, 0);
```
- Copies entire frame buffer to window at position (0, 0)
- Makes rendering visible to user
- Double-buffering prevents tearing

---

### render_background() - Floor/Ceiling

**File:** `src/render/background.c`

```c
void	render_background(t_game *game)
{
	int	x;
	int	y;
	int	ceiling;
	int	floor;

	ceiling = rgb_to_int(game->config.ceiling);
	floor = rgb_to_int(game->config.floor);
	y = 0;
	while (y < WIN_HEIGHT)
	{
		x = 0;
		while (x < WIN_WIDTH)
		{
			if (y < WIN_HEIGHT / 2)
				draw_pixel(&game->frame, x, y, ceiling);
			else
				draw_pixel(&game->frame, x, y, floor);
			x++;
		}
		y++;
	}
}
```

**Purpose:** Fill frame buffer with solid colors for floor and ceiling.

**Simple algorithm:**
```
For each pixel in 1280×720 frame:
  If y < 360 (top half):
    Draw ceiling color
  Else (bottom half):
    Draw floor color
```

**Visual result:**
```
┌────────────────────────────┐
│                            │ ← Ceiling color
│     SKY (top half)         │   (e.g., light blue)
│                            │
├────────────────────────────┤ ← y = 360 (horizon)
│                            │
│   FLOOR (bottom half)      │ ← Floor color
│                            │   (e.g., brown)
└────────────────────────────┘
```

**Color conversion:**
```c
ceiling = rgb_to_int(game->config.ceiling);
floor = rgb_to_int(game->config.floor);
```

Converts RGB structure to 32-bit integer:
```c
int	rgb_to_int(t_color color)
{
    return ((color.r & 0xFF) << 16)
        | ((color.g & 0xFF) << 8)
        | (color.b & 0xFF);
}
```

**Example:**
```
config.ceiling = {135, 206, 250}  // Sky blue
→ (135 << 16) | (206 << 8) | 250
→ 0x87CEFA

config.floor = {220, 100, 0}  // Brown
→ (220 << 16) | (100 << 8) | 0
→ 0xDC6400
```

---

### render_column() - Single Raycast

**File:** `src/render/raycast.c`

```c
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
```

**Purpose:** Cast one ray and draw resulting wall slice.

**Process:**
1. Initialize ray for column `x`
2. Set up stepping direction
3. Step until wall hit
4. Draw wall column

---

### ray_init() - Ray Setup

**File:** `src/render/ray_setup.c`

```c
void	ray_init(t_game *game, t_ray *ray, int x)
{
	double	camera_x;

	camera_x = 2.0 * x / (double)WIN_WIDTH - 1.0;
	ray->dir_x = game->player.dir_x + game->player.plane_x * camera_x;
	ray->dir_y = game->player.dir_y + game->player.plane_y * camera_x;
	ray->map_x = (int)game->player.x;
	ray->map_y = (int)game->player.y;
	if (ray->dir_x == 0)
		ray->delta_dist_x = 1e30;
	else
		ray->delta_dist_x = fabs(1.0 / ray->dir_x);
	if (ray->dir_y == 0)
		ray->delta_dist_y = 1e30;
	else
		ray->delta_dist_y = fabs(1.0 / ray->dir_y);
}
```

**Purpose:** Calculate ray direction and stepping distances.

#### Camera X Calculation

**Line 5:**
```c
camera_x = 2.0 * x / (double)WIN_WIDTH - 1.0;
```

**Maps screen column to camera space:**
```
x = 0       → camera_x = 2*0/1280 - 1 = -1.0 (left edge)
x = 640     → camera_x = 2*640/1280 - 1 = 0.0 (center)
x = 1279    → camera_x = 2*1279/1280 - 1 ≈ 0.998 (right edge)
```

**Camera space:**
```
      -1.0           0.0          +1.0
       │             │             │
   ────┼─────────────┼─────────────┼────
  Left            Center        Right
  edge           (player)        edge
```

#### Ray Direction Calculation

**Lines 6-7:**
```c
ray->dir_x = game->player.dir_x + game->player.plane_x * camera_x;
ray->dir_y = game->player.dir_y + game->player.plane_y * camera_x;
```

**Formula breakdown:**
```
ray_direction = player_direction + camera_plane * camera_x
```

**Example (player facing North, x=0 leftmost ray):**
```
player.dir = (0, -1)       // Facing north
player.plane = (0.66, 0)   // FOV plane (west-east)
camera_x = -1.0            // Leftmost column

ray.dir_x = 0 + 0.66 * (-1.0) = -0.66
ray.dir_y = -1 + 0 * (-1.0) = -1.0

→ Ray points northwest
```

**Visual (top-down view):**
```
        ╱ Ray x=0 (northwest)
       ╱
      │ Ray x=640 (north, center)
     ╱
    •  Player
   ╱
  ╱
 ╱ Ray x=1279 (northeast)
```

#### Grid Position

**Lines 8-9:**
```c
ray->map_x = (int)game->player.x;
ray->map_y = (int)game->player.y;
```

**Starting grid cell:**
```
Player at (2.7, 3.4)
→ map_x = 2
→ map_y = 3
→ Ray starts in grid cell [3][2]
```

#### Delta Distance

**Lines 10-17:**
```c
if (ray->dir_x == 0)
    ray->delta_dist_x = 1e30;
else
    ray->delta_dist_x = fabs(1.0 / ray->dir_x);
if (ray->dir_y == 0)
    ray->delta_dist_y = 1e30;
else
    ray->delta_dist_y = fabs(1.0 / ray->dir_y);
```

**Purpose:** Calculate how far ray travels to cross one grid line.

**Delta distance formula:**
```
delta_dist_x = |1 / dir_x|
delta_dist_y = |1 / dir_y|
```

**Geometric meaning:**
```
If ray has dir_x = 0.5:
  → Moves 0.5 units in X per 1 unit along ray
  → To move 1 full grid square in X:
    delta_dist_x = 1 / 0.5 = 2.0 ray units

If ray has dir_y = 0:
  → Ray is perfectly horizontal
  → Never crosses Y grid lines
  → delta_dist_y = infinity (1e30)
```

**Example:**
```
Ray direction: (0.6, 0.8)

delta_dist_x = |1 / 0.6| = 1.667
delta_dist_y = |1 / 0.8| = 1.25

Visualization:
       Y
       │
   ────┼────┼────┼──── X
       │    ╱    │
       │   ╱ Ray │
       │  ╱      │
       │ ╱       │
       │•        │
    Player
    
Ray crosses Y gridlines every 1.25 units
Ray crosses X gridlines every 1.667 units
```

---

### ray_set_step() - Stepping Setup

**File:** `src/render/ray_setup.c`

```c
void	ray_set_step(t_game *game, t_ray *ray)
{
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (game->player.x - ray->map_x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - game->player.x)
			* ray->delta_dist_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (game->player.y - ray->map_y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - game->player.y)
			* ray->delta_dist_y;
	}
}
```

**Purpose:** Determine step direction and initial side distances.

#### Step Direction

**Lines 3-5 and 9-11:**
```c
if (ray->dir_x < 0)
    ray->step_x = -1;  // Moving west
else
    ray->step_x = 1;   // Moving east
```

**Step values:**
- `-1` = step backward (decrease index)
- `+1` = step forward (increase index)

#### Side Distance

**What is side_dist?**
Distance from player to next grid line in that direction.

**Negative direction (west/north):**
```c
ray->side_dist_x = (game->player.x - ray->map_x) * ray->delta_dist_x;
```

**Example:**
```
Player at x = 2.7, in grid cell 2
→ Distance to left edge: 2.7 - 2.0 = 0.7
→ If delta_dist_x = 1.5
→ side_dist_x = 0.7 * 1.5 = 1.05 ray units
```

**Positive direction (east/south):**
```c
ray->side_dist_x = (ray->map_x + 1.0 - game->player.x) * ray->delta_dist_x;
```

**Example:**
```
Player at x = 2.7, in grid cell 2
→ Distance to right edge: 3.0 - 2.7 = 0.3
→ If delta_dist_x = 1.5
→ side_dist_x = 0.3 * 1.5 = 0.45 ray units
```

**Visual:**
```
Grid cell boundaries:
    2.0         3.0         4.0
     │           │           │
     │    •      │           │  Player at (2.7, y)
     │ ←─┼─→     │           │
     │ 0.7│0.3   │           │
     │           │           │
     
Ray going west (-):
  side_dist_x = 0.7 * delta_dist_x

Ray going east (+):
  side_dist_x = 0.3 * delta_dist_x
```

---

### ray_step_until_hit() - DDA Algorithm

**File:** `src/render/ray_setup.c`

```c
int	ray_step_until_hit(t_game *game, t_ray *ray)
{
	int	status;

	status = ray_advance_cell(game, ray);
	while (status == 0)
		status = ray_advance_cell(game, ray);
	if (status < 0)
		return (-1);
	if (ray->side == 0)
		ray->perp_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_dist = ray->side_dist_y - ray->delta_dist_y;
	return (0);
}
```

**Purpose:** Step through grid using DDA (Digital Differential Analysis) until hitting a wall.

**DDA Algorithm:**
```
Repeat:
  1. Find which grid line is closer (X or Y)
  2. Step to that grid line
  3. Check if hit wall
Until: Wall found or error
```

#### ray_advance_cell() - Single Step

```c
static int	ray_advance_cell(t_game *game, t_ray *ray)
{
	if (ray->side_dist_x < ray->side_dist_y)
	{
		ray->side_dist_x += ray->delta_dist_x;
		ray->map_x += ray->step_x;
		ray->side = 0;
	}
	else
	{
		ray->side_dist_y += ray->delta_dist_y;
		ray->map_y += ray->step_y;
		ray->side = 1;
	}
	if (ray->map_y < 0 || ray->map_y >= game->map.height
		|| ray->map_x < 0 || ray->map_x >= game->map.width)
		return (-1);
	if (game->map.grid[ray->map_y][ray->map_x] == '1')
		return (1);
	return (0);
}
```

**Return values:**
- `0` = empty space, keep stepping
- `1` = hit wall, stop
- `-1` = out of bounds, error

**Step logic:**
```c
if (ray->side_dist_x < ray->side_dist_y)
{
    // X gridline is closer
    ray->side_dist_x += ray->delta_dist_x;  // Move to next X line
    ray->map_x += ray->step_x;              // Change grid cell
    ray->side = 0;                          // Hit vertical wall
}
else
{
    // Y gridline is closer
    ray->side_dist_y += ray->delta_dist_y;  // Move to next Y line
    ray->map_y += ray->step_y;              // Change grid cell
    ray->side = 1;                          // Hit horizontal wall
}
```

**Visual example:**
```
Starting: Player at (2.3, 2.5) in cell [2][2]
Ray direction: Northeast

Step 1:
  side_dist_x = 1.2
  side_dist_y = 0.8  ← Smaller, step Y first
  → Move to cell [2][3] (north)
  → side = 1 (horizontal wall)

Step 2:
  side_dist_x = 1.2  ← Now smaller
  side_dist_y = 2.0
  → Move to cell [3][3] (east)
  → side = 0 (vertical wall)

Step 3:
  Check grid[3][3] = '1' (wall!)
  → Return 1 (hit)

Result:
  Ray hit vertical wall (side=0) at cell [3][3]
```

**Perpendicular distance calculation:**
```c
if (ray->side == 0)
    ray->perp_dist = ray->side_dist_x - ray->delta_dist_x;
else
    ray->perp_dist = ray->side_dist_y - ray->delta_dist_y;
```

**Why subtract delta_dist?**
- `side_dist` is distance to *next* grid line
- We want distance to *current* grid line (where wall is)
- Subtract one step to get back to wall position

**Perpendicular distance prevents fish-eye effect:**
```
Wrong (Euclidean distance):
  Walls at edges appear curved
  
Correct (perpendicular distance):
  Walls remain straight
```

---

### draw_wall() - Wall Rendering

**File:** `src/render/raycast.c`

```c
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
```

**Purpose:** Draw one textured wall column on screen.

#### Wall Height Calculation

**Lines 6-8:**
```c
if (ray->perp_dist <= 0)
    ray->perp_dist = 0.0001;
col.line_height = (int)(WIN_HEIGHT / ray->perp_dist);
```

**Projection formula:**
```
line_height = screen_height / distance
```

**Examples:**
```
Distance 1.0:
  line_height = 720 / 1.0 = 720 pixels (fills entire screen height)

Distance 2.0:
  line_height = 720 / 2.0 = 360 pixels (half screen)

Distance 10.0:
  line_height = 720 / 10.0 = 72 pixels (small, far away)

Distance 0.1:
  line_height = 720 / 0.1 = 7200 pixels (huge, very close)
```

**Why clamp to 0.0001?**
- Prevents division by zero
- Handles edge case of ray hitting at player position
- Small value keeps math valid

#### Wall Limits

**Line 9:**
```c
compute_wall_limits(col.line_height, &col.start, &col.end);
```

**File:** `src/render/ray_texture.c`

```c
void	compute_wall_limits(int line_height, int *start, int *end)
{
	*start = -line_height / 2 + WIN_HEIGHT / 2;
	if (*start < 0)
		*start = 0;
	*end = line_height / 2 + WIN_HEIGHT / 2;
	if (*end >= WIN_HEIGHT)
		*end = WIN_HEIGHT - 1;
}
```

**Purpose:** Calculate which screen pixels to draw the wall on.

**Formula:**
```
Center screen at y = 360 (WIN_HEIGHT / 2)

Wall top:
  start = center - (line_height / 2)
  
Wall bottom:
  end = center + (line_height / 2)
```

**Examples:**

```
Line height = 200:
  start = 360 - 100 = 260
  end = 360 + 100 = 460
  → Draw pixels 260 to 460

Line height = 1000 (very close wall):
  start = 360 - 500 = -140 → clamped to 0
  end = 360 + 500 = 860 → clamped to 719
  → Draw entire column (off-screen parts ignored)

Line height = 50 (far wall):
  start = 360 - 25 = 335
  end = 360 + 25 = 385
  → Draw small 50-pixel tall wall
```

**Visual:**
```
Screen (y-axis):          Wall rendering:
0   ┌───────────┐         ┌───────────┐
    │ Ceiling   │         │ Ceiling   │ 
    │           │         │           │
260 ├───────────┤  ←start ├═══════════┤ ← Wall top
    │           │         ║ Textured  ║
360 │  Horizon  │         ║   Wall    ║ ← Center
    │           │         ║           ║
460 ├───────────┤  ←end   ├═══════════┤ ← Wall bottom
    │           │         │           │
    │  Floor    │         │  Floor    │
720 └───────────┘         └───────────┘
```

#### Texture Selection

**Line 10:**
```c
tex = &game->texture[select_texture_index(game, ray)];
```

**File:** `src/render/ray_texture.c`

```c
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
```

**Purpose:** Choose correct wall texture based on which side was hit.

**Logic:**
```
Vertical walls (side == 0):
  Ray pointing east (dir_x > 0) → West texture
  Ray pointing west (dir_x < 0) → East texture

Horizontal walls (side == 1):
  Ray pointing south (dir_y > 0) → South texture
  Ray pointing north (dir_y < 0) → North texture
```

**Why backwards?**
```
Ray going east hits WEST face of wall:
    │
←───•  Ray
    │ ← West face visible
    
Ray going west hits EAST face of wall:
    │
    •───→ Ray
← East face visible
    │
```

**Texture mapping:**
| Side | Direction | Texture |
|------|-----------|---------|
| Vertical | East | TEX_WE (0) |
| Vertical | West | TEX_EA (3) |
| Horizontal | South | TEX_SO (1) |
| Horizontal | North | TEX_NO (2) |

#### Texture X Coordinate

**Line 11:**
```c
col.tex_x = compute_tex_x(game, ray, tex);
```

**File:** `src/render/ray_texture.c`

```c
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
```

**Purpose:** Determine which column of the texture to use.

**Wall position calculation:**
```c
if (ray->side == 0)
    wall_x = game->player.y + ray->perp_dist * ray->dir_y;
else
    wall_x = game->player.x + ray->perp_dist * ray->dir_x;
```

**What is wall_x?**
- World coordinate where ray hit the wall
- If vertical wall: use Y coordinate
- If horizontal wall: use X coordinate

**Example:**
```
Player at (2.3, 5.7)
Ray hits vertical wall at distance 3.2
Ray direction: (0.6, 0.8)

wall_x = 5.7 + 3.2 * 0.8 = 5.7 + 2.56 = 8.26
```

**Fractional part:**
```c
wall_x -= floor(wall_x);
```

**Purpose:** Get position within grid cell (0.0 to 1.0)

```
wall_x = 8.26
→ floor(8.26) = 8.0
→ wall_x = 8.26 - 8.0 = 0.26

Meaning: Hit wall 26% across the grid cell
```

**Texture column:**
```c
tex_x = (int)(wall_x * tex->width);
```

**Example:**
```
wall_x = 0.26
tex->width = 64

tex_x = 0.26 * 64 = 16.64 → 16

→ Use column 16 of 64-pixel wide texture
```

**Mirroring for consistency:**
```c
if (ray->side == 0 && ray->dir_x > 0)
    tex_x = tex->width - tex_x - 1;
if (ray->side == 1 && ray->dir_y < 0)
    tex_x = tex->width - tex_x - 1;
```

**Why mirror?**
- Ensures textures face correct direction
- Prevents textures appearing backwards
- Creates visual consistency

**Lines 12-15: Clamp bounds**
```c
if (col.tex_x < 0)
    col.tex_x = 0;
if (col.tex_x >= tex->width)
    col.tex_x = tex->width - 1;
```
- Prevents array out-of-bounds
- Handles edge cases in calculation

#### Draw Texture Column

**Line 16-17:**
```c
col.x = x;
draw_texture_column(game, ray, tex, &col);
```

**File:** `src/render/ray_texture.c`

```c
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
```

**Purpose:** Draw one textured column from `start` to `end`.

#### Texture Stepping

**Line 8:**
```c
step = (double)tex->height / col->line_height;
```

**Purpose:** Calculate how much to advance in texture per screen pixel.

**Examples:**
```
Texture height = 64
Wall height on screen = 320

step = 64 / 320 = 0.2

→ Each screen pixel advances 0.2 pixels in texture
→ Texture stretched to fit wall

Texture height = 64
Wall height on screen = 32

step = 64 / 32 = 2.0

→ Each screen pixel advances 2.0 pixels in texture
→ Texture compressed (wall is far)
```

**Initial texture position:**
```c
tex_pos = (col->start - WIN_HEIGHT / 2 + col->line_height / 2) * step;
```

**Purpose:** Align texture correctly even if wall is clipped.

#### Drawing Loop

**Lines 10-18:**
```c
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
```

**Process:**
1. Get color from texture at (tex_x, tex_pos)
2. Darken if horizontal wall (shading)
3. Draw pixel to frame buffer
4. Advance texture position
5. Next screen row

**Shading:**
```c
if (ray->side == 1)
    color = (color >> 1) & 0x7F7F7F;
```

**Purpose:** Make horizontal walls darker for depth perception.

**How it works:**
```
Original color: 0xAABBCC (R=AA, G=BB, B=CC)

Right shift by 1: 0xAABBCC >> 1 = 0x555DE6
Mask with 0x7F7F7F: 0x555DE6 & 0x7F7F7F = 0x555D66

Result: Each color component halved
→ Darker appearance
```

**Visual effect:**
```
Vertical walls (side=0):   Full brightness
Horizontal walls (side=1): 50% brightness

Creates subtle lighting effect
Helps distinguish wall orientation
```

---

## Utility Functions

### Memory and String Functions

**File:** `src/utils/string.c`, `src/utils/memory.c`

These implement standard C library functions to avoid external dependencies:

- `ft_strlen()` - String length
- `ft_strdup()` - String duplication
- `ft_strncmp()` - String comparison
- `ft_bzero()` - Zero memory
- `ft_calloc()` - Allocate zeroed memory
- `safe_malloc()` - Checked allocation

### Error Handling

**File:** `src/utils/error.c`

```c
int	print_error(const char *msg)
{
	int	len;

	write(2, "Error\n", 6);
	if (!msg)
		return (1);
	len = ft_strlen(msg);
	if (len > 0)
		write(2, msg, len);
	write(2, "\n", 1);
	return (1);
}
```

**Format:**
```
Error
<message>
```

**Return:** Always `1` for easy error propagation

**Example:**
```c
if (!valid)
    return (print_error("Invalid map"));

→ Outputs to stderr:
Error
Invalid map

→ Returns 1
```

---

## Complete Execution Flow

### From Start to Render

```
1. main() starts
   ├─ Validate arguments
   ├─ check_extension(".cub")
   ├─ init_game_struct() - zero memory
   │
   ├─ parse_scene()
   │  ├─ Open .cub file
   │  ├─ Parse textures (NO/SO/WE/EA)
   │  ├─ Parse colors (F/C)
   │  ├─ Parse map grid
   │  ├─ setup_player() - find start position
   │  └─ validate_scene() - check walls
   │
   ├─ init_game()
   │  ├─ mlx_init()
   │  ├─ mlx_new_window()
   │  ├─ create_frame() - frame buffer
   │  └─ load_texture() × 4 - wall textures
   │
   ├─ setup_hooks()
   │  ├─ mlx_hook() - key press
   │  ├─ mlx_hook() - key release
   │  ├─ mlx_hook() - window close
   │  └─ mlx_loop_hook() - game loop
   │
   └─ mlx_loop() ◄──────────────┐
                                │
2. Game loop (repeating):       │
   ├─ game_loop()               │
   │  ├─ update_game_state()    │
   │  │  ├─ update_position()   │
   │  │  │  └─ move_player()    │
   │  │  └─ apply_rotation()    │
   │  │                         │
   │  └─ render_frame()         │
   │     ├─ render_background() │
   │     │  └─ Fill floor/ceiling
   │     │                      │
   │     ├─ For each column (0-1279):
   │     │  ├─ ray_init()       │
   │     │  ├─ ray_set_step()   │
   │     │  ├─ ray_step_until_hit()
   │     │  │  └─ DDA algorithm │
   │     │  └─ draw_wall()      │
   │     │     ├─ select_texture()
   │     │     ├─ compute_tex_x()
   │     │     └─ draw_texture_column()
   │     │                      │
   │     └─ mlx_put_image_to_window()
   │                            │
   └────────────────────────────┘

3. User closes window:
   ├─ handle_close()
   │  ├─ destroy_game()
   │  │  ├─ Destroy images
   │  │  ├─ Destroy window
   │  │  ├─ Free map
   │  │  └─ Free config
   │  └─ exit(0)
   └─ Program terminates
```

---

## Performance Considerations

### Frame Rate

**No frame limiting:**
- Game loop runs as fast as possible
- Typical: 60-1000+ FPS depending on CPU
- Movement scaled by fixed constants (frame-independent)

**Optimization opportunities:**
- Frame rate limiting (e.g., 60 FPS cap)
- Multi-threading for raycasting
- Sprite caching

### Memory Usage

**Approximate totals:**
- Frame buffer: ~3.5 MB (1280×720×4)
- Textures: ~65 KB (4×64×64×4)
- Map grid: Variable (depends on map size)
- Total: ~4-5 MB typical

### Raycasting Cost

**Per frame:**
- 1280 rays cast
- Each ray: ~5-50 DDA steps (depends on map)
- ~10,000 total steps per frame
- At 60 FPS: ~600,000 steps/second

**Bottlenecks:**
- Texture sampling (most expensive)
- Trigonometric functions (rotation)
- Memory access patterns

---

## Summary

This Cub3D engine demonstrates:

✅ **Parsing:** Robust .cub file validation  
✅ **Graphics:** MiniLibX integration  
✅ **Raycasting:** DDA algorithm for 3D rendering  
✅ **Input:** Event-driven state management  
✅ **Physics:** Smooth movement with collision  
✅ **Textures:** XPM texture mapping  
✅ **Memory:** Safe allocation and cleanup  

**Key algorithms:**
- DDA (Digital Differential Analysis) for raycasting
- 2D rotation matrices for camera control
- Perpendicular distance correction for fisheye prevention
- Texture mapping with proper alignment

**Total lines:** ~2000 lines of C code organized into:
- Parsing: ~400 lines
- Rendering: ~500 lines  
- Game logic: ~300 lines
- Utilities: ~300 lines
- Headers: ~200 lines

---

**End of Code Explanation**

🎮 **Happy raycasting!**

