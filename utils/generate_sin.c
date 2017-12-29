/*
 * Writes a pre-generated sine tables on stdout.
 * example:
 *	$ ./generate_sin 20 1 10 | paste -sd, -
 *	5,6,7,8,8,9,8,8,7,6,5,3,2,1,1,1,1,1,2,3
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <err.h>
#include <limits.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *outfile;
	long values[3];
	if (argc != 4)
		errx(1, "usage: generate_sin count min max\n");

	for (int i = 0; i < argc - 1; i++) {
		char *ep;
		char *s = argv[i+1];
		/* strtol sucks */
		errno = 0;
		values[i] = strtol(s, &ep, 10);
		if (s[0] == '\0' || *ep != '\0')
			errx(1, "invalid number '%s'", s);
		if (errno)
			err(1, "strtol: '%s'", s);
	}
	if (values[0] < 0)
		err(1, "count must be non-negative");
	unsigned long count = values[0];
	long minval = values[1];
	long maxval = values[2];
	if (minval >= maxval)
		errx(1, "max must be larger than min");
	/* codomain is [-1,1] so its range is 2 */
	unsigned long scale = (maxval - minval) / 2;

	for (unsigned long i = 0; i < count; i++) {
		double sn = (double) i / count * 2*M_PI;
		/* adjust the sine value to [0,2] before scaling */
		long sinval = minval + (1 + sin(sn)) * scale;
		printf("%ld\n", sinval);
	}

	return 0;
}

