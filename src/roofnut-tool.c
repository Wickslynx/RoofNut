#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "--version") == 0) {
            printf("(V) RoofNut v1.1.2 \n(L) MIT license. \nBuild: Stable-release. \nSupport: Wickslynx@yahoo.com\n");
        }
    } else {
        printf("Please specify a command [--version --force-update --force-quit --logs]\n");
    }
    return 0;
}
