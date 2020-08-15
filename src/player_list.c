#include "player_list.h"
#include "player_alias.h"
#include <pthread.h>

struct player_list_t
{
    pthread_mutex_t *lock;
    struct player_node_t *first;
};

struct player_node_t
{
    struct player_node_t *next;
    struct player_alias_t *player;
};

struct player_list_iterator_t
{
    struct player_list_t *list;
    struct player_node_t *current;
};

int player_list_iterator_create(struct player_list_t *list, struct player_list_iterator_t *iterator)
{
    //TODO
}

int player_list_iterator_destroy(struct player_list_iterator_t *iterator)
{
    //TODO
}

struct player_alias_t *player_list_iterator_next(struct player_list_iterator_t *iterator, int *last)
{
    /*TODO body*/
}
