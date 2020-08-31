#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

/* macro definitions */
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30
#define PORT_NUMBER_POSITION 2

/* Global variables */
int socketDescriptor = 0;

/* Forward declarations */
void game(int socketDescriptor);
void connection(int *socketDescriptor, struct sockaddr_in *address);
void userQuery(int socketDescriptor);
void checkSocketError(ssize_t error, int sockDescriptor);
uint16_t extractPortNumber(const char **argv);
void checkArgumentNumber(int argc);
int isInteger(const char *string);
void bufferToString(char *buffer, char *string);
void preparaIndirizzo(struct sockaddr_in *indirizzo, const char *stringaIP, uint16_t numeroPorta);
void clearBuffer(void);
void gameAccessError(char response);

int main(int argc, const char *argv[])
{
	struct sockaddr_in address;

	preparaIndirizzo(&address, argv[1], extractPortNumber(argv));
	connection(&socketDescriptor, &address);
	userQuery(socketDescriptor);
	//game(socketDescriptor);
}

void checkArgumentNumber(int argc)
{
	if (argc != 3)
	{
		fprintf(stderr, "Invalid arguments. First argument must be the server address and second must be the port.\n");
		exit(1);
	}
}

void preparaIndirizzo(struct sockaddr_in *indirizzo, const char *stringaIP, uint16_t numeroPorta)
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

void game(int socketDescriptor)
{	
	char message;
	read(socketDescriptor, &message, 1);
}

void connection(int *socketDescriptor, struct sockaddr_in *address)
{
	struct sockaddr *addr = (struct sockaddr *) address;
	int connError = 0;
	*socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if((*socketDescriptor) == -1)
	{
		perror("Socket creation error");
		exit(1);
	}
	connError = connect((*socketDescriptor), addr, (socklen_t) sizeof(struct sockaddr));
	if (connError != 0)
	{
		perror("Socket connection error");
		fprintf(stderr, "Could not connect on socket.\n");
		close(*socketDescriptor);
		exit(1);
	}
}

void userQuery(int socketDescriptor)
{
	char username[USERNAME_SIZE];
	char password[PASSWORD_SIZE];
	char buffer[100];
	char command, response;
	int connected = 0, commandValid = 0;
	int stringSize = (int) htonl(USERNAME_SIZE);
	ssize_t error;

	while(!commandValid)
	{
		printf("Would you like to register ('r') or login ('l') ?\n");
		command = getchar();
		clearBuffer();

		if(command == 'r' || command == 'l')
			commandValid = 1;
		else
			printf("Not a valid command.\n");

	}

	while(!connected)
	{
		printf("Insert username (max %d characters, 'q' to exit):\n", USERNAME_SIZE-1);
		scanf("%s", buffer);

		bufferToString(buffer, username);

		if(strcmp(username, "q") == 0)
		{
			error = write(socketDescriptor, username, 1);
			checkSocketError(error, socketDescriptor);
			close(socketDescriptor);
			exit(0);
		}

		printf("Insert password (max %d characters):\n", PASSWORD_SIZE-1);
		scanf("%s", buffer);

		bufferToString(buffer, password);

		error = write(socketDescriptor, &command, 1);
		checkSocketError(error, socketDescriptor);

		error = write(socketDescriptor, &stringSize, 4);
		checkSocketError(error, socketDescriptor);

		error = write(socketDescriptor, username, USERNAME_SIZE);
		checkSocketError(error, socketDescriptor);

		error = write(socketDescriptor, &stringSize, 4);
		checkSocketError(error, socketDescriptor);

		error = write(socketDescriptor, password, PASSWORD_SIZE);
		checkSocketError(error, socketDescriptor);
		
		error = read(socketDescriptor, &response, 1);
		checkSocketError(error, socketDescriptor);

		if(response == 't') 
		{
			connected = 1;
			printf("%s successful!\n", (command == 'r')? "Registration" : "Login");
		} 
		else
		{
			printf("Could not %s successfully.\n", (command == 'r')? "register" : "login");
			gameAccessError(response);
			}		
	}

	if (command == 'r')
		close(socketDescriptor);
		exit(0);
	
	return;
}

void gameAccessError(char response)
{
	switch (response)
	{
	case 'a':
		printf("Error: player already connected!\n");
		break;
	case 'f':
		printf("Error: invalid username or password.\n");
	default:
		break;
	}
}

void clearBuffer(void)
{    
  while ( getchar() != '\n' );
}

void bufferToString(char *buffer, char *string)
{
	int i = 0;
	for(i = 0; i < 29 && buffer[i] != '\0'; i++)
	{
		string[i] = buffer[i];
	}
	string[i] = '\0';
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

uint16_t extractPortNumber(const char **argv)
{
    if (!isInteger(argv[PORT_NUMBER_POSITION]))
    {
        fprintf(stderr, "Port number should be an integer\n");
    }

    int numero = atoi(argv[PORT_NUMBER_POSITION]);
    if (numero < 1 || numero > 65535) {
        fprintf(stderr, "Port number is out of range (1-65535)\n");
    }
    return (uint16_t)numero;
}

int isInteger(const char *string)
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
