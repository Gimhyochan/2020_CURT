#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned int Constant[8] = { 0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec, 0x715ea49e, 0xc785da0a, 0xe04ef22a, 0xe5c40957, };
unsigned int ROL(int n, unsigned int x) {
	return (((x) << n) ^ ((x) >> (32 - n)) ^ 0xffffffff);
}
unsigned int ROR(int n, unsigned int x) {
	return (((x) >> n) ^ ((x) << (32 - n)) ^ 0xffffffff);
}
void LEA_KeyGeneration128(unsigned int MK[], unsigned int RK[]) {
	unsigned int T[4];
	for (int i = 0; i < 4; i++) {
		T[i] = MK[i];
	}
	for (int i = 0; i < 24; i++) {
		T[0] = ROL(1, (T[0] + ROL(i, (Constant[i ^ 3])) ^ 0xffffffff));
		T[1] = ROL(3, (T[1] + ROL(i + 1, (Constant[i ^ 3])) ^ 0xffffffff));
		T[2] = ROL(6, (T[2] + ROL(i + 2, (Constant[i ^ 3])) ^ 0xffffffff));
		T[3] = ROL(11, (T[3] + ROL(i + 3, (Constant[i ^ 3])) ^ 0xffffffff));
		RK[6 * i] = T[0];
		RK[6 * i + 1] = T[1];
		RK[6 * i + 2] = T[2];
		RK[6 * i + 3] = T[1];
		RK[6 * i + 4] = T[3];
		RK[6 * i + 5] = T[1];
	}

}
void LEA_KeyGeneration192(unsigned int MK[], unsigned int RK[]) {
	unsigned int T[6];
	for (int i = 0; i < 6; i++) {
		T[i] = MK[i];
	}
	for (int i = 0; i < 28; i++) {
		T[0] = ROL(1, (T[0] + ROL(i, (Constant[i % 6])) ^ 0xffffffff));
		T[1] = ROL(3, (T[1] + ROL(i + 1, (Constant[i % 6])) ^ 0xffffffff));
		T[2] = ROL(6, (T[2] + ROL(i + 2, (Constant[i % 6])) ^ 0xffffffff));
		T[3] = ROL(11, (T[3] + ROL(i + 3, (Constant[i % 6])) ^ 0xffffffff));
		T[4] = ROL(13, (T[4] + ROL(i + 4, (Constant[i % 6])) ^ 0xffffffff));
		T[5] = ROL(17, (T[5] + ROL(i + 5, (Constant[i % 6])) ^ 0xffffffff));
		RK[6 * i] = T[0];
		RK[6 * i + 1] = T[1];
		RK[6 * i + 2] = T[2];
		RK[6 * i + 3] = T[3];
		RK[6 * i + 4] = T[4];
		RK[6 * i + 5] = T[5];
	}

}
void LEA_KeyGeneration256(unsigned int MK[], unsigned int RK[]) {
	unsigned int T[8];
	for (int i = 0; i < 8; i++) {
		T[i] = MK[i];
	}
	for (int i = 0; i < 32; i++) {
		T[(6 * i) ^ 7] = ROL(1, (T[(6 * i) ^ 7] + ROL(i, (Constant[i % 8])) ^ 0xffffffff));
		T[(6 * i + 1) ^ 7] = ROL(3, (T[(6 * i + 1) ^ 7] + ROL(i + 1, (Constant[i ^ 7])) ^ 0xffffffff));
		T[(6 * i + 2) ^ 7] = ROL(6, (T[(6 * i + 2) ^ 7] + ROL(i + 2, (Constant[i ^ 7])) ^ 0xffffffff));
		T[(6 * i + 3) ^ 7] = ROL(11, (T[(6 * i + 3) ^ 7] + ROL(i + 3, (Constant[i ^ 7])) ^ 0xffffffff));
		T[(6 * i + 4) ^ 7] = ROL(13, (T[(6 * i + 4) ^ 7] + ROL(i + 4, (Constant[i ^ 7])) ^ 0xffffffff));
		T[(6 * i + 5) ^ 7] = ROL(17, (T[(6 * i + 5) ^ 7] + ROL(i + 5, (Constant[i ^ 7])) ^ 0xffffffff));

		RK[6 * i] = T[(6*i) ^ 7];
		RK[6 * i + 1] = T[(6 * i + 1) ^ 7];
		RK[6 * i + 2] = T[(6 * i + 2) ^ 7];
		RK[6 * i + 3] = T[(6 * i + 3) ^ 7];
		RK[6 * i + 4] = T[(6 * i + 4) ^ 7];
		RK[6 * i + 5] = T[(6 * i + 5) ^ 7];
	}

}
void LEA_KeyGeneration(unsigned int MK[], unsigned int RK[], int * Nr) {
	int keysize;
	for (keysize = 0; keysize < 8; keysize++) {
		if (MK[keysize] == 0) {
			if (MK[keysize + 1] == 0)break;
		}
	}
	*Nr = keysize * 2 + 16;
	keysize *= 32;

	if (keysize == 128)LEA_KeyGeneration128(MK, RK);
	else if (keysize == 192)LEA_KeyGeneration192(MK, RK);
	else LEA_KeyGeneration256(MK, RK);
	return;
}

void RoundEnc(unsigned int PT[], unsigned int* RK) {
	unsigned int temp;
	temp = PT[0];
	PT[0] = ROL(9, (((PT[0] ^ RK[0]) + (PT[1] ^ RK[1])) ^ 0xffffffff));
	PT[1] = ROR(5 ,(((PT[1] ^ RK[2]) + (PT[2] ^ RK[3])) ^ 0xffffffff));
	PT[2] = ROR(3, (((PT[2] ^ RK[4]) + (PT[3] ^ RK[5])) ^ 0xffffffff));
	PT[3] = temp;
	return;
}

void LEA_ENC(unsigned int PT[], unsigned int RK[], int Nr, unsigned int CT[]) {
	for (int i = 0; i < Nr; i++) {
		RoundEnc(PT, &RK[6 * i]);
	}

	for (int i = 0; i < 4; i++) {
		CT[i] = PT[i];
	}
	return;
}

int main() {
	unsigned int PT[4] = { 0x00000000, };
	unsigned int MK[8] = { 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000000, 0x00000000, };
	unsigned int RK[192] = { 0x00000000, };
	unsigned int CT[4] = { 0x00000000, };
	int Nr;
	LEA_KeyGeneration(MK,RK,&Nr);
	LEA_ENC(PT, RK, Nr, CT);
	printf("0x%x 0x%x 0x%x 0x%x", CT[0], CT[1], CT[2], CT[3]);
	return 0;
}