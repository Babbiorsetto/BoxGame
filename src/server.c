#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include "player_list.h"

/* macro definitions */
#define ARGUMENT_NUMBER 2
#define PORT_NUMBER_POSITION 1
#define CONNECTION_QUEUE_SIZE 5

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

void *handleConnection(void *data)
{
    char command;
    int error;
    int receive;
    int dataSize;
    char username[30];
    char password[30];

    struct connection_info *connectionInfo = (struct connection_info *)data;
    int clientDescriptor = connectionInfo->fd;

    // read command, either 'i' or 'a'
    error = read(clientDescriptor, &command, 1);
    if (error != 1)
    {
        // TODO do what? what would this even mean?
    }
    // read username length
    error = read(clientDescriptor, &receive, sizeof(int));
    if (error)
    {
        //TODO
    }
    dataSize = ntohl(receive);
    // read username
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
        break;
    case 'i':
        //TODO save credentials and terminate connection
        break;
    default:
        //TODO tell client this shouldn't have even happened
        break;
    }
    

    


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
