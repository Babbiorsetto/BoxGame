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

/*
* Frees memory initialized by player_alias_create, that is username, map and the struct itself.
* Does not free address, which create doesn't allocate.
*
* @param alias The player_alias to destroy
*/
void player_alias_destroy(struct player_alias_t *alias);

/*
* Used to copy some fields from one instance of a player_alias to another. 
* The fields that are copied are the ones that need updating when a player rejoins a game, 
* namely address, connection
*
* @param dest the alias to copy the fields in
* @param src the alias from which to copy
* @return 0 on success, -1 in case of error
*/
int player_alias_copyOver(struct player_alias_t *dest, struct player_alias_t *src);

#endif
