#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
void SEED_ENC(unsigned int PT[], unsigned int RK[], unsigned int CT[]) {
	int i = 0;
	for (; i < 15; i++) {
		unsigned int temp[2] = { PT[2], PT[3] };
		Xor64(PT, F_function(&PT[2], &RK[2 * i]));
	}
	
}
int main() {
	unsigned int PT[4] = { 0x00000000, };
	unsigned int CT[4] = { 0x00000000, };
	unsigned int RK[32] = { 0x00000000, };
	unsigned int MK[4] = { 0x00000000, };

	SEED_KeySchedule();
	SEED_ENC(PT, RK, CT);
	return 0;
}