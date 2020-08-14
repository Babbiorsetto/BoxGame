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

/*
* Allocates memory for an iterator on the given list.
*
* @param list The list on which the new iterator cycles
* @return A pointer to the newly created iterator
*/
struct player_list_iterator_t *create_iterator(struct player_list_t *list)
{
    //TODO
}

/*
* Frees the memory used by the given iterator.
*
* @param iterator The iterator to destroy
* @return 0 if the iterator was destroyed successfully, non-zero otherwise
*/
int destroy_iterator(struct player_list_iterator_t *iterator)
{
    //TODO
}

/*
* Advances iterator, returns the current player alias and signals whether the returned element was the last in one atomic operation.
*
* @param iterator The iterator to advance
* @param last After the call to next, it points to 1 if the element returned by iterator was the last element of the list, 0 otherwise
* @return The player at the iterator's current position
*/
struct player_alias_t *player_list_iterator_next(struct player_list_iterator_t *iterator, int *last)
{
    /*TODO body*/
}
