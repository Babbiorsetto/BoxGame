#include "player_alias.h"

struct player_list_t;
struct player_list_iterator_t;

/*
* Allocates memory for an iterator on the given list.
*
* @param list The list on which the new iterator cycles
* @param iterator Memory location that will contain the new iterator, if the call to create succeded
* @return 0 if the iterator was created successfully, non-zero otherwise
*/
int player_list_iterator_create(struct player_list_t *list, struct player_list_iterator_t *iterator);

/*
* Frees the memory used by the given iterator.
*
* @param iterator The iterator to destroy
* @return 0 if the iterator was destroyed successfully, non-zero otherwise
*/
int player_list_iterator_destroy(struct player_list_iterator_t *iterator);

/*
* Advances iterator, returns the current player alias and signals whether the returned element was the last in one atomic operation.
*
* @param iterator The iterator to advance
* @param last After the call to next, it points to 1 if the element returned by iterator was the last element of the list, 0 otherwise
* @return The player at the iterator's current position
*/
struct player_alias_t *player_list_iterator_next(struct player_list_iterator_t *iterator, int *last);

/*
* Allocates memory for a player list and initializes it.
*
* @param list Memory location that will contain the new list, if the call succeded
* @return 0 if successful, non-zero otherwise
*/
int player_list_create(struct player_list_t *list);