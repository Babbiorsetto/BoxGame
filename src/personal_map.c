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
    int refW = game_map_getWidth(reference), refH = game_map_getHeight(reference);
    // same height as reference, 1 more width to store \n and \0
    (*map)->data = malloc( (refW + 1) * refH * sizeof(char));
    if ((*map)->data == NULL)
    {
        free(*map);
        return -1;
    }

    /* copy info from reference */
    int i, j, n;
    for (i = 0; i < refH; i++)
    {
        for (j = 0; j < refW + 1; j++)
        {
            // it's at the right, either \n or \0
            if (j == refW)
            {
                if (i == refH - 1)
                {
                    (*map)->data[i * (refW + 1) + j] = '\0';
                }
                else
                {
                    (*map)->data[i * (refW + 1) + j] = '\n';
                }
                
            }
            // players have the highest priority
            else if (game_map_hasPlayer(reference, i, j) == 1)
            {
                (*map)->data[i * (refW + 1) + j] = 'P';
            }
            // dropoffs and boxes have lower priority
            else if (n = game_map_hasDropoff(reference, i, j) > 0)
            {
                switch (n)
                {
                case 1:
                    (*map)->data[i * (refW + 1) + j] = 'A';
                    break;
                case 2:
                    (*map)->data[i * (refW + 1) + j] = 'B';
                    break;
                case 3:
                    (*map)->data[i * (refW + 1) + j] = 'C';
                    break;
                default:
                    break;
                }
            }
            else if (n = game_map_hasBox(reference, i, j) > 0)
            {
                switch (n)
                {
                case 1:
                    (*map)->data[i * (refW + 1) + j] = '1';
                    break;
                case 2:
                    (*map)->data[i * (refW + 1) + j] = '2';
                    break;
                case 3:
                    (*map)->data[i * (refW + 1) + j] = '3';
                    break;
                default:
                    break;
                }
            }
            else
            {
                (*map)->data[i * (refW + 1) + j] = '-';
            }  
        }
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
