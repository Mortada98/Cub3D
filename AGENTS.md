# Repository Guidelines

## Project Structure & Module Organization
- `main.c` drives CLI entry, orchestrating parsing and cleanup.
- `cub3d.h` centralizes structs (`t_walls`, `t_map_data`, `t_player`) and helper prototypes.
- `parse_data/` parses wall textures and the map grid; `utils/` hosts shared string and memory helpers.
- `.cub` fixtures live in `maps/`; keep new fixtures alongside the existing samples for easy regression checks.
- Vendor code stays in `minilibx-linux/`; avoid modifying it without syncing upstream.

## Build, Test, and Development Commands
- `make` builds the `cub3D` binary with `cc -Wall -Wextra -Werror`.
- `make clean`, `make fclean`, and `make re` remove intermediates, fully clean, or rebuild from scratch.
- `./cub3D maps/example.cub` runs the parser against a map; swap in your fixture path during development.
- `valgrind ./cub3D maps/example.cub` (optional) checks for leaks before submitting changes.

## Coding Style & Naming Conventions
- Follow 42 Norminette conventions: tabs for indentation, 80-character lines, and `snake_case` identifiers.
- Mirror the existing 42 headers on new source files and keep prototypes in `cub3d.h`.
- Group new helpers by concern inside `utils/` and register each new `.c` file in the `SRCS` list of the `Makefile`.
- Prefer explicit error messages beginning with `Error\n` to match current output expectations.

## Testing Guidelines
- No automated suite yet; craft targeted `.cub` files under `maps/` for both positive and negative cases.
- Use descriptive filenames (e.g., `invalid_texture.cub`, `missing_player.cub`) to clarify intent.
- Run `./cub3D <map>` after each change and inspect stderr for the expected validation failures.
- Capture Valgrind output or logs in PR descriptions when touching allocation-heavy code.

## Commit & Pull Request Guidelines
- History uses short, present-tense summaries (`working on parsing`); keep messages concise but outcome-focused (`Add wall parser`, `Fix map trimming`).
- Squash noisy WIP commits before review and include key commands run in the commit body.
- PRs should note new maps, update instructions for reproducing errors, and verify `make re` succeeds.
- Link issues or cards when available, and request feedback on parsing edge cases explicitly.

## Map & Asset Notes
- Store shared textures outside version control; reference them with relative paths in `.cub` files.
- Validate floor and ceiling colors plus texture paths locally before sharing sample maps.
