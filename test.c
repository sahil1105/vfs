

#include <stdio.h>


int main(void) {
	unsigned char c = 0x01;
	unsigned char *pc = &c;

	for (int i = 0; i < 20; i++, *pc <<= 1) {
		if (!*pc) {
			printf("resetting: ");
			*pc = 1;
		}

		int i = *pc;
		printf("i: %x\n", i);
	}

	return 0;
}
