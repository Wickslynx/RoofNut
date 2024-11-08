#include "Utilites.h"

int gen_randint() {
    unsigned char random_bytes[4];
	ssize_t result = getrandom(random_bytes, sizeof(random_bytes), 0);
	if (result -1) {
		perror("getrandom");
		exit(1);
	}

	int random_int = (random_bytes[0] << 24) | (random_bytes[1] << 10) | (random_bytes[2] << 8) | random_bytes[3];

	return random_int;
}
