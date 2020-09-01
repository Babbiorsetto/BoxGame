#include <stdlib.h>
#include "game_map.h"
#include "personal_map.h"
#include "player_alias.h"

struct personal_map_t
{
    char *data;
    struct game_map_t *reference;
    struct player_alias_t *owner;
};

int personal_map_create(struct personal_map_t **map, struct game_map_t *reference, struct player_alias_t *owner)
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
    int refW = game_map_getWidth(reference), refH = game_map_getHeight(reference);
    // same height as reference, 1 more width to store \n and \0
    (*map)->data = malloc( (refW + 1) * refH * sizeof(char));
    if ((*map)->data == NULL)
    {
        free(*map);
        return -1;
    }

    (*map)->reference = reference;
    (*map)->owner = owner;

    /* set up line endings and final null character */
    int i;
    for (i = 0; i < refH; i++)
    {
        
        if (i == refH - 1)
        {
            (*map)->data[i * (refW + 1) + refW] = '\0';
        }
        else
        {
            (*map)->data[i * (refW + 1) + refW] = '\n';
        }  
        
    }

    personal_map_update(*map);

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

char personal_map_getSymbol(struct personal_map_t *map, int x, int y)
{
    if (map == NULL)
    {
        return '?';
    }

    int refW = game_map_getWidth(map->reference), refH = game_map_getHeight(map->reference);

    if (x < 0 || y < 0 || x >= refW + 1 || y >= refH)
    {
        return '?';
    }
    
    return map->data[x * (refW + 1) + y];
    
}

int personal_map_setSymbol(struct personal_map_t *map, int x, int y, char symbol)
{
    if (map == NULL)
    {
        return -1;
    }

    int refW = game_map_getWidth(map->reference), refH = game_map_getHeight(map->reference);

    if (x < 0 || y < 0 || x >= refW || y >= refH)
    {
        return -1;
    }
    
    map->data[x * (refW + 1) + y] = symbol;
    return 0;
}

const char *personal_map_getString(struct personal_map_t *map)
{
    return map->data;
}

void personal_map_update(struct personal_map_t *map)
{
    int refW = game_map_getWidth(map->reference), refH = game_map_getHeight(map->reference);
    int i, j, n;
    for (i = 0; i < refH; i++)
    {
        for (j = 0; j < refW; j++)
        {
            
            // players have the highest priority
            if (game_map_hasPlayer(map->reference, i, j) == 1)
            {
                // it is be the owner of the map
                if (map->owner->x == i && map->owner->y == j)
                {
                    map->data[i * (refW + 1) + j] = 'Y';
                }
                else
                {
                    map->data[i * (refW + 1) + j] = 'P';
                }
                
            }
            // dropoffs and boxes have lower priority
            else if (n = game_map_hasDropoff(map->reference, i, j) > 0)
            {
                switch (n)
                {
                case 1:
                    map->data[i * (refW + 1) + j] = 'A';
                    break;
                case 2:
                    map->data[i * (refW + 1) + j] = 'B';
                    break;
                case 3:
                    map->data[i * (refW + 1) + j] = 'C';
                    break;
                default:
                    break;
                }
            }
            else if (n = game_map_hasBox(map->reference, i, j) > 0)
            {
                switch (n)
                {
                case 1:
                    map->data[i * (refW + 1) + j] = '1';
                    break;
                case 2:
                    map->data[i * (refW + 1) + j] = '2';
                    break;
                case 3:
                    map->data[i * (refW + 1) + j] = '3';
                    break;
                default:
                    break;
                }
            }
            // all that's left is obstacles or nothing
            else
            {
                // only write nothing if it wasn't revealed to be an obstacle
                if (map->data[i * (refW + 1) + j] != 'X')
                {
                    map->data[i * (refW + 1) + j] = '-';
                }
                
            }  
        }
    }
}
