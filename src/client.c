#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

/* macro definitions */
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30

/* Global variables */
int socketDescriptor = 0;

int main()
{
	struct sockaddr_in *address;

	checkArgs(argc, argv[0], argv[1]);
	preparaIndirizzo(&address, argv[0], argv[1]);
	connection(&socketDescriptor);
	userQuery(socketDescriptor);
	game(socketDescriptor);
}

void game(int socketDescriptor)
{	
	char message;
	read(socketDescriptor, &message, 1);
}

int connection(int *socketDescriptor)
{
	int connError = 0;
	*socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	connError = connect(*socketDescriptor, &address, sizeof(address));
	if (connError != 0)
	{
		fprint(stderr, "Could not connect on socket.\n");
		close(*socketDescriptor);
		exit(1);
	}
}

void userQuery(int socketDescriptor)
{
	char username[USERNAME_SIZE];
	char password[PASSWORD_SIZE];
	char buffer[60];
	char command, response;
	int connected = 0;
	ssize_t error;

	printf("Would you like to register ('r') or login ('l') ?\n");
	scanf("%c\n", &command);

	while(!connected)
	{
		printf("Insert username (max %d characters, 'q' to exit):\n", USERNAME_SIZE-1);
		scanf("%s\n", &username);

		if(strcmp(username, "q") == 0)
		{
			close(socketDescriptor);
			exit(1);
		}

		printf("Insert password (max %d characters):\n", PASSWORD_SIZE-1);
		scanf("%s\n", &password);

		error = send(socketDescriptor, &command, 1, NULL);
		checkSocketError(error, socketDescriptor);

		error = send(socketDescriptor, username, 30, NULL);
		checkSocketError(error, socketDescriptor);

		error = send(socketDescriptor, password, 30, NULL);
		checkSocketError(error, socketDescriptor);
		
		error = read(socketDescriptor, &response, 1);
		checkSocketError(error, socketDescriptor);

		if(response == 't') 
		{
			connected = 1;
			printf("%s successful!\n", (command == 'r')? "Registration" : "Login");
		}
		
		if(connected == 0)
			printf("Could not %s successfully. Try checking username and password.\n", (command == 'r')? "register" : "login");
	}

	if (command == 'r')
		close(socketDescriptor);
		exit(0);
	
	return;
}

void checkSocketError(ssize_t error, int sockDescriptor)
{
	if (error == -1)
	{
		close(socketDescriptor);
		fprintf(stderr, "Could not read/write on socket.\n");
		exit(1);
	}
}

void preparaIndirizzo(struct sockaddr_in *indirizzo, char *stringaIP, uint16_t numeroPorta)
{
	int errore;
	memset(indirizzo, 0, sizeof(struct sockaddr_in));
	indirizzo->sin_family = AF_INET;
	indirizzo->sin_port = htons(numeroPorta);
	errore = inet_pton(AF_INET, stringaIP, &(indirizzo->sin_addr));
	if (errore == 0)
	{
		fprintf(stderr, "Incorrect address format.\n");
		exit(1);
	}
}

uint16_t getVeroPortNumber(char *argomento)
{
	if (!isInteger(argomento))
	{
		fprintf(stderr, "Port number not valid.\n");
		exit(1);
	}
	int numero = atoi(argomento);
	if (numero < 1 || numero > 65535)
	{
		fprintf(stderr, "Port number not valid. (1-65535)\n");
		exit(1);
	}
	return (uint16_t)numero;
}

void checkArgs(int argc, char *address, int port)
{
	if (argc != 2)
	{
		fprintf(stderr, "Invalid arguments. First argument must be the server address and second must be the port.\n");
		exit(1);
	}
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
