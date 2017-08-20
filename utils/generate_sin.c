// Writes a sindata.h file for pre-generated sin tables

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

const int range = 128;
const int maxval = 16;

int main(int argc, char** argv)
{
	FILE* f = fopen("./sindata.h", "w");
	if (f == NULL)
		return 1;

	uint8_t val;
	
	fprintf(f, "u8 SIN_COUNT = %d;\n", range);
	fprintf(f, "const u8 SINDATA[%d] = {\n", range);

	float sin_input = 0.f;
	float sinval = 0.f;
	int i = 0;
	for (int i = 0; i < range; i++, sin_input += 0.1f) {
		sinval = (sin(sin_input) + 1.0f) * maxval / 2;
		fprintf(stderr, "sinval: %f\n", sinval);
		if (i < range-1)
			fprintf(f, "%d,", (uint8_t)sinval);
		else
			fprintf(f, "%d };\n", (uint8_t)sinval);
	}

	fclose(f);
	return 0;
}

