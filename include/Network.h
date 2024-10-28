#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct RoofNutServer {
    void* (*createSocket) (struct RoofNutServer *self, const int port, const bool listening);

} RoofNutServer;

void createSocket(RoofNutServer *self, const int port, const bool listening);
