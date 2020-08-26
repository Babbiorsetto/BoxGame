#include <stdlib.h>
#include <string.h>
#include "player_alias.h"
#include "personal_map.h"
#include "game_map.h"

struct player_alias_t *player_alias_create(char *username, struct sockaddr_in *address, int clientDescriptor, struct game_map_t *gameMap)
{
    struct player_alias_t *alias = malloc(sizeof(struct player_alias_t));
    if (alias == NULL)
    {
        return NULL;
    }
    alias->username = malloc(strlen(username) + 1);
    if (alias->username == NULL)
    {
        free(alias);
        return NULL;
    }
    int error = personal_map_create(&(alias->map), gameMap);
    if (error == -1)
    {
        free(alias->username);
        free(alias);
        return -1;
    }
    
    alias->address = address;
    alias->active = 1;
    alias->connection = clientDescriptor;
    alias->x = 0;
    alias->y = 0;
    alias->box = 0;
    alias->duration = 0;
    alias->points = 0;
    
    return alias;
}

void player_alias_destroy(struct player_alias_t *alias)
{
    free(alias->username);
    //TODO should I really always free the address? It was allocated by someone else
    free(alias->address);
    free(alias);
}
