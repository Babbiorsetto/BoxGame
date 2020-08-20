#include <stdlib.h>
#include <string.h>
#include "player_alias.h"

struct player_alias_t *player_alias_create(char *username, struct sockaddr_in *address, int clientDescriptor)
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
    alias->address = address;
    alias->active = 1;
    alias->connection = clientDescriptor;
    
    return alias;
}

void player_alias_destroy(struct player_alias_t *alias)
{
    free(alias->username);
    //TODO should I really always free the address? It was allocated by someone else
    free(alias->address);
    free(alias);
}