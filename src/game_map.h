#ifndef GAME_MAP_H
#define GAME_MAP_H

struct game_map_t;

int game_map_create(struct game_map_t **map, unsigned short width, unsigned short height);

int game_map_destroy(struct game_map_t *map);

/*
* Returns width of given map
*
* @return Width of map map, -1 if map is NULL
*/
int game_map_getWidth(struct game_map_t *map);

/*
* Returns height of given map
*
* @return Height of map map, -1 if map is NULL
*/
int game_map_getHeight(struct game_map_t *map);

int game_map_hasPlayer(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_hasObstacle(struct game_map_t *map, unsigned int x, unsigned int y);

/*
* Sets the given square to contain a player, if it can according to the rules. 
* A square cannot be set to contain a player if 
* it already contains a player or 
* it contains an obstacle
*
* @return 1 if the square is set to contain a player, 
* 2 if it cannot be set because it had a player already, 
* 3 if it has an obstacle, -1 on error
*/
int game_map_setPlayer(struct game_map_t *map, unsigned int x, unsigned int y);

/*
* Sets the given square to not contain a player, with no checks on whether it had one 
* to begin with
*
* @return 0 on normal operation, -1 on error
*/
int game_map_unsetPlayer(struct game_map_t *map, unsigned int x, unsigned int y);

/*
* Removes the box on the square given by coordinates and returns its properties 
* in the passed parameters
* TODO
* @param name A parameter
* @return 1 if a box was removed from the square, 0 if there was no box to remove, -1 in case of error
*/
int game_map_pickup(struct game_map_t *map, unsigned int x, unsigned int y, int *number, int *duration);

/*
* Places the given box on the square given by coordinates 
* if it can be placed according to the game rules
* 
* @param name A parameter
* @return 1 if the box is dropped successfully, 
* 2 if it is dropped and consumed by a corresponding dropoff point, 
* 3 if it is dropped and consumed by the wrong dropoff point, 
* 4 if it cannot be dropped because another box is already present, 
* -1 in case of error
*/
int game_map_drop(struct game_map_t *map, unsigned int x, unsigned int y, int number, int duration);

int game_map_setObstacle(struct game_map_t *map, unsigned int x, unsigned int y);

int game_map_setBox(struct game_map_t *map, unsigned int x, unsigned int y, unsigned short number, unsigned short duration);

int game_map_setDropoff(struct game_map_t *map, unsigned int x, unsigned int y, unsigned short number);

int game_map_tick(struct game_map_t *map);

#endif
