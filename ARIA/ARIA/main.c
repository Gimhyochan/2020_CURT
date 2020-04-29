#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ARIA.h"
/* 키 초기화에서 사용되는 상수 전역변수로 정의*/
u8 CK1[16] = { 0x51, 0x7c, 0xc1, 0xb7, 0x27, 0x22, 0x0a, 0x94, 0xfe, 0x13, 0xab, 0xe8, 0xfa, 0x9a, 0x6e, 0xe0 };
u8 CK2[16] = { 0x6d, 0xb1, 0x4a, 0xcc, 0x9e, 0x21, 0xc8, 0x20, 0xff, 0x28, 0xb1, 0xd5, 0xef, 0x5d, 0xe2, 0xb0 };
u8 CK3[16] = { 0xdb, 0x92, 0x37, 0x1d, 0x21, 0x26, 0xe9, 0x70, 0x03, 0x24, 0x97, 0x75, 0x04, 0xe8, 0xc9, 0x0e };
/* Change 8-bits to 32-bits */
u32 u4byte_in(u8* x) {
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}
// Change 8-bits to 64-bits
/*u64 u4byte_in64(u8* x) {
	return (x[0] << 56) | (x[1] << 48) | (x[2] << 40) | (x[3] << 32) | 
		(x[4] << 24) | (x[5] << 16) | (x[6] << 8) | x[7];
} */
/* Change 32-bits to 8-bits */
void u4byte_out(u8* x, u32 y) {
	x[0] = (y >> 24) & 0xff;
	x[1] = (y >> 16) & 0xff;
	x[2] = (y >> 8) & 0xff;
	x[3] = y & 0xff;
}
// Change 64-bits to 8-bits 
/* void u4byte_out64(u8* x, u64 y) {
	x[0] = (y >> 56) & 0xff; x[1] = (y >> 48) & 0xff;
	x[2] = (y >> 40) & 0xff; x[3] = (y >> 32) & 0xff;
	x[4] = (y >> 24) & 0xff; x[5] = (y >> 16) & 0xff;
	x[6] = (y >> 8) & 0xff; x[7] = y & 0xff;
} */
void AddRoundKey(u8 S[16], u8 RK[16]) {
	S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] = RK[3];
	S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] = RK[7];
	S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] = RK[11];
	S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] = RK[15];
}
void SubstLayer(u8 S[16], int round) {
	// 라운드 수가 홀수, 짝수일때 sbox 사용이 다름
	if (round % 2 == 1) {
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
void DiffLayer(u8 S[16]) {
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

void RoundKeyGeneration128(u8 W[64], u8 RK[208]) {
	u32 temp[16];
	/* W[0] = temp[0] ~ temp[3]; W[1] = temp[4] ~ temp[7];
	   W[2] = temp[8] ~ temp[11]; W[3] = temp[12] ~ temp[15] */
	u32 result[16];
	int i;

	for (i = 0; i < 64; i += 4)
		temp[i / 4] = u4byte_in(W + i);

	// ek1
	result[4] = (temp[7] & 0x7ffff) | ((temp[4] >> 19) & 0x1fff);
	result[5] = (temp[4] & 0x7ffff) | ((temp[5] >> 19) & 0x1fff);
	result[6] = (temp[5] & 0x7ffff) | ((temp[6] >> 19) & 0x1fff);
	result[7] = (temp[6] & 0x7ffff) | ((temp[7] >> 19) & 0x1fff);
	u4byte_out(RK, temp[0] ^ result[4]);
	u4byte_out(RK + 4, temp[1] ^ result[5]);
	u4byte_out(RK + 8, temp[2] ^ result[6]);
	u4byte_out(RK + 12, temp[3] ^ result[7]);

	// ek2
	result[8] = (temp[11] & 0x7ffff) | ((temp[8] >> 19) & 0x1fff);
	result[9] = (temp[8] & 0x7ffff) | ((temp[9] >> 19) & 0x1fff);
	result[10] = (temp[9] & 0x7ffff) | ((temp[10] >> 19) & 0x1fff);
	result[11] = (temp[10] & 0x7ffff) | ((temp[11] >> 19) & 0x1fff);
	u4byte_out(RK + 16, temp[4] ^ result[8]);
	u4byte_out(RK + 20, temp[5] ^ result[9]);
	u4byte_out(RK + 24, temp[6] ^ result[10]);
	u4byte_out(RK + 28, temp[7] ^ result[11]);

	// ek3
	result[12] = (temp[15] & 0x7ffff) | ((temp[12] >> 19) & 0x1fff);
	result[13] = (temp[12] & 0x7ffff) | ((temp[13] >> 19) & 0x1fff);
	result[14] = (temp[13] & 0x7ffff) | ((temp[14] >> 19) & 0x1fff);
	result[15] = (temp[14] & 0x7ffff) | ((temp[15] >> 19) & 0x1fff);
	u4byte_out(RK + 32, temp[8] ^ result[12]);
	u4byte_out(RK + 36, temp[9] ^ result[13]);
	u4byte_out(RK + 40, temp[10] ^ result[14]);
	u4byte_out(RK + 44, temp[11] ^ result[15]);

	// ek4
	result[0] = (temp[3] & 0x7ffff) | ((temp[0] >> 19) & 0x1fff);
	result[1] = (temp[0] & 0x7ffff) | ((temp[1] >> 19) & 0x1fff);
	result[2] = (temp[1] & 0x7ffff) | ((temp[2] >> 19) & 0x1fff);
	result[3] = (temp[2] & 0x7ffff) | ((temp[3] >> 19) & 0x1fff);
	u4byte_out(RK + 48, temp[12] ^ result[0]);
	u4byte_out(RK + 52, temp[13] ^ result[1]);
	u4byte_out(RK + 56, temp[14] ^ result[2]);
	u4byte_out(RK + 60, temp[15] ^ result[3]);

	// ek5
	result[4] = (temp[7] & 0x7fffffff) | ((temp[4] >> 31) & 0x1);
	result[5] = (temp[4] & 0x7fffffff) | ((temp[5] >> 31) & 0x1);
	result[6] = (temp[5] & 0x7fffffff) | ((temp[6] >> 31) & 0x1);
	result[7] = (temp[6] & 0x7fffffff) | ((temp[7] >> 31) & 0x1);
	u4byte_out(RK + 64, temp[0] ^ result[4]);
	u4byte_out(RK + 68, temp[1] ^ result[5]);
	u4byte_out(RK + 72, temp[2] ^ result[6]);
	u4byte_out(RK + 76, temp[3] ^ result[7]);

	// ek6
	result[8] = (temp[11] & 0x7fffffff) | ((temp[8] >> 31) & 0x1);
	result[9] = (temp[8] & 0x7fffffff) | ((temp[9] >> 31) & 0x1);
	result[10] = (temp[9] & 0x7fffffff) | ((temp[10] >> 31) & 0x1);
	result[11] = (temp[10] & 0x7fffffff) | ((temp[11] >> 31) & 0x1);
	u4byte_out(RK + 80, temp[4] ^ result[8]);
	u4byte_out(RK + 84, temp[5] ^ result[9]);
	u4byte_out(RK + 88, temp[6] ^ result[10]);
	u4byte_out(RK + 92, temp[7] ^ result[11]);

	// ek7
	result[12] = (temp[15] & 0x7fffffff) | ((temp[12] >> 31) & 0x1);
	result[13] = (temp[12] & 0x7fffffff) | ((temp[13] >> 31) & 0x1);
	result[14] = (temp[13] & 0x7fffffff) | ((temp[14] >> 31) & 0x1);
	result[15] = (temp[14] & 0x7fffffff) | ((temp[15] >> 31) & 0x1);
	u4byte_out(RK + 96, temp[8] ^ result[12]);
	u4byte_out(RK + 100, temp[9] ^ result[13]);
	u4byte_out(RK + 104, temp[10] ^ result[14]);
	u4byte_out(RK + 108, temp[11] ^ result[15]);

	// ek8
	result[0] = (temp[3] & 0x7fffffff) | ((temp[0] >> 31) & 0x1);
	result[1] = (temp[0] & 0x7fffffff) | ((temp[1] >> 31) & 0x1);
	result[2] = (temp[1] & 0x7fffffff) | ((temp[2] >> 31) & 0x1);
	result[3] = (temp[2] & 0x7fffffff) | ((temp[3] >> 31) & 0x1);
	u4byte_out(RK + 112, temp[12] ^ result[0]);
	u4byte_out(RK + 116, temp[13] ^ result[1]);
	u4byte_out(RK + 120, temp[14] ^ result[2]);
	u4byte_out(RK + 124, temp[15] ^ result[3]);

	// ek9
	result[4] = (temp[5] & 0x7) | ((temp[6] >> 3) & 0x1fffffff);
	result[5] = (temp[6] & 0x7) | ((temp[7] >> 3) & 0x1fffffff);
	result[6] = (temp[7] & 0x7) | ((temp[4] >> 3) & 0x1fffffff);
	result[7] = (temp[4] & 0x7) | ((temp[5] >> 3) & 0x1fffffff);
	u4byte_out(RK + 128, temp[0] ^ result[4]);
	u4byte_out(RK + 132, temp[1] ^ result[5]);
	u4byte_out(RK + 136, temp[2] ^ result[6]);
	u4byte_out(RK + 140, temp[3] ^ result[7]);

	// ek10
	result[8] = (temp[9] & 0x7) | ((temp[10] >> 3) & 0x1fffffff);
	result[9] = (temp[10] & 0x7) | ((temp[11] >> 3) & 0x1fffffff);
	result[10] = (temp[11] & 0x7) | ((temp[8] >> 3) & 0x1fffffff);
	result[11] = (temp[8] & 0x7) | ((temp[9] >> 3) & 0x1fffffff);
	u4byte_out(RK + 144, temp[4] ^ result[8]);
	u4byte_out(RK + 148, temp[5] ^ result[9]);
	u4byte_out(RK + 152, temp[6] ^ result[10]);
	u4byte_out(RK + 156, temp[7] ^ result[11]);

	// ek11
	result[12] = (temp[13] & 0x7) | ((temp[14] >> 3) & 0x1fffffff);
	result[13] = (temp[14] & 0x7) | ((temp[15] >> 3) & 0x1fffffff);
	result[14] = (temp[15] & 0x7) | ((temp[12] >> 3) & 0x1fffffff);
	result[15] = (temp[12] & 0x7) | ((temp[13] >> 3) & 0x1fffffff);
	u4byte_out(RK + 160, temp[8] ^ result[12]);
	u4byte_out(RK + 164, temp[9] ^ result[13]);
	u4byte_out(RK + 168, temp[10] ^ result[14]);
	u4byte_out(RK + 172, temp[11] ^ result[15]);

	// ek12
	result[0] = (temp[1] & 0x7) | ((temp[2] >> 3) & 0x1fffffff);
	result[1] = (temp[2] & 0x7) | ((temp[3] >> 3) & 0x1fffffff);
	result[2] = (temp[3] & 0x7) | ((temp[0] >> 3) & 0x1fffffff);
	result[3] = (temp[0] & 0x7) | ((temp[1] >> 3) & 0x1fffffff);
	u4byte_out(RK + 176, temp[12] ^ result[0]);
	u4byte_out(RK + 180, temp[13] ^ result[1]);
	u4byte_out(RK + 184, temp[14] ^ result[2]);
	u4byte_out(RK + 188, temp[15] ^ result[3]);

	// ek13
	result[4] = (temp[4] & 0x1) | ((temp[5] >> 1) & 0x7fffffff);
	result[5] = (temp[5] & 0x1) | ((temp[6] >> 1) & 0x7fffffff);
	result[6] = (temp[6] & 0x1) | ((temp[7] >> 1) & 0x7fffffff);
	result[7] = (temp[7] & 0x1) | ((temp[4] >> 1) & 0x7fffffff);
	u4byte_out(RK + 192, temp[0] ^ result[4]);
	u4byte_out(RK + 196, temp[1] ^ result[5]);
	u4byte_out(RK + 200, temp[2] ^ result[6]);
	u4byte_out(RK + 204, temp[3] ^ result[7]);
}
void ARIA_KeySchedule_Initialization(u8 MK[], u8 KL[16], u8 KR[16], u8 W[64], u8 RK[208]) {
	u8 temp[16];
	/* W0 : W[0] ~ W[15] */
	W[0] = KL[0]; W[1] = KL[1]; W[2] = KL[2]; W[3] = KL[3];
	W[4] = KL[4]; W[5] = KL[5]; W[6] = KL[6]; W[7] = KL[7];
	W[8] = KL[8]; W[9] = KL[9]; W[10] = KL[10]; W[11] = KL[11];
	W[12] = KL[12]; W[13] = KL[13]; W[14] = KL[14]; W[15] = KL[15];

	/* W1 : W[16] ~ W[31]
	   DiffLayer(SubstLayer(W0 XOR CK1)) XOR KR*/
	temp[0] = KL[0] ^ CK1[0]; temp[1] = KL[1] ^ CK1[1]; temp[2] = KL[2] ^ CK1[2]; temp[3] = KL[3] ^ CK1[3];
	temp[4] = KL[4] ^ CK1[4]; temp[5] = KL[5] ^ CK1[5]; temp[6] = KL[6] ^ CK1[6]; temp[7] = KL[7] ^ CK1[7];
	temp[8] = KL[8] ^ CK1[8]; temp[9] = KL[9] ^ CK1[9]; temp[10] = KL[10] ^ CK1[10]; temp[11] = KL[11] ^ CK1[11];
	temp[12] = KL[12] ^ CK1[12]; temp[13] = KL[13] ^ CK1[13]; temp[14] = KL[14] ^ CK1[14]; temp[15] = KL[15] ^ CK1[15];

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

	/* Start for Key Generation */
	RoundKeyGeneration128(W, RK);
}
void ARIA_ENC(u8 PT[16], u8 CT[16], int keysize, u8 W[64], u8 RK[208]) {
	int Nr = keysize / 32 + 8;
	int i;
	u8 temp[16];

	for (i = 0; i < 16; i++)
		temp[i] = PT[i];

	AddRoundKey(temp, RK);

	for (i = 0; i < Nr - 1; i++) {
		SubstLayer(temp, i + 1);
		DiffLayer(temp);
		AddRoundKey(temp, RK + 16 * (i + 1));
	}

	SubstLayer(temp, i + 1);
	AddRoundKey(temp, RK + 16 * (i + 1));

	for (i = 0; i < 16; i++)
		CT[i] = temp[i];
}

int main() {

	int i;
	int keysize = 128;
	u8 PT[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	u8 MK[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	/* 키 확장 초기화 KL 담을 공간 */
	u8 KL[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	/* 키 확장 초기화 KR 담을 공간 */
	u8 KR[16] = { 0x00 };
	u8 CT[16] = { 0x00 };
	/* 키 확장 초기화와 라운드키 생성 중간 W0, W1, W2, W3를 담을 배열
	   W0 : [0] ~ [15], W1 : [16] ~ [31], W2 : [32] ~ [47], W3 : [48] ~ [63] */
	u8 W[64] = { 0x00 };
	/* 13 x 16 (128비트는 13라운드키 필요, 각각 8비트배열 16개 필요 */
	u8 RK[208] = { 0x00 };

	/* KL takes upper 128-bits of MK */
	/* 반복문으로 KL, KR 구분해서 넣을려고 했는데 변수 충돌 오류 생겨서 일단 위에 변수 선언에서
	   이거는 그냥 마스터키 절반 자르는거니까 절반 잘라서 일단 넣었음 */
	   /* for (i = 0; i < 16; i++)
		   KL[i] = (MK[i]);
	   for (i = 16; i < 32; i++)
		   KR[i] = (MK[i]); */

	/* Round Key Check */
	ARIA_KeySchedule_Initialization(MK, KL, KR, W, RK);
	for (i = 0; i < 208; i++) {
		printf("%02x ", RK[i]);
		if (i % 16 == 15)
			printf("\n\n");
	}
	ARIA_ENC(PT, CT, keysize, W, RK);

	for (i = 0; i < 16; i++)
		printf("%02x ", CT[i]);
	printf("\n");

	/* printf("0x%02x \n", S1box[0]);
	printf("0x%02x \n", inv_S1box[0]);
	printf("0x%02x \n", S2box[0]);
	printf("0x%02x \n", inv_S2box[0]); */

	return 0;
}