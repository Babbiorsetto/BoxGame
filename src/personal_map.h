#ifndef PERSONAL_MAP_H
#define PERSONAL_MAP_H

#include "game_map.h"

struct personal_map_t;

int personal_map_create(struct personal_map_t **map, struct game_map_t *reference);

int personal_map_destroy(struct personal_map_t *map);

char personal_map_getSymbol(struct personal_map_t *map, int x, int y);

const char *personal_map_getString(struct personal_map_t *map);

void personal_map_revealObstacle(struct personal_map_t *map, int x, int y);

#endif
