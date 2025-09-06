#include "../cub3d.h"

int get_map(char **data)
{
    // TODO: Implement map extraction logic
    if (!data)
        return (write(2, "Error\nNo data provided\n", 24), 1);
    return (0);
}
