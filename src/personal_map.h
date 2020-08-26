#ifndef PERSONAL_MAP_H
#define PERSONAL_MAP_H

#include "game_map.h"

struct personal_map_t;

int personal_map_create(struct personal_map_t **map, struct game_map_t *reference);

int personal_map_destroy(struct personal_map_t *map);

#endif
