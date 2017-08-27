// Writes a sindata.h file for pre-generated sin tables

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

const int range = 320;
const int maxval = 16;

float lerp(float a, float b, float alpha)
{
	return a * (1.0f - alpha) + alpha * b;
}

int main(int argc, char** argv)
{
	FILE* f = fopen("./sindata.h", "w");
	if (f == NULL)
		return 1;

	uint8_t val;
	
	fprintf(f, "u16 SIN_COUNT = %d;\n", range);
	fprintf(f, "const u8 SINDATA[%d] = {\n", range);

	float sin_input = 0.f;
	float sinval = 0.f;
	int i = 0;
	for (int i = 0; i < range; i++) {
		sin_input = lerp(0.0, M_PI*2, i / 320.f);
		sinval = (sin(sin_input) + 1.0f) * maxval / 2;
		if (sinval < 1)
			sinval = 1;
		fprintf(stderr, "sinval of %f is %f\n", sin_input, sinval);
		if (i < range-1)
			fprintf(f, "%d,", (uint8_t)sinval);
		else
			fprintf(f, "%d };\n", (uint8_t)sinval);
	}

	fclose(f);
	return 0;
}

