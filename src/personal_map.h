#ifndef PERSONAL_MAP_H
#define PERSONAL_MAP_H

#include "game_map.h"
#include "player_alias.h"

struct personal_map_t;

/*
* Creates a new personal_map with given reference game_map, copying
* boxes, dropoffs and players, but not obstacles.
*
* @param map The address of a pointer that will be made to point to the newly created personal_map
* @param reference The game_map to use as a reference to fill the new personal_map, as well as the permanent reference it will hold.
* must not be NULL
* @param owner The owner of the new map, used to properly draw their position. Must not be NULL
* @return 0 on normal operation, -1 in case of error
*/
int personal_map_create(struct personal_map_t **map, struct game_map_t *reference, struct player_alias_t *owner);

/*
* Destroys a previously created personal_map
* TODO
* @param map The map to destroy, must not be NULL
* @return 0 on normal operation, -1 on error
*/
int personal_map_destroy(struct personal_map_t *map);

/*
* Returns the symbol in the given position in the map
*
* @param map The map to extract the symbol from. Must not be NULL
* @param x The x coordinate of the symbol. Must be valid for the given map
* @param y The y coordinate of the symbol. Must be valid for the given map
* @return '?' on error, a char representing the symbol otherwise
*/
char personal_map_getSymbol(struct personal_map_t *map, int x, int y);

/*
* Provides a string (NULL terminated) representation of the content of given map
* TODO
* @param map The map to get the string of. Must not be NULL
* @return A string representing the content of the map
*/
const char *personal_map_getString(struct personal_map_t *map);

/*
* Sets the given position to given symbol.
*
* @param map The map to set the obstacle on. Must not be NULL
* @param x The x coordinate of the obstacle. Must be valid for the given map
* @param y The y coordinate of the obstacle. Must be valid for the given map
* @param symbol The symbol to set. No check is performed on this
* @return 0 on normal operation. -1 on error
*/
int personal_map_setSymbol(struct personal_map_t *map, int x, int y, char symbol);

/*
* Updates the state of given personal_map according to the info in its reference 
* game_map. Players, boxes and dropoffs are updated. Obstacles are left unchanged in their discovery status.
*
* @param map
*/
void personal_map_update(struct personal_map_t *map);

#endif
