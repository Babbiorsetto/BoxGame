#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ARGUMENT_NUMBER 2
#define PORT_NUMBER_POSITION 1

void processArguments(int argc, char const **argv, struct sockaddr_in *address);
void checkArgumentNumber(int argc, char const **argv);
uint16_t extractPortNumber(char const **argv);
int isInteger(char *string);
    
int main(int argc, char const *argv[])
{

    struct sockaddr_in serverAddress;
    
    processArguments(argc, argv, &serverAddress);

    startGameThread();
    waitForConnections();

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

}
