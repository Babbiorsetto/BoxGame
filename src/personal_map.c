#include <stdlib.h>
#include "game_map.h"
#include "personal_map.h"

struct personal_map_t
{
    char *data;
    struct game_map_t *reference;
};

int personal_map_create(struct personal_map_t **map, struct game_map_t *reference)
{
    if (reference == NULL)
    {
        return -1;
    }
    
    *map = malloc(sizeof(struct personal_map_t));
    if (map == NULL)
    {
        return -1;
    }
    (*map)->data = malloc(game_map_getWidth(reference) * game_map_getHeight(reference) * sizeof(char));
    if ((*map)->data == NULL)
    {
        free(*map);
        return -1;
    }

    (*map)->reference = reference;
    return 0;
}

int personal_map_destroy(struct personal_map_t *map)
{
    if (map == NULL)
    {
        return -1;
    }

    free(map->data);
    free(map);
    return 0;
}

void personal_map_revealObstacle(struct personal_map_t *map, int x, int y)
{
    //TODO
}
