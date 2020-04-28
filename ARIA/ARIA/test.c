#include <stdio.h>
#include "ARIA.h"
int main() {

	u8 temp[2] = { 0x63, 0x52 };
	temp[0] >>= 3;
	temp[1] >>= 3;

	printf("%02x, %02x \n", temp[0], temp[1]);

	return 0;
}