#include <stdio.h>


void out_log (const char *s) {
	fprintf (stderr, "log: %s\n", s);
	fflush(stderr);
}

int getRandom(void *out, size_t len) {
	FILE *urandom = fopen("/dev/urandom", "r");
	if (urandom != NULL) {
		if (fread(out, 1, len, urandom) == len) {
			fclose(urandom);
			return 1;
		}
		fclose(urandom);
	}
	return 0;
}

