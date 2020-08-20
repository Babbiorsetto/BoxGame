#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "player_list.h"
#include "player_alias.h"

/* macro definitions */
#define ARGUMENT_NUMBER 2
#define PORT_NUMBER_POSITION 1
#define CONNECTION_QUEUE_SIZE 5
#define USER_CREDENTIALS_FILE "userCredentials"
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30
#define USER_FILE_CREATE_PERMISSIONS S_IRUSR | S_IWUSR
#define USER_FILE_OFLAGS O_WRONLY | O_CREAT | O_APPEND

/* function declarations */
void processArguments(int argc, char const **argv, struct sockaddr_in *address);
void checkArgumentNumber(int argc, char const **argv);
uint16_t extractPortNumber(char const **argv);
int isInteger(char *string);
void gameError(char const *functionName, char const *customMessage, int shouldExit, int exitStatus);
void setupMemory(struct sockaddr_in *address, struct player_list_t *list);
struct sockaddr_in *makeNewAddress();
void waitForConnections(struct sockaddr_in *address);
pthread_t startGameThread();
void *handleConnection(void *data);
void *game(void *arg);

/**/
struct connection_info{
    int fd;
    struct sockaddr_in *address;
};

/* global */
struct player_list_t *playerList;
int usersFile;
pthread_mutex_t *fileLock;

int main(int argc, char const *argv[])
{

    struct sockaddr_in *serverAddress;

    setupMemory(serverAddress, playerList);
    processArguments(argc, argv, serverAddress);
    startGameThread();
    waitForConnections(serverAddress);

    return 0;
}

void processArguments(int argc, char const **argv, struct sockaddr_in *address)
{
    checkArgumentNumber(argc, argv);
    uint16_t portNumber = extractPortNumber(argv);
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    address->sin_addr.s_addr = INADDR_ANY;
}

void checkArgumentNumber(int argc, char const **argv)
{
    if (argc != ARGUMENT_NUMBER)
    {
        fprintf(stderr, "%s: usage: %s PORT_NUMBER\n", argv[0], argv[0]);
        exit(1);
    }
}

uint16_t extractPortNumber(char const **argv)
{
    if (!isInteger(argv[PORT_NUMBER_POSITION]))
    {
        gameError("command line", "PORT_NUMBER should be int", 1, 1);
    }

    int numero = atoi(argv[PORT_NUMBER_POSITION]);
    if (numero < 1 || numero > 65535) {
        gameError("command line", "PORT_NUMBER is out of range (1-65535)", 1, 1);
    }
    return (uint16_t)numero;
}

int isInteger(char *string)
{
    int x = 0;
    int len = strlen(string);

    while (x < len)
    {
        if (!isdigit(string[x]) && x > 0)
        {
            return 0;
        }
        else if (!isdigit(string[x]) && x == 0)
        {
            if (string[0] != '-')
            {
                return 0;
            }
        }
        x++;
    }

    return 1;
}

/*
* Prints a game error and optionally exits the program
*
* @param functionName The name of the function that caused the error
* @param customMessage An additional message to append to the error report
* @param shouldExit Indicates if the program should terminate after printing
* @param exitStatus The exit status to use if exiting
*/
void gameError(char const *functionName, char const *customMessage, int shouldExit, int exitStatus)
{
    perror(functionName);
    fprintf(stderr, "%s error: %s\n", functionName, customMessage);
    if (shouldExit)
    {
        exit(exitStatus);
    }
    
}

/*
* Creates all needed data structures for the start of the game. Namely:
* - global playerList to hold the players currently in game and their order
* - server address struct
* - Maybe something else in the future
*
*/
void setupMemory(struct sockaddr_in *address, struct player_list_t *list)
{
    int error = player_list_create(list);
    if (error)
    {
        gameError("player_list_create", "cannot create player list", 1, 1);
    }

    address = makeNewAddress();
    if (address == NULL)
    {
        gameError("makeNewAddress", "cannot allocate server address", 1, 1);
    }

    usersFile = open(USER_CREDENTIALS_FILE, USER_FILE_OFLAGS, USER_FILE_CREATE_PERMISSIONS);
    if (usersFile == -1)
    {
        gameError("open", strcat("cannot open ", USER_CREDENTIALS_FILE), 1, 1);
    }

    fileLock = malloc(sizeof(pthread_mutex_t));
    if (fileLock == NULL)
    {
        gameError("malloc", "cannot instantiate file lock", 1, 1);
    }
    error = pthread_mutex_init(fileLock, NULL);
    if (error != 0)
    {
        gameError("pthread_mutex_init", "cannot initialize file lock", 1, 1);
    }
    
    
}

struct sockaddr_in *makeNewAddress()
{
    return calloc(1, sizeof(struct sockaddr_in));
}

/*
* Create thread for the game and start it
*
* @return The tid of the newly created game thread
*/
pthread_t startGameThread()
{
    int error;
    pthread_t newId;

    error = pthread_create(newId, NULL, game, NULL);
    if (error)
    {
        gameError("pthread_create", "cannot create game thread", 1, 1);
    }
    return newId;
    
}

/*
* Creates socket, binds address to it, listens for connections, accepts connections
* and delegates handling them to new thread started in the detached state
* 
* @param address The address on which to listen for connections
*/
void waitForConnections(struct sockaddr_in *address)
{
    int addrLen;
    struct sockaddr_in *address;
    pthread_attr_t threadAttributes;
    struct connection_info *connectionInfo;

    int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor == -1)
    {
        gameError("socket", "cannot create socket", 1, 1);
    }
    
    int error = bind(socketDescriptor, address, sizeof(struct sockaddr_in));
    if (error)
    {
        gameError("bind", "cannot bind address to socket", 1, 1);
    }
    
    error = listen(socketDescriptor, CONNECTION_QUEUE_SIZE);
    if (error)
    {
        gameError("listen", "cannot listen on the socket", 1, 1);
    }

    error = pthread_attr_init(&threadAttributes);
    if (error)
    {
        gameError("pthread_attr_init", "cannot initialize thread attribute", 1, 1);
    }
    error = pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_DETACHED);
    if (error)
    {
        gameError("pthread_attr_setdetachstate", "cannot set detach state on thread attribute", 1, 1);
    }
    
    while (1)
    {
        address = makeNewAddress();
        if (address == NULL)
        {
            gameError("makeNewAddress", "cannot allocate client address", 1, 1);
        }
        addrLen = sizeof(struct sockaddr_in);
        error = accept(socketDescriptor, address, &addrLen);
        if (error == -1)
        {
            gameError("accept", "cannot accept new connection", 1, 1);
        }
        connectionInfo = malloc(sizeof(struct connection_info));
        if (connectionInfo == NULL)
        {
            gameError("malloc", "cannot allocate connection_info struct", 1, 1);
        }
        connectionInfo->address = address;
        connectionInfo->fd = error;
        pthread_create(NULL, &threadAttributes, handleConnection, connectionInfo);
        
    }
    
}

/*
* Yes, synchronization on the credentials file is done at this level. 
* Sue me
* 
* @param data A struct connection_info pointer coming from an established connection
*/
void *handleConnection(void *data)
{
    char command;
    char response;
    int error;
    int receive;
    int dataSize;
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];

    struct connection_info *connectionInfo = (struct connection_info *)data;
    int clientDescriptor = connectionInfo->fd;

    // read command, either 'i' or 'a'
    error = read(clientDescriptor, &command, 1);
    if (error != 1)
    {
        // TODO do what? what would this even mean?
    }
    int again = 1;
    while (again)
    {
        // read username length
        error = read(clientDescriptor, &receive, sizeof(int));
        if (error)
        {
            //TODO
        }
        dataSize = ntohl(receive);
        // read username. Must also contain \0
        error = readNBytes(clientDescriptor, username, dataSize);
        if (error != dataSize)
        {
            //TODO
        }
        // read password length
        error = read(clientDescriptor, &receive, sizeof(int));
        if (error)
        {
            //TODO
        }
        dataSize = ntohl(receive);
        // read password
        error = readNBytes(clientDescriptor, password, dataSize);
        if (error != dataSize)
        {
            //TODO
        }
        
        switch (command)
        {
        case 'a':
            //TODO let user log in and put them in the game
            pthread_mutex_lock(fileLock);
            // player is registered
            if (checkCredentials(username, password))
            {
                pthread_mutex_unlock(fileLock);
                struct player_alias_t *alias = player_alias_create(username, connectionInfo->address, clientDescriptor);
                if (alias == NULL)
                {
                    //TODO this is a really bad situation. System must be out of memory 
                }
                
                // either they're already present and active or can't be added for some reason
                if(!player_list_add(playerList, alias))
                {
                    response = 'a';
                    player_alias_destroy(alias);
                    write(clientDescriptor, response, 1);
                }
                // they are added
                else
                {
                    response = 't';
                    free(connectionInfo);
                    again = 0;
                    write(clientDescriptor, response, 1);
                }
                
            }
            else
            {
                pthread_mutex_unlock(fileLock);
                response = 'f';
                write(clientDescriptor, response, 1);
            }
            
            
            pthread_mutex_unlock(fileLock);
            break;
        case 'i':
            pthread_mutex_lock(fileLock);
            // save credentials and terminate connection
            if (!isUsernamePresent(username))
            {
                insertUserCredentials(username, password);
                pthread_mutex_unlock(fileLock);
                response = 't';
                write(clientDescriptor, response, 1);
                close(clientDescriptor);
                free(connectionInfo->address);
                free(connectionInfo);
                again = 0;
            }
            // warn client and go back to getting credentials
            else
            {
                pthread_mutex_unlock(fileLock);
                response = 'f';
                write(clientDescriptor, response, 1);
            }
            break;
        default:
            //TODO tell client this shouldn't have even happened
            break;
        }

    }
    return NULL;
}

/*
* Checks if given string contains a username already present
* in the user credentials file
* 
* @param username 
* @return 1 if the username is already present, 0 if it isn't. -1 shouldn't ever be returned
*/
int isUsernamePresent(char *username)
{
    char buffer[USERNAME_SIZE];
    char trash[PASSWORD_SIZE];
    int error;
    int fd = open(USER_CREDENTIALS_FILE, O_RDONLY);
    if (fd == -1)
    {
        //TODO
    }

    while (1)
    {
        error = read(fd, buffer, USERNAME_SIZE);
        if (error == -1)
        {
            //TODO
        }
        else if (error == 0)
        {
            return 0;
        }   

        if (strcmp(username, buffer) == 0)
        {
            return 1;
        }

        error = read(fd, trash, PASSWORD_SIZE);
        if (error == -1)
        {
            //TODO
        }
    
    }

    return -1;
}

int insertUserCredentials(char *username, char *password)
{
    int error = write(usersFile, username, USERNAME_SIZE);
    if (error == -1)
    {
        //TODO uh oh! This is bad
    }
    error = write(usersFile, password, PASSWORD_SIZE);
    if (error == -1)
    {
        //TODO
    }

    return 0;
}

/*
* Check that the user corresponding to the given info is present 
* in the user credentials file
*
* @param username The username to check for
* @param password The password corresponding to the username
* @return 1 if the username-password combination is present in the credentials file, 0 otherwise
*/
int checkCredentials(char *username, char *password)
{
    /*TODO*/
}
/*
* To be reasonably sure that n bytes were actually read into buffer
* TODO
*/
int readNBytes(int fileDescriptor, void *buffer, int nbytes)
{
    int error;
    int remaining = nbytes;
    while (remaining)
    {
        error = read(fileDescriptor, buffer + (nbytes - remaining), remaining);
        if (error == -1 || error == 0)
        {
            return -1;
        }
        remaining -= error;
    }
    
    return nbytes;
}

/*Here be game*/

/*
* Entry point for the game loop
*
* @param arg Mandatory argument for a pthread routine
* @return Probably nothing
*/
void *game(void *arg)
{
    /*body*/
}
