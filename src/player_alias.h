#ifndef PLAYER_ALIAS_H
#define PLAYER_ALIAS_H

#include <netinet/in.h>
#include "personal_map.h"
#include "game_map.h"

struct player_alias_t
{
    char *username;
    struct sockaddr_in *address;
    int connection;
    int active;
    int x;
    int y;
    int box;
    int duration;
    int points;
    struct personal_map_t *map;
};

struct player_alias_t *player_alias_create(char *username, struct sockaddr_in *address, int clientDescriptor, struct game_map_t *gameMap);

void player_alias_destroy(struct player_alias_t *alias);

#endif
