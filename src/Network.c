
#include "Network.h"
#include <stdlib.h>
#include <netinet.c>
#include <sys/socket.h>
#include <unistd.h>

void createSocket(RoofnutServer *self, const int port, const bool listening) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
    if (listening = true) {
          listen(serverAddr, 500);
          clientSocket = accept(serverSocket, ,0)
    } else {
        close(serverSocket);
    }

}

//Work in progress, I should prob js scrap this.