#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void Keyschedule(unsigned char MK[], unsigned char WK[], unsigned char RK[]) {
	WK[0] = MK[12]; WK[1] = MK[13]; WK[2] = MK[14]; WK[3] = MK[15];
	WK[4] = MK[0]; WK[5] = MK[1]; WK[6] = MK[2]; WK[7] = MK[3];

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			RK[16 * i + j] = MK[(j - i + 8) % 8] + S[16 * i + j];
		}
		for (int j = 0; j < 8; j++) {
			RK[16 * i + j + 8] = MK[(j - i + 8) % 8 + 8] + S[16 * i + j + 8];
		}
	}
}

HIGHT_ENC(unsigned char PT[], unsigned char RK[], unsigned char CT[]) {

}
int main() {
	unsigned char PT[8];
	unsigned char MK[16];
	unsigned char CT[8];
	unsigned char RK[1000];
	unsigned char WK[8];
	Keyschedule(MK, WK, RK);
	HIGHT_ENC(PT, RK, CT);
}