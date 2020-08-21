#include "player_list.h"
#include "player_alias.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

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

int player_list_is_empty(struct player_list_t *list)
{
    int check = 0;
    pthread_mutex_lock(list->lock);
    check = (list->first == NULL);
    pthread_mutex_unlock(list->lock);
    return check;
}

int player_list_iterator_create(struct player_list_t *list, struct player_list_iterator_t *iterator)
{
    if ((iterator = malloc(sizeof(struct player_list_iterator_t))) == NULL)
        return -1;

    else
    {
        iterator->list = list;
        if (!(player_list_is_empty(list)))
        { //if the list is empty, the iterator will be freed
            iterator->current = iterator->list->first;
        }
        else
        {
            free(iterator);
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

int player_list_create(struct player_list_t *list)
{
    if ((list = malloc(sizeof(struct player_list_t))) == NULL)
    {
        return -1;
    }

    if ((list->lock = malloc(sizeof(pthread_mutex_t))) == NULL)
    {
        free(list);
        return -2;
    }

    list->first = NULL;
    pthread_mutex_init(list->lock, NULL);
    return 0;
}

int player_list_add(struct player_list_t *list, struct player_alias_t *player)
{
    struct player_node_t *curr = NULL;
    struct player_node_t *newnode = NULL;

    if (list == NULL || player == NULL)
    {
        return -1;
    }
    else
    {
        pthread_mutex_lock(list->lock);

        if ((newnode = malloc(sizeof(struct player_node_t))) == NULL)
        {
            return -2;
        }
        newnode->player = player;
        newnode->next = NULL;

        if (list->first == NULL)
        {
            list->first = newnode;
        }
        else
        {
            curr = list->first;
            while (curr->next != NULL)
            {

                if ((strcmp(curr->player->username, player->username)) != 0)
                {
                    pthread_mutex_unlock(list->lock);
                    return 0;
                }

                curr = curr->next;
            }

            curr->next = newnode;
        }

        pthread_mutex_unlock(list->lock);
    }
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

        while (curr->next != NULL)
        {

            if (!(curr->player->active))
            {

                if (curr == list->first)
                {
                    list->first = list->first->next;
                    player_alias_destroy(curr->player);
                    free(curr);
                    curr = list->first;
                    prev = curr;
                }
                else
                {
                    prev->next = curr->next;
                    player_alias_destroy(curr->player);
                    free(curr);
                    curr = prev->next;
                }
            }

            prev = curr;
            curr = curr->next;
        }

        if (!(curr->player->active))
        { //if the last player in the list is inactive, it will be removed
            player_alias_destroy(curr->player);
            free(curr);
            prev = NULL;
        }
        pthread_mutex_unlock(list->lock);
    }
    return;
}
