#include <sys/socket.h>
#include <netinet/in.h>

void processArguments(int, char const**, struct sockaddr_in);

int main(int argc, char const *argv[]) {
    
    struct sockaddr_in serverAddress;
    
    processArguments(argc, argv, &serverAddress);

    startGameThread();
    waitForConnections();

    return 0;
}

void processArguments(int argc, char const** argv, struct sockaddr_in *address) {
    checkArgumentNumber(argc);
    int portNumber = extractPortNumber(argc, argv);
}
