#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

#include "player_alias.h"

struct player_list_t;
struct player_list_iterator_t;

/*
* Allocates memory for an iterator on the given list.
*
* @param list The list on which the new iterator cycles
* @param iterator Memory location that will contain the new iterator, if the call to create succeded
* @return 0 if the iterator was created successfully, non-zero otherwise (specifically, -1 if the function cannot allocate enough memory for the iterator, -2 if the player list is empty)
*/
int player_list_iterator_create(struct player_list_t *list, struct player_list_iterator_t **iterator);

/*
* Frees the memory used by the given iterator.
*
* @param iterator The iterator to destroy
* @return 0 after the iterator is destroyed
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
* @param list Memory location that will contain a pointer to the new list, if the call succeded
* @return 0 if successful, non-zero otherwise (specifically, -2 if there is not enough memory to create the lock for the list, -1 if there is not enough memory to create the list)
*/
int player_list_create(struct player_list_t **list);

/*
* Frees a player_list.
* Does nothing yet. Implement it
* TODO
* @param name A parameter
* @return A return
*/
int player_list_destroy(struct player_list_t *list);

/*
* Checks if the given player list is empty
* 
* @param list Memory location that contains the list that needs to be checked
* @return 1 if the list is empty, 0 otherwise
*/
int player_list_is_empty(struct player_list_t *list);

/*
* Appends the given player to the given player list.
* @param list is the memory location that contains the list to which the player must be added
* @param player is the memory location that contains the player that is going to be added to the list
* @return 0 if the player cannot be added because they are already in the list and active, 
* 1 if the player is succesfully added, 2 if they are re-added, 
* -1 on error
*/
int player_list_add(struct player_list_t *list, struct player_alias_t *player);

/*
* Removes inactive players from the given list.
* @param list is the memory location of the given player list
* @return void
*/
void player_list_purge(struct player_list_t *list);

/*
* Puts caller in wait for the given list to not be empty. 
* Returns when the list contains any element.
* 
* @param list The list to wait on
*/
void player_list_waitOnEmpty(struct player_list_t *list);

/*
* All players in the list have their box duration reduced by 1 turn. 
* If any box's duration becomes 0, that box is removed from the game.
*
* @param list The list whose players need to have their duration reduced
* @return The number of boxes removed due to their duration becoming 0
*/
int player_list_tick(struct player_list_t *list);

/*
* Empties all data regarding the players in the given players list.
* @param list The player list that must be reset
* @return void
*/
void player_list_resetPlayers(struct player_list_t *list);

int player_list_count(struct player_list_t *list);

#endif
