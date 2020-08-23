#ifndef GAME_MAP_H
#define GAME_MAP_H

struct game_map_t;

int game_map_create(struct game_map_t **map, unsigned short width, unsigned short height);

int game_map_destroy(struct game_map_t *map);

int game_map_hasPlayer(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_hasObstacle(struct game_map_t *map, unsigned int x, unsigned int y);

/*
* Signals whether the given square contains a box and the number of the box 
* if there happens to be one
* 
* @return 0 if there is no box, the box number otherwise
*/
int game_map_hasBox(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_boxDuration(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_dropoffNumber(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_setPlayer(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_setObstacle(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_setBox(struct game_map_t *map, unsigned int x, unsigned int y, unsigned short number, unsigned short duration);

int game_map_setDropoff(struct game_map_t *map, unsigned int x, unsigned int y, unsigned short number);

int game_map_tick(struct game_map_t *map);

#endif