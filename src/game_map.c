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
    unsigned short *data;
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
#define PLAYER_LSB 0
#define BOX_BITMASK 6
#define BOX_LSB 1
#define DROPOFF_BITMASK 24
#define DROPOFF_LSB 3
#define DURATION_BITMASK 480
#define DURATION_LSB 5
#define OBSTACLE_BITMASK 512
#define OBSTACLE_LSB 9

int game_map_create(struct game_map_t **map, unsigned short width, unsigned short height)
{
    *map = malloc(sizeof(struct game_map_t));
    if (*map == NULL)
    {
        return -1;
    }    
    (*map)->data = calloc(width * height, sizeof(unsigned short));
    if ((*map)->data == NULL)
    {
        free(*map);
        return -1;
    }
    
    (*map)->height = height;
    (*map)->width = width;
    return 0;
}

int game_map_destroy(struct game_map_t *map)
{
    if (map == NULL)
    {
        return -1;
    }

    free(map->data);
    free(map);
    return 0;
}

int game_map_getWidth(struct game_map_t *map)
{
    if (map == NULL)
    {
        return -1;
    }
    
    return map->width;
}

int game_map_getHeight(struct game_map_t *map)
{
    if (map == NULL)
    {
        return -1;
    }
    
    return map->height;
}

int game_map_hasPlayer(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    return (val & PLAYER_BITMASK) >> PLAYER_LSB;
    
}

int game_map_hasObstacle(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    return (val & OBSTACLE_BITMASK) >> OBSTACLE_LSB;
}

/*
* Signals whether the given square contains a box and the number of the box 
* if there happens to be one. Internal use
* 
* @return 0 if there is no box, the box number otherwise
*/
int game_map_hasBox(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    return (val & BOX_BITMASK) >> BOX_LSB;
}

/*
* Internal use
* 
* 
*/
int game_map_boxDuration(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    return (val & DURATION_BITMASK) >> DURATION_LSB;
}

int game_map_hasDropoff(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    return (val & DROPOFF_BITMASK) >> DROPOFF_LSB;
}

int game_map_setPlayer(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    if (game_map_hasPlayer(map, x, y))
    {
        return 2;
    }

    if (game_map_hasObstacle(map, x, y))
    {
        return 3;
    }    

    unsigned short val = map->data[x * map->width + y];
    map->data[x * map->width + y] = val | PLAYER_BITMASK;
    return 1;
}

int game_map_unsetPlayer(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    map->data[x * map->width + y] = val & ~(PLAYER_BITMASK);
    return 0;
}

int game_map_setObstacle(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    map->data[x * map->width + y] = val | OBSTACLE_BITMASK;
}

int game_map_unsetObstacle(struct game_map_t *map, unsigned int x, unsigned int y)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    map->data[x * map->width + y] = val & ~(OBSTACLE_BITMASK);
    return 0;
}

int game_map_setBox(struct game_map_t *map, unsigned int x, unsigned int y, unsigned short number, unsigned short duration)
{
    if (x < 0 || x >= map->height || y < 0 || y >= map->width || duration < 0 || duration > 15 || number < 0 || number > 3)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    val = val & (~(BOX_BITMASK | DURATION_BITMASK));
    map->data[x * map->width + y] = val | (number << BOX_LSB) | (duration << DURATION_LSB); 
    return 0;
}

int game_map_setDropoff(struct game_map_t *map, unsigned int x, unsigned int y, unsigned short number)
{
    if (x >= map->height || y >= map->width)
    {
        return -1;
    }

    unsigned short val = map->data[x * map->width + y];
    map->data[x * map->width + y] = val | (number << DROPOFF_LSB);
}

int game_map_tick(struct game_map_t *map)
{

}

int game_map_pickup(struct game_map_t *map, unsigned int x, unsigned int y, int *number, int *duration)
{
    if (map == NULL || x < 0 || x >= map->height || y < 0 || y >= map->width || number == NULL || duration == NULL)
    {
        return -1;
    }
    
    if (game_map_hasBox(map, x, y) > 0)
    {
        *number = game_map_hasBox(map, x, y);
        *duration= game_map_boxDuration(map, x, y);
        game_map_setBox(map, x, y, 0, 0);
        return 1;
    }
    else
    {
        return 0;
    }
    
    return -1;
}

int game_map_drop(struct game_map_t *map, unsigned int x, unsigned int y, int number, int duration)
{
    if (map == NULL || x < 0 || x >= map->height || y < 0 || y >= map->width)
    {
        return -1;
    }
    if (duration < 0 || duration > 15 || number < 0 || number > 3)
    {
        return -1;
    }

    if (game_map_hasBox(map, x, y))
    {
        return 4;
    }

    int dropN = game_map_hasDropoff(map, x, y);
    // same dropoff as the box
    if (dropN == number)
    {
        return 2;
    }
    // no dropoff
    else if (dropN == 0)
    {
        
        // game_map_setBox(map, x, y, number, duration);
        return 4;
    }
    // dropN == a valid dropoff but not the right one
    else
    {
        return 3;
    }
    
    return -1;
    
}

void game_map_clear(struct game_map_t *map)
{
    int length = game_map_getHeight(map) * game_map_getWidth(map);
    int i, j;

    for (i = 0; i < length; i++)
    {
        map->data[i] = 0u;
    }
    
}
