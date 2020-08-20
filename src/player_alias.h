#include <netinet/in.h>

struct player_alias_t
{
    char *username;
    struct sockaddr_in *address;
    int connection;
    int active;
};

struct player_alias_t *player_alias_create(char *username, struct sockaddr_in *address, int clientDescriptor);

void player_alias_destroy(struct player_alias_t *alias);