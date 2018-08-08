#include <stdio.h>
#include <stdint.h>

int main()
{
	struct bitbit
	{
		uint8_t a : 7;
		uint8_t b : 1;
	};

	union bitunion
	{
		uint8_t c;
		bitbit d;
	};

	union bitunion e;
	e.c = 0x00;
	e.d.b = 1;

	printf("0x%x\n", e.c);
	return 0;
}
