#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct RoofNutServer {
    void (*createSocket) (struct RoofNutServer *self, const int port, const bool listening);
    void (*listen)(struct RoofNutServer *self, const int socket)
} RoofNutServer;

void createSocket(RoofNutServer *self, const int port, const bool listening);
