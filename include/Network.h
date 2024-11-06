#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct RoofNutServer {
    void (*createSocket) (struct RoofNutServer *self, const int port, const bool listening);
    void (*listen)(struct RoofNutServer *self, const int socket)

} RoofNutServer;

void createSocket(RoofNutServer *self, const int port, const bool listening);
