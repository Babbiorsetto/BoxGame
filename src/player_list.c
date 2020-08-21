#include "player_list.h"
#include "player_alias.h"
#include <pthread.h>
#include <stdlib.h>

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
    if((*iterator = malloc(sizeof(player_list_iterator_t))) == NULL)
        return -1;

    else {
        iterator->list = list;
        if(!(player_list_is_empty(list))) {
            iterator->current = iterator->list->first;
        } else {
            return -2;
        }
        return 0;
    }
    
}

int player_list_iterator_destroy(struct player_list_iterator_t *iterator)
{
    free(iterator);
    return 0;
}

struct player_alias_t *player_list_iterator_next(struct player_list_iterator_t *iterator, int *last)
{
    player_alias_t *current_player;

    pthread_mutex_lock(iterator->list->lock);
    current_player = iterator->current->player;

    if(iterator->current->next == NULL) {
        *last = 1;
        iterator->current = iterator->list->first;
    } else {
        *last = 0;
        iterator->current = iterator->current->next;
    }

    pthread_mutex_unlock(iterator->list->lock);
    return current_player;
}

int player_list_is_empty(struct player_list_t *list)
{
    int check = 0;
    pthread_mutex_lock(list->lock);
    check = (list->first == NULL);
    pthread_mutex_unlock(list->lock);
    return check;
}

int player_list_create(struct player_list_t *list) 
{
    if(!(list = malloc(sizeof(player_list_t))) == NULL) {
        if(!(list->lock = malloc(sizeof(pthread_mutex_t)) == NULL)) {
            list->first = NULL;
            pthread_mutex_init(list->lock, NULL);
            return 0;
        } else 
            return -2;
    } else 
        return -1;
}

int player_list_add(struct player_list_t *list, struct player_alias_t *player) 
{
    player_node_t *curr = NULL;
    player_node_t *pl = NULL;

    if(list == NULL || player == NULL) {
        return -1;
    } else {
        pthread_mutex_lock(list->lock);

        if(list->first == NULL) {
            *list->first = *player;
        } else {
            curr = list->first;
            while(curr->next != NULL) {

                if(curr->player == player) {
                    pthread_mutex_unlock(list->lock);
                    return 0;
                    }

                curr = curr->next;
            }

            if((pl = malloc(sizeof(player_node_t))) == NULL) {
                return -2;
            }
            pl->player = player;
            pl->next = NULL;

            *curr->next = *pl;
        }

        pthread_mutex_unlock(list->lock);
    }
    return 1;
}

void player_list_purge(struct player_list_t *list) 
{
    player_node_t *curr, *prev = NULL;
    if (!player_list_is_empty(list)){
        pthread_mutex_lock(list->lock);

        curr = list->first;
        prev = list->first;

        while(curr->next != NULL) {

            if (!(curr->player->active)) {

                if(curr == list->first) {
                    list->first = list->first->next;
                    free(curr);
                    curr = list->first;
                    prev = curr;
                } else {
                    prev->next = curr->next;
                    free(curr);
                    curr=prev->next;
                }
            }

            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(list->lock);
    }
    return;
}
