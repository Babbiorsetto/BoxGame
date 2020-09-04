#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "player_alias.h"
#include "player_list.h"
#include "personal_map.h"

struct player_list_t
{
    pthread_mutex_t *lock;
    pthread_cond_t *notEmpty;
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

int player_list_is_empty(struct player_list_t *list)
{
    int check = 0;
    pthread_mutex_lock(list->lock);
    check = (list->first == NULL);
    pthread_mutex_unlock(list->lock);
    return check;
}

int player_list_iterator_create(struct player_list_t *list, struct player_list_iterator_t **iterator)
{
    if ((*iterator = malloc(sizeof(struct player_list_iterator_t))) == NULL)
        return -1;

    else
    {
        pthread_mutex_lock(list->lock);
        (*iterator)->list = list;
        if (list->first != NULL)
        { 
            (*iterator)->current = (*iterator)->list->first;
        } 
        //if the list is empty, the iterator will be freed
        else    
        {
            free(*iterator);
            return -2;
        }
        pthread_mutex_unlock(list->lock);
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
    struct player_alias_t *current_player;

    pthread_mutex_lock(iterator->list->lock);
    current_player = iterator->current->player;

    if (iterator->current->next == NULL)
    {
        *last = 1;
        iterator->current = iterator->list->first;
    }
    else
    {
        *last = 0;
        iterator->current = iterator->current->next;
    }

    pthread_mutex_unlock(iterator->list->lock);
    return current_player;
}

int player_list_create(struct player_list_t **list)
{
    if ((*list = malloc(sizeof(struct player_list_t))) == NULL)
    {
        return -1;
    }

    if (((*list)->lock = malloc(sizeof(pthread_mutex_t))) == NULL)
    {
        free(*list);
        return -2;
    }

    if (((*list)->notEmpty = malloc(sizeof(pthread_cond_t))) == NULL)
    {
        free((*list)->lock);
        free(*list);
        return -1;
    }
    
    (*list)->first = NULL;
    if (pthread_mutex_init((*list)->lock, NULL) != 0)
    {
        free((*list)->notEmpty);
        free((*list)->lock);
        free(*list);
        return -1;
    }
    if (pthread_cond_init((*list)->notEmpty, NULL) != 0)
    {
        pthread_mutex_destroy((*list)->lock);
        free((*list)->notEmpty);
        free((*list)->lock);
        free(*list);
        return -1;
    }
    
    return 0;
}

int player_list_destroy(struct player_list_t *list)
{

}

int player_list_add(struct player_list_t *list, struct player_alias_t *player)
{
    struct player_node_t *curr = NULL, *prev = NULL;
    struct player_node_t *newnode = NULL;

    if (list == NULL || player == NULL)
    {
        return -1;
    }
    
    if ((newnode = malloc(sizeof(struct player_node_t))) == NULL)
    {
        return -1;
    }
    newnode->player = player;
    newnode->next = NULL;

    pthread_mutex_lock(list->lock);

    // the list is empty
    if (list->first == NULL)
    {
        list->first = newnode;
    }
    // it is not empty, go to the end
    else
    {
        curr = list->first;
        prev = curr;
        while (curr != NULL)
        {
            // meanwhile if you find the player is already in
            if (strcmp(curr->player->username, player->username) == 0)
            {
                // let them rejoin if they had left
                if (curr->player->active == 0)
                {
                    pthread_mutex_unlock(list->lock);
                    free(curr->player->address);
                    player_alias_copyOver(curr->player, player);
                    player_alias_destroy(player);
                    free(newnode);
                    return 2;
                }
                // or prevent them from joining if they were still active in game
                else
                {
                    pthread_mutex_unlock(list->lock);
                    free(newnode);
                    return 0;
                }
            }
            prev = curr;
            curr = curr->next;
        }
        // reached the end. Put new player in
        prev->next = newnode;
    }
    pthread_cond_signal(list->notEmpty);
    pthread_mutex_unlock(list->lock);
    return 1;
}

void player_list_purge(struct player_list_t *list)
{
    struct player_node_t *curr, *prev = NULL;
    pthread_mutex_lock(list->lock);
    if (list->first != NULL)
    {

        curr = list->first;
        prev = list->first;

        while (curr != NULL)
        {
            curr->player->x = 0;
            curr->player->y = 0;
            curr->player->points = 0;
            curr->player->box = 0;
            curr->player->duration = 0;
            personal_map_clear(curr->player->map);

            if (!(curr->player->active))
            {
                // the player to remove is in first position
                if (curr == list->first)
                {
                    list->first = list->first->next;
                    free(curr->player->address);
                    player_alias_destroy(curr->player);
                    free(curr);
                    curr = list->first;
                    prev = curr;
                }
                // every other position except for last
                else
                {
                    prev->next = curr->next;
                    free(curr->player->address);
                    player_alias_destroy(curr->player);
                    free(curr);
                    curr = prev->next;
                }
            }
        }
        
        prev->player->x = 0;
        prev->player->y = 0;
        prev->player->points = 0;
        prev->player->box = 0;
        prev->player->duration = 0;
        personal_map_clear(prev->player->map);

        // reached the last position
        if (!(prev->player->active))
        { //if the last player in the list is inactive, it will be removed
            free(prev->player->address);
            player_alias_destroy(prev->player);
            free(prev);
        }
        pthread_mutex_unlock(list->lock);
    }
    return;
}

void player_list_waitOnEmpty(struct player_list_t *list)
{
    pthread_mutex_lock(list->lock);
    while (list->first == NULL)
    {
        pthread_cond_wait(list->notEmpty, list->lock);
    }
    pthread_mutex_unlock(list->lock);
}

int player_list_tick(struct player_list_t *list)
{
    struct player_list_iterator_t *iterator;
    struct player_alias_t *player;
    int i = 0;
    int expired = 0;

    player_list_iterator_create(list, &iterator);

    while (i != 1)
    {
        player = player_list_iterator_next(iterator, &i);

        if (player->box != 0)
        {
            player->duration -= 1;
            if (player->duration == 0)
            {
                player->box = 0;
                expired += 1;
            }
            
        }
        
    }

    player_list_iterator_destroy(iterator);
    return expired;
}

void player_list_resetPlayers(struct player_list_t *list)
{
    struct player_node_t *curr = NULL;

    if(list == NULL)
    {
        exit(1);
    }

    pthread_mutex_lock(list->lock);

    if(list->first == NULL)
    {
        pthread_mutex_unlock(list->lock);
        return;
    }

    curr = list->first;

    while(curr != NULL)
    {
        curr->player->x = 0;
        curr->player->y = 0;
        curr->player->points = 0;
        curr->player->box = 0;
        curr->player->duration = 0;
        personal_map_clear(curr->player->map);

        curr = curr->next;
    }
    pthread_mutex_unlock(list->lock);
    return;
}

int player_list_count(struct player_list_t *list)
{
    struct player_node_t *curr;
    int n = 0;

    pthread_mutex_lock(list->lock);
    if (list->first == NULL)
    {
        pthread_mutex_unlock(list->lock);
        return 0;
    }
    
    curr = list->first;

    while (curr != NULL)
    {
        n++;
        curr = curr->next;
    }
    pthread_mutex_unlock(list->lock);
    return n;
}
