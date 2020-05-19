/* ARIA는 128bit key는 12, 192bit는 14, 256bit는 16라운드 동작 */
/* 키를 생성하는 과정은 1. 키확장 초기화 과정과 2. 라운드키 생성 두 부분으로 이루어짐 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ARIA.h"

/* 128bit key의 경우의 상수, 192와 256은 키 생성 과정에서 바꾸는 코드 사용 */
u8 CK1[16] = { 0x51, 0x7c, 0xc1, 0xb7, 0x27, 0x22, 0x0a, 0x94, 0xfe, 0x13, 0xab, 0xe8, 0xfa, 0x9a, 0x6e, 0xe0 };
u8 CK2[16] = { 0x6d, 0xb1, 0x4a, 0xcc, 0x9e, 0x21, 0xc8, 0x20, 0xff, 0x28, 0xb1, 0xd5, 0xef, 0x5d, 0xe2, 0xb0 };
u8 CK3[16] = { 0xdb, 0x92, 0x37, 0x1d, 0x21, 0x26, 0xe9, 0x70, 0x03, 0x24, 0x97, 0x75, 0x04, 0xe8, 0xc9, 0x0e };

u32 u4byte_in(u8* x) {
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}
void u4byte_out(u8* x, u32 y) {
	x[0] = (y >> 24) & 0xff;
	x[1] = (y >> 16) & 0xff;
	x[2] = (y >> 8) & 0xff;
	x[3] = y & 0xff;
}

void AddRoundKey(u8 S[], u8 RK[]) {
	S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3];
	S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7];
	S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11];
	S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];
}
/* 홀수 라운드와 짝수 라운드의 S-box 구조가 달라 짝수, 홀수의 경우를 나눠서 작성 */
void SubstLayer(u8 S[], int round) {
	if (round % 2) {
		S[0] = S1box[S[0]]; S[1] = S2box[S[1]]; S[2] = inv_S1box[S[2]]; S[3] = inv_S2box[S[3]];
		S[4] = S1box[S[4]]; S[5] = S2box[S[5]]; S[6] = inv_S1box[S[6]]; S[7] = inv_S2box[S[7]];
		S[8] = S1box[S[8]]; S[9] = S2box[S[9]]; S[10] = inv_S1box[S[10]]; S[11] = inv_S2box[S[11]];
		S[12] = S1box[S[12]]; S[13] = S2box[S[13]]; S[14] = inv_S1box[S[14]]; S[15] = inv_S2box[S[15]];
	}
	else {
		S[0] = inv_S1box[S[0]]; S[1] = inv_S2box[S[1]]; S[2] = S1box[S[2]]; S[3] = S2box[S[3]];
		S[4] = inv_S1box[S[4]]; S[5] = inv_S2box[S[5]]; S[6] = S1box[S[6]]; S[7] = S2box[S[7]];
		S[8] = inv_S1box[S[8]]; S[9] = inv_S2box[S[9]]; S[10] = S1box[S[10]]; S[11] = S2box[S[11]];
		S[12] = inv_S1box[S[12]]; S[13] = inv_S2box[S[13]]; S[14] = S1box[S[14]]; S[15] = S2box[S[15]];
	}
}
/* 입력 128bit를 확산계층에 사용되는 16 x 16 행렬대로 XOR 연산 수행 */
void DiffLayer(u8 S[]) {
	u8 temp[16];
	int i;

	temp[0] = S[3] ^ S[4] ^ S[6] ^ S[8] ^ S[9] ^ S[13] ^ S[14];
	temp[1] = S[2] ^ S[5] ^ S[7] ^ S[8] ^ S[9] ^ S[12] ^ S[15];
	temp[2] = S[1] ^ S[4] ^ S[6] ^ S[10] ^ S[11] ^ S[12] ^ S[15];
	temp[3] = S[0] ^ S[5] ^ S[7] ^ S[10] ^ S[11] ^ S[13] ^ S[14];

	temp[4] = S[0] ^ S[2] ^ S[5] ^ S[8] ^ S[11] ^ S[14] ^ S[15];
	temp[5] = S[1] ^ S[3] ^ S[4] ^ S[9] ^ S[10] ^ S[14] ^ S[15];
	temp[6] = S[0] ^ S[2] ^ S[7] ^ S[9] ^ S[10] ^ S[12] ^ S[13];
	temp[7] = S[1] ^ S[3] ^ S[6] ^ S[8] ^ S[11] ^ S[12] ^ S[13];

	temp[8] = S[0] ^ S[1] ^ S[4] ^ S[7] ^ S[10] ^ S[13] ^ S[15];
	temp[9] = S[0] ^ S[1] ^ S[5] ^ S[6] ^ S[11] ^ S[12] ^ S[14];
	temp[10] = S[2] ^ S[3] ^ S[5] ^ S[6] ^ S[8] ^ S[13] ^ S[15];
	temp[11] = S[2] ^ S[3] ^ S[4] ^ S[7] ^ S[9] ^ S[12] ^ S[14];

	temp[12] = S[1] ^ S[2] ^ S[6] ^ S[7] ^ S[9] ^ S[11] ^ S[12];
	temp[13] = S[0] ^ S[3] ^ S[6] ^ S[7] ^ S[8] ^ S[10] ^ S[13];
	temp[14] = S[0] ^ S[3] ^ S[4] ^ S[5] ^ S[9] ^ S[11] ^ S[14];
	temp[15] = S[1] ^ S[2] ^ S[4] ^ S[5] ^ S[8] ^ S[10] ^ S[15];

	for (i = 0; i < 16; i += 4) {
		S[i] = temp[i];
		S[i + 1] = temp[i + 1];
		S[i + 2] = temp[i + 2];
		S[i + 3] = temp[i + 3];
	}
}

/* 2. 라운드 키 생성 */
/* 생성된 W0, W1, W2, W3를 32bit 4개씩 나눠서 넣음 */
void RoundKeyGeneration(u8 W[], u8 RK[], int keysize) {
	u32 temp[16];
	/* W[0] = temp[0] ~ temp[3]; W[1] = temp[4] ~ temp[7];
	   W[2] = temp[8] ~ temp[11]; W[3] = temp[12] ~ temp[15] */
	u32 result[4];
	/* 라운드 키 생성에서 비트를 옮긴 작업을 저장할 공간 생서 */
	int i;

	/* W : W0 ~ W3를 8비트로 표현된걸 32비트로 묶어서 사용하기 위해 작성 */
	for (i = 0; i < 64; i += 4)
		temp[i / 4] = u4byte_in(W + i);

	// ek1
	/* ek1 = (W0) ^ (W1 >> 19) */
	/* 이때 W0 = temp[0] ~ temp[3], W1 = temp[4] ~ temp[7] */
	/* 나머지 키도 문서 참고해서 보면 됨 */
	/* 라운드키 생성 수식 그대로임 */
	result[0] = (temp[7] << 13) ^ (temp[4] >> 19) ^ temp[0];
	result[1] = (temp[4] << 13) ^ (temp[5] >> 19) ^ temp[1];
	result[2] = (temp[5] << 13) ^ (temp[6] >> 19) ^ temp[2];
	result[3] = (temp[6] << 13) ^ (temp[7] >> 19) ^ temp[3];
	/* 생성된 라운드키를 다시 8비트 배열에 저장하기 위해 사용 */
	u4byte_out(RK, result[0]);
	u4byte_out(RK + 4, result[1]);
	u4byte_out(RK + 8, result[2]);
	u4byte_out(RK + 12, result[3]);

	// ek2
	result[0] = (temp[11] << 13) ^ (temp[8] >> 19) ^ temp[4];
	result[1] = (temp[8] << 13) ^ (temp[9] >> 19) ^ temp[5];
	result[2] = (temp[9] << 13) ^ (temp[10] >> 19) ^ temp[6];
	result[3] = (temp[10] << 13) ^ (temp[11] >> 19) ^ temp[7];
	u4byte_out(RK + 16, result[0]);
	u4byte_out(RK + 20, result[1]);
	u4byte_out(RK + 24, result[2]);
	u4byte_out(RK + 28, result[3]);

	// ek3
	result[0] = (temp[15] << 13) ^ (temp[12] >> 19) ^ temp[8];
	result[1] = (temp[12] << 13) ^ (temp[13] >> 19) ^ temp[9];
	result[2] = (temp[13] << 13) ^ (temp[14] >> 19) ^ temp[10];
	result[3] = (temp[14] << 13) ^ (temp[15] >> 19) ^ temp[11];
	u4byte_out(RK + 32, result[0]);
	u4byte_out(RK + 36, result[1]);
	u4byte_out(RK + 40, result[2]);
	u4byte_out(RK + 44, result[3]);

	// ek4
	result[0] = (temp[3] << 13) ^ (temp[0] >> 19) ^ temp[12];
	result[1] = (temp[0] << 13) ^ (temp[1] >> 19) ^ temp[13];
	result[2] = (temp[1] << 13) ^ (temp[2] >> 19) ^ temp[14];
	result[3] = (temp[2] << 13) ^ (temp[3] >> 19) ^ temp[15];
	u4byte_out(RK + 48, result[0]);
	u4byte_out(RK + 52, result[1]);
	u4byte_out(RK + 56, result[2]);
	u4byte_out(RK + 60, result[3]);

	// ek5
	result[0] = (temp[7] << 1) ^ (temp[4] >> 31) ^ temp[0];
	result[1] = (temp[4] << 1) ^ (temp[5] >> 31) ^ temp[1];
	result[2] = (temp[5] << 1) ^ (temp[6] >> 31) ^ temp[2];
	result[3] = (temp[6] << 1) ^ (temp[7] >> 31) ^ temp[3];
	u4byte_out(RK + 64, result[0]);
	u4byte_out(RK + 68, result[1]);
	u4byte_out(RK + 72, result[2]);
	u4byte_out(RK + 76, result[3]);

	// ek6
	result[0] = (temp[11] << 1) ^ (temp[8] >> 31) ^ temp[4];
	result[1] = (temp[8] << 1) ^ (temp[9] >> 31) ^ temp[5];
	result[2] = (temp[9] << 1) ^ (temp[10] >> 31) ^ temp[6];
	result[3] = (temp[10] << 1) ^ (temp[11] >> 31) ^ temp[7];
	u4byte_out(RK + 80, result[0]);
	u4byte_out(RK + 84, result[1]);
	u4byte_out(RK + 88, result[2]);
	u4byte_out(RK + 92, result[3]);

	// ek7
	result[0] = (temp[15] << 1) ^ (temp[12] >> 31) ^ temp[8];
	result[1] = (temp[12] << 1) ^ (temp[13] >> 31) ^ temp[9];
	result[2] = (temp[13] << 1) ^ (temp[14] >> 31) ^ temp[10];
	result[3] = (temp[14] << 1) ^ (temp[15] >> 31) ^ temp[11];
	u4byte_out(RK + 96, result[0]);
	u4byte_out(RK + 100, result[1]);
	u4byte_out(RK + 104, result[2]);
	u4byte_out(RK + 108, result[3]);

	// ek8
	result[0] = (temp[3] << 1) ^ (temp[0] >> 31) ^ temp[12];
	result[1] = (temp[0] << 1) ^ (temp[1] >> 31) ^ temp[13];
	result[2] = (temp[1] << 1) ^ (temp[2] >> 31) ^ temp[14];
	result[3] = (temp[2] << 1) ^ (temp[3] >> 31) ^ temp[15];
	u4byte_out(RK + 112, result[0]);
	u4byte_out(RK + 116, result[1]);
	u4byte_out(RK + 120, result[2]);
	u4byte_out(RK + 124, result[3]);

	// ek9
	result[0] = (temp[5] << 29) ^ (temp[6] >> 3) ^ temp[0];
	result[1] = (temp[6] << 29) ^ (temp[7] >> 3) ^ temp[1];
	result[2] = (temp[7] << 29) ^ (temp[4] >> 3) ^ temp[2];
	result[3] = (temp[4] << 29) ^ (temp[5] >> 3) ^ temp[3];
	u4byte_out(RK + 128, result[0]);
	u4byte_out(RK + 132, result[1]);
	u4byte_out(RK + 136, result[2]);
	u4byte_out(RK + 140, result[3]);

	// ek10
	result[0] = (temp[9] << 29) ^ (temp[10] >> 3) ^ temp[4];
	result[1] = (temp[10] << 29) ^ (temp[11] >> 3) ^ temp[5];
	result[2] = (temp[11] << 29) ^ (temp[8] >> 3) ^ temp[6];
	result[3] = (temp[8] << 29) ^ (temp[9] >> 3) ^ temp[7];
	u4byte_out(RK + 144, result[0]);
	u4byte_out(RK + 148, result[1]);
	u4byte_out(RK + 152, result[2]);
	u4byte_out(RK + 156, result[3]);

	// ek11
	result[0] = (temp[13] << 29) ^ (temp[14] >> 3) ^ temp[8];
	result[1] = (temp[14] << 29) ^ (temp[15] >> 3) ^ temp[9];
	result[2] = (temp[15] << 29) ^ (temp[12] >> 3) ^ temp[10];
	result[3] = (temp[12] << 29) ^ (temp[13] >> 3) ^ temp[11];
	u4byte_out(RK + 160, result[0]);
	u4byte_out(RK + 164, result[1]);
	u4byte_out(RK + 168, result[2]);
	u4byte_out(RK + 172, result[3]);

	// ek12
	result[0] = (temp[1] << 29) ^ (temp[2] >> 3) ^ temp[12];
	result[1] = (temp[2] << 29) ^ (temp[3] >> 3) ^ temp[13];
	result[2] = (temp[3] << 29) ^ (temp[0] >> 3) ^ temp[14];
	result[3] = (temp[0] << 29) ^ (temp[1] >> 3) ^ temp[15];
	u4byte_out(RK + 176, result[0]);
	u4byte_out(RK + 180, result[1]);
	u4byte_out(RK + 184, result[2]);
	u4byte_out(RK + 188, result[3]);

	// ek13
	result[0] = (temp[4] << 31) ^ (temp[5] >> 1) ^ temp[0];
	result[1] = (temp[5] << 31) ^ (temp[6] >> 1) ^ temp[1];
	result[2] = (temp[6] << 31) ^ (temp[7] >> 1) ^ temp[2];
	result[3] = (temp[7] << 31) ^ (temp[4] >> 1) ^ temp[3];
	u4byte_out(RK + 192, result[0]);
	u4byte_out(RK + 196, result[1]);
	u4byte_out(RK + 200, result[2]);
	u4byte_out(RK + 204, result[3]);

	if (keysize == 192) {
		// ek14
		result[0] = (temp[8] << 31) ^ (temp[9] >> 1) ^ temp[4];
		result[1] = (temp[9] << 31) ^ (temp[10] >> 1) ^ temp[5];
		result[2] = (temp[10] << 31) ^ (temp[11] >> 1) ^ temp[6];
		result[3] = (temp[11] << 31) ^ (temp[8] >> 1) ^ temp[7];
		u4byte_out(RK + 208, result[0]);
		u4byte_out(RK + 212, result[1]);
		u4byte_out(RK + 216, result[2]);
		u4byte_out(RK + 220, result[3]);

		// ek15
		result[0] = (temp[12] << 31) ^ (temp[13] >> 1) ^ temp[8];
		result[1] = (temp[13] << 31) ^ (temp[14] >> 1) ^ temp[9];
		result[2] = (temp[14] << 31) ^ (temp[15] >> 1) ^ temp[10];
		result[3] = (temp[15] << 31) ^ (temp[12] >> 1) ^ temp[11];
		u4byte_out(RK + 224, result[0]);
		u4byte_out(RK + 228, result[1]);
		u4byte_out(RK + 232, result[2]);
		u4byte_out(RK + 236, result[3]);
	}
	else if (keysize == 256) {
		// ek14
		result[0] = (temp[8] << 31) ^ (temp[9] >> 1) ^ temp[4];
		result[1] = (temp[9] << 31) ^ (temp[10] >> 1) ^ temp[5];
		result[2] = (temp[10] << 31) ^ (temp[11] >> 1) ^ temp[6];
		result[3] = (temp[11] << 31) ^ (temp[8] >> 1) ^ temp[7];
		u4byte_out(RK + 208, result[0]);
		u4byte_out(RK + 212, result[1]);
		u4byte_out(RK + 216, result[2]);
		u4byte_out(RK + 220, result[3]);

		// ek15
		result[0] = (temp[12] << 31) ^ (temp[13] >> 1) ^ temp[8];
		result[1] = (temp[13] << 31) ^ (temp[14] >> 1) ^ temp[9];
		result[2] = (temp[14] << 31) ^ (temp[15] >> 1) ^ temp[10];
		result[3] = (temp[15] << 31) ^ (temp[12] >> 1) ^ temp[11];
		u4byte_out(RK + 224, result[0]);
		u4byte_out(RK + 228, result[1]);
		u4byte_out(RK + 232, result[2]);
		u4byte_out(RK + 236, result[3]);

		// ek16
		result[0] = (temp[0] << 31) ^ (temp[1] >> 1) ^ temp[12];
		result[1] = (temp[1] << 31) ^ (temp[2] >> 1) ^ temp[13];
		result[2] = (temp[2] << 31) ^ (temp[3] >> 1) ^ temp[14];
		result[3] = (temp[3] << 31) ^ (temp[0] >> 1) ^ temp[15];
		u4byte_out(RK + 240, result[0]);
		u4byte_out(RK + 244, result[1]);
		u4byte_out(RK + 248, result[2]);
		u4byte_out(RK + 252, result[3]);

		// ek17
		result[0] = (temp[4] << 19) ^ (temp[5] >> 13) ^ temp[0];
		result[1] = (temp[5] << 19) ^ (temp[6] >> 13) ^ temp[1];
		result[2] = (temp[6] << 19) ^ (temp[7] >> 13) ^ temp[2];
		result[3] = (temp[7] << 19) ^ (temp[4] >> 13) ^ temp[3];
		u4byte_out(RK + 256, result[0]);
		u4byte_out(RK + 260, result[1]);
		u4byte_out(RK + 264, result[2]);
		u4byte_out(RK + 268, result[3]);
	}
}
/* 1. 키확장 초기화 과정 */
void ARIA_KeySchedule_Initialization(u8 MK[], u8 KL[16], u8 KR[], u8 W[], u8 RK[], int keysize) {
	u8 temp[16];
	int i;
	/* keysize에 따라 사용되는 상수가 다른 것을 해결 */
	if (keysize == 192) {
		for (i = 0; i < 16; i++)
			temp[i] = CK1[i];
		for (i = 0; i < 16; i++) {
			CK1[i] = CK2[i];
			CK2[i] = CK3[i];
			CK3[i] = temp[i];
		}
	}
	else if (keysize == 256) {
		for (i = 0; i < 16; i++)
			temp[i] = CK3[i];
		for (i = 0; i < 16; i++) {
			CK3[i] = CK2[i];
			CK2[i] = CK1[i];
			CK1[i] = temp[i];
		}
	}

	/* 키 확장 초기화 과정 그대로 따라가는 것임. */
	/* W0 : W[0] ~ W[15] */
	W[0] = KL[0]; W[1] = KL[1]; W[2] = KL[2]; W[3] = KL[3];
	W[4] = KL[4]; W[5] = KL[5]; W[6] = KL[6]; W[7] = KL[7];
	W[8] = KL[8]; W[9] = KL[9]; W[10] = KL[10]; W[11] = KL[11];
	W[12] = KL[12]; W[13] = KL[13]; W[14] = KL[14]; W[15] = KL[15];

	/* W1 : W[16] ~ W[31] */
	temp[0] = KL[0] ^ CK1[0]; temp[1] = KL[1] ^ CK1[1]; temp[2] = KL[2] ^ CK1[2]; temp[3] = KL[3] ^ CK1[3];
	temp[4] = KL[4] ^ CK1[4]; temp[5] = KL[5] ^ CK1[5]; temp[6] = KL[6] ^ CK1[6]; temp[7] = KL[7] ^ CK1[7];
	temp[8] = KL[8] ^ CK1[8]; temp[9] = KL[9] ^ CK1[9]; temp[10] = KL[10] ^ CK1[10]; temp[11] = KL[11] ^ CK1[11];
	temp[12] = KL[12] ^ CK1[12]; temp[13] = KL[13] ^ CK1[13]; temp[14] = KL[14] ^ CK1[14]; temp[15] = KL[15] ^ CK1[15];

	/* 키 확장 초기화 과정에서 홀수 라운드 함수와 짝수 라운드 함수가 사용됨 */
	SubstLayer(temp, 1);
	DiffLayer(temp);

	W[16] = temp[0] ^ KR[0]; W[17] = temp[1] ^ KR[1]; W[18] = temp[2] ^ KR[2]; W[19] = temp[3] ^ KR[3];
	W[20] = temp[4] ^ KR[4]; W[21] = temp[5] ^ KR[5]; W[22] = temp[6] ^ KR[6]; W[23] = temp[7] ^ KR[7];
	W[24] = temp[8] ^ KR[8]; W[25] = temp[9] ^ KR[9]; W[26] = temp[10] ^ KR[10]; W[27] = temp[11] ^ KR[11];
	W[28] = temp[12] ^ KR[12]; W[29] = temp[13] ^ KR[13]; W[30] = temp[14] ^ KR[14]; W[31] = temp[15] ^ KR[15];

	/* W2 : W[32] ~ W[47]
	   DiffLayer(SubstLayer(W1 XOR CK2)) XOR W0*/
	temp[0] = W[16] ^ CK2[0]; temp[1] = W[17] ^ CK2[1]; temp[2] = W[18] ^ CK2[2]; temp[3] = W[19] ^ CK2[3];
	temp[4] = W[20] ^ CK2[4]; temp[5] = W[21] ^ CK2[5]; temp[6] = W[22] ^ CK2[6]; temp[7] = W[23] ^ CK2[7];
	temp[8] = W[24] ^ CK2[8]; temp[9] = W[25] ^ CK2[9]; temp[10] = W[26] ^ CK2[10]; temp[11] = W[27] ^ CK2[11];
	temp[12] = W[28] ^ CK2[12]; temp[13] = W[29] ^ CK2[13]; temp[14] = W[30] ^ CK2[14]; temp[15] = W[31] ^ CK2[15];

	SubstLayer(temp, 2);
	DiffLayer(temp);

	W[32] = temp[0] ^ W[0]; W[33] = temp[1] ^ W[1]; W[34] = temp[2] ^ W[2]; W[35] = temp[3] ^ W[3];
	W[36] = temp[4] ^ W[4]; W[37] = temp[5] ^ W[5]; W[38] = temp[6] ^ W[6]; W[39] = temp[7] ^ W[7];
	W[40] = temp[8] ^ W[8]; W[41] = temp[9] ^ W[9]; W[42] = temp[10] ^ W[10]; W[43] = temp[11] ^ W[11];
	W[44] = temp[12] ^ W[12]; W[45] = temp[13] ^ W[13]; W[46] = temp[14] ^ W[14]; W[47] = temp[15] ^ W[15];

	/* W3 : W[48] ~ W[63]
	   DiffLayer(SubstLayer(W2 XOR CK3)) XOR W1 */
	temp[0] = W[32] ^ CK3[0]; temp[1] = W[33] ^ CK3[1]; temp[2] = W[34] ^ CK3[2]; temp[3] = W[35] ^ CK3[3];
	temp[4] = W[36] ^ CK3[4]; temp[5] = W[37] ^ CK3[5]; temp[6] = W[38] ^ CK3[6]; temp[7] = W[39] ^ CK3[7];
	temp[8] = W[40] ^ CK3[8]; temp[9] = W[41] ^ CK3[9]; temp[10] = W[42] ^ CK3[10]; temp[11] = W[43] ^ CK3[11];
	temp[12] = W[44] ^ CK3[12]; temp[13] = W[45] ^ CK3[13]; temp[14] = W[46] ^ CK3[14]; temp[15] = W[47] ^ CK3[15];

	SubstLayer(temp, 1);
	DiffLayer(temp);

	W[48] = temp[0] ^ W[16]; W[49] = temp[1] ^ W[17]; W[50] = temp[2] ^ W[18]; W[51] = temp[3] ^ W[19];
	W[52] = temp[4] ^ W[20]; W[53] = temp[5] ^ W[21]; W[54] = temp[6] ^ W[22]; W[55] = temp[7] ^ W[23];
	W[56] = temp[8] ^ W[24]; W[57] = temp[9] ^ W[25]; W[58] = temp[10] ^ W[26]; W[59] = temp[11] ^ W[27];
	W[60] = temp[12] ^ W[28]; W[61] = temp[13] ^ W[29]; W[62] = temp[14] ^ W[30]; W[63] = temp[15] ^ W[31];

	/* 생성된 W를 가지고 RK에 저장하기 위해 2. 라운드키 생성을 시작 */
	RoundKeyGeneration(W, RK, keysize);
}

void ARIA_ENC(u8 PT[], u8 CT[], int keysize, u8 RK[]) {
	int Nr = keysize / 32 + 8;
	int i;
	u8 temp[16];

	for (i = 0; i < 16; i++)
		temp[i] = PT[i];

	/* 짝수, 홀수 라운드 구분을 위해 SubstLayer는 i를 추가로 인자로 넘김 */
	for (i = 1; i < Nr; i++) {
		AddRoundKey(temp, RK + 16 * (i - 1));
		SubstLayer(temp, i);
		DiffLayer(temp);
	}
	/* 마지막 라운드에는 DiffLayer가 없음 */
	AddRoundKey(temp, RK + 16 * (i - 1));
	SubstLayer(temp, i);
	AddRoundKey(temp, RK + 16 * i);

	for (i = 0; i < 16; i++)
		CT[i] = temp[i];
}

int main() {

	int i;
	int keysize = 256;
	u8 PT[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	u8 MK[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
	/* KL은 MK의 상위 16바이트, KR은 MK의 하위 16바이트 */
	u8 KL[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	u8 KR[16] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
	u8 CT[16] = { 0x00 };
	u8 W[64] = { 0x00 };
	/* enough space for 256-bits key generation */
	/* 128bit : 13 x 16 = 208 */
	/* 192bit : 15 x 16 = 240 */
	/* 256bit : 17 x 16 = 272 */
	u8 RK[272] = { 0x00 };

	ARIA_KeySchedule_Initialization(MK, KL, KR, W, RK, keysize);
	ARIA_ENC(PT, CT, keysize, RK);

	for (i = 0; i < 16; i++)
		printf("%02x ", CT[i]);
	printf("\n");

	return 0;
}