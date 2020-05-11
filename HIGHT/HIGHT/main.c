#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define F0(X) ((((X) << 1) ^ ((X) >> 7)) ^ (((X) << 2) ^ ((X) >> 6)) ^ (((X) << 7) ^ ((X) >> 1))) & 0xff
#define F1(X) ((((X) << 3) ^ ((X) >> 5)) ^ (((X) << 4) ^ ((X) >> 4)) ^ (((X) << 6) ^ ((X) >> 2))) & 0xff

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



HIGHT_ENC(unsigned char PT[], unsigned char WK[], unsigned char RK[], unsigned char CT[]) {
	unsigned char X[8] = { PT[0] + WK[0], PT[1], PT[2] + WK[1], PT[3], PT[4] + WK[2], PT[5], PT[6] + WK[3], PT[7], };
	unsigned char temp[8];
	int i;
	for (i = 1; i <= 30; i+2) {
		temp[0] = (X[7] ^ (F0(X[6]) + RK[4 * i - 1]));
		temp[2] = (X[1] + (F0(X[0]) ^ RK[4 * i - 4]));
		temp[4] = (X[3] ^ (F0(X[2]) + RK[4 * i - 3]));
		temp[6] = (X[5] + (F0(X[4]) ^ RK[4 * i - 2]));
		temp[1] = X[0]; temp[3] = X[2]; temp[5] = X[4]; temp[7] = X[6];

		X[0] = (temp[7] ^ (F0(temp[6]) + RK[4 * i - 1]));
		X[2] = (temp[1] + (F0(temp[0]) ^ RK[4 * i - 4]));
		X[4] = (temp[3] ^ (F0(temp[2]) + RK[4 * i - 3]));
		X[6] = (temp[5] + (F0(temp[4]) ^ RK[4 * i - 2]));
		X[1] = temp[0]; X[3] = temp[2]; X[5] = temp[4]; X[7] = temp[6];
	}
	temp[0] = (X[7] ^ (F0(X[6]) + RK[4 * i - 1]));
	temp[2] = (X[1] + (F0(X[0]) ^ RK[4 * i - 4]));
	temp[4] = (X[3] ^ (F0(X[2]) + RK[4 * i - 3]));
	temp[6] = (X[5] + (F0(X[4]) ^ RK[4 * i - 2]));
	temp[1] = X[0]; temp[3] = X[2]; temp[5] = X[4]; temp[7] = X[6];

	X[1] = (temp[1] ^ (F0(temp[0]) + RK[124]));
	X[3] = (temp[3] + (F0(temp[2]) ^ RK[125]));
	X[5] = (temp[5] ^ (F0(temp[4]) + RK[126]));
	X[7] = (temp[7] + (F0(temp[6]) ^ RK[127]));
	X[0] = temp[0]; X[2] = temp[2]; X[4] = temp[4]; X[5] = temp[6];

	CT[1] = X[1]; CT[3] = X[3]; CT[5] = X[5]; CT[7] = X[7];
	CT[0] = X[0] + WK[4];
	CT[2] = X[2] ^ WK[5];
	CT[4] = X[4] + WK[6];
	CT[6] = X[6] ^ WK[7];
}
int main() {
	unsigned char PT[8];
	unsigned char MK[16];
	unsigned char CT[8];
	unsigned char RK[1000];
	unsigned char WK[8];
	Keyschedule(MK, WK, RK);
	HIGHT_ENC(PT, WK, RK, CT);
}