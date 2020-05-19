#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned int Constant[8] = { 0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec, 0x715ea49e, 0xc785da0a, 0xe04ef22a, 0xe5c40957, };

unsigned int ROL(int n, unsigned int x) {
	while (n >= 32) {
		n -= 32;
	}
	if (!n)return x;
	return (((x) << n) ^ ((x) >> (32 - n)));
}
unsigned int ROR(int n, unsigned int x) {
	while (n >= 32) {
		n -= 32;
	}
	if (!n)return x;
	return (((x) >> n) ^ ((x) << (32 - n)));
}
void LEA_KeyGeneration128(unsigned int MK[], unsigned int RK[]) { //128bit-key keygeneration
	unsigned int T[4];
	for (int i = 0; i < 4; i++) {
		T[i] = MK[i];
	}
	for (int i = 0; i < 24; i++) {
		T[0] = ROL(1, (T[0] + ROL(i, Constant[i & 3]) & 0xffffffff));
		T[1] = ROL(3, (T[1] + ROL(i + 1, Constant[i & 3]) & 0xffffffff));
		T[2] = ROL(6, (T[2] + ROL(i + 2, Constant[i & 3]) & 0xffffffff));
		T[3] = ROL(11, (T[3] + ROL(i + 3, Constant[i & 3]) & 0xffffffff));
		RK[6 * i] = T[0];
		RK[6 * i + 1] = T[1];
		RK[6 * i + 2] = T[2];
		RK[6 * i + 3] = T[1];
		RK[6 * i + 4] = T[3];
		RK[6 * i + 5] = T[1];
	}
}
void LEA_KeyGeneration192(unsigned int MK[], unsigned int RK[]) { //192bit-key keygeneration
	unsigned int T[6];
	for (int i = 0; i < 6; i++) {
		T[i] = MK[i];
	}
	for (int i = 0; i < 28; i++) {
		T[0] = ROL(1, T[0] + ROL(i, (Constant[i % 6])));
		T[1] = ROL(3, T[1] + ROL(i + 1, (Constant[i % 6])));
		T[2] = ROL(6, T[2] + ROL(i + 2, (Constant[i % 6])));
		T[3] = ROL(11, T[3] + ROL(i + 3, (Constant[i % 6])));
		T[4] = ROL(13, T[4] + ROL(i + 4, (Constant[i % 6])));
		T[5] = ROL(17, T[5] + ROL(i + 5, (Constant[i % 6])));
		RK[6 * i] = T[0];
		RK[6 * i + 1] = T[1];
		RK[6 * i + 2] = T[2];
		RK[6 * i + 3] = T[3];
		RK[6 * i + 4] = T[4];
		RK[6 * i + 5] = T[5];
	}

}
void LEA_KeyGeneration256(unsigned int MK[], unsigned int RK[]) { //256bit-key keygeneration
	unsigned int T[8];
	for (int i = 0; i < 8; i++) {
		T[i] = MK[i];
	}
	for (int i = 0; i < 32; i++) {
		T[(6 * i) % 8] = ROL(1, T[(6 * i) % 8] + ROL(i, (Constant[i % 8])));
		T[(6 * i + 1) % 8] = ROL(3, T[(6 * i + 1) % 8] + ROL(i + 1, (Constant[i % 8])));
		T[(6 * i + 2) % 8] = ROL(6, T[(6 * i + 2) % 8] + ROL(i + 2, (Constant[i % 8])));
		T[(6 * i + 3) % 8] = ROL(11, T[(6 * i + 3) % 8] + ROL(i + 3, (Constant[i % 8])));
		T[(6 * i + 4) % 8] = ROL(13, T[(6 * i + 4) % 8] + ROL(i + 4, (Constant[i % 8])));
		T[(6 * i + 5) % 8] = ROL(17, T[(6 * i + 5) % 8] + ROL(i + 5, (Constant[i % 8])));

		RK[6 * i] = T[(6*i) % 8];
		RK[6 * i + 1] = T[(6 * i + 1) % 8];
		RK[6 * i + 2] = T[(6 * i + 2) % 8];
		RK[6 * i + 3] = T[(6 * i + 3) % 8];
		RK[6 * i + 4] = T[(6 * i + 4) % 8];
		RK[6 * i + 5] = T[(6 * i + 5) % 8];
	}

}
void LEA_KeyGeneration(unsigned int MK[], unsigned int RK[], int * Nr) { //keygeneration
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

void RoundEnc(unsigned int PT[], unsigned int* RK) { //Encryption for each round
	unsigned int temp;
	temp = PT[0];
	PT[0] = ROL(9, ((PT[0] ^ RK[0]) + (PT[1] ^ RK[1])));
	PT[1] = ROR(5, ((PT[1] ^ RK[2]) + (PT[2] ^ RK[3])));
	PT[2] = ROR(3, ((PT[2] ^ RK[4]) + (PT[3] ^ RK[5])));
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
	//128bit Encryption
	unsigned int PT128[4] = { 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c };
	unsigned int MK128[8] = { 0x3c2d1e0f, 0x78695a4b,0xb4a59687, 0xf0e1d2c3, 0x00000000, 0x00000000, 0x00000000, 0x00000000, };
	unsigned int RK128[192] = { 0x00000000, };
	unsigned int CT128[4] = { 0x00000000, };
	int Nr=0;
	LEA_KeyGeneration(MK128,RK128,&Nr);
	LEA_ENC(PT128, RK128, Nr, CT128);
	printf("0x%x 0x%x 0x%x 0x%x\n", CT128[0], CT128[1], CT128[2], CT128[3]);
	//192bit Encryption
	unsigned int PT192[4] = { 0x23222120, 0x27262524, 0x2b2a2928, 0x2f2e2d2c };
	unsigned int MK192[8] = { 0x3c2d1e0f, 0x78695a4b,0xb4a59687, 0xf0e1d2c3, 0xc3d2e1f0, 0x8796a5b4, 0x00000000, 0x00000000, };
	unsigned int RK192[192] = { 0x00000000, };
	unsigned int CT192[4] = { 0x00000000, };
	LEA_KeyGeneration(MK192, RK192, &Nr);
	LEA_ENC(PT192, RK192, Nr, CT192);
	printf("0x%x 0x%x 0x%x 0x%x\n", CT192[0], CT192[1], CT192[2], CT192[3]);
	//256bit Encryption
	unsigned int PT256[4] = { 0x33323130, 0x37363534, 0x3b3a3938, 0x3f3e3d3c };
	unsigned int MK256[8] = { 0x3c2d1e0f, 0x78695a4b,0xb4a59687, 0xf0e1d2c3, 0xc3d2e1f0, 0x8796a5b4, 0x4b5a6978, 0x0f1e2d3c, };
	unsigned int RK256[192] = { 0x00000000, };
	unsigned int CT256[4] = { 0x00000000, };
	LEA_KeyGeneration(MK256, RK256, &Nr);
	LEA_ENC(PT256, RK256, Nr, CT256);
	printf("0x%x 0x%x 0x%x 0x%x\n", CT256[0], CT256[1], CT256[2], CT256[3]);

	return 0;
}