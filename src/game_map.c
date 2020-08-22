#include <stdlib.h>
#include "game_map.h"

/*
* A game map is implemented as an array of unsigned short int treated as a matrix.
* Each short represents a square on the game board, its value
* is a bit encoded state of that square.
* 
* XXXX-XXXX XXXX-XXXX
* encoding of the bits (numbered from least significant):
* 0: presence of a player
* 1-2: presence/number of a box where 00 means there is no box
* and 01, 10, 11 mean box numbered 1, 2, 3 respectively
* 3-4: presence/number of a dropoff point
* 5-8: duration of a box on the square in turns where 0 means
* it will disappear next turn and 1-15 is still has that many turns left
* 9: presence of an obstacle
* 10-15: reserved for future use
*
*/
struct game_map_t
{
    unsigned short **data;
    unsigned short width;
    unsigned short height;
};

/*
* player,   bit 0,      0000-0000 0000-0001 = 1
* box,      bit 1-2,    0000-0000 0000-0110 = 6
* dropoff,  bit 3-4,    0000-0000 0001-1000 = 24
* duration, bit 5-8,    0000-0001 1110-0000 = 480
* obstacle, bit 9,      0000-0010 0000-0000 = 512
*/

#define PLAYER_BITMASK 1
#define BOX_BITMASK 6
#define DROPOFF_BITMASK 24
#define DURATION_BITMASK 480
#define OBSTACLE_BITMASK 512

int game_map_create(struct game_map_t **map, unsigned short width, unsigned short height)
{
    *map = malloc(sizeof(struct game_map_t));
    if (*map == NULL)
    {
        return -1;
    }    
    (*map)->data = malloc(sizeof(unsigned short) * width * height);
    if ((*map)->data == NULL)
    {
        free(*map);
        return -1;
    }
    
    (*map)->height = height;
    (*map)->width = width;
    return 0;
}
