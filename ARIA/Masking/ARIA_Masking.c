/*
<Reference>
Title : A High-speed Masking Method to protect ARIA against Side Channel Analysis
Written By : HeeSeok Kim, Tae Hyun Kim, Jeong-Choon Ryoo, Dong-Guk Han, SeokHie Hong
Graduate School of Information Management and Security, Korea University,
Electronics and Telecommunications Research Institute
*/
/*
<논문 관련 추가 정보>
논문에 나와있는 [그림 3], [그림 4]의 '중간 마스킹 값'은 해당 값으로 마스킹을 적용하는 것이 아니라
해당 마스킹이 적용된 입력값이 들어온다는 의미임. 즉, '추가 연산'에 해당하는 부분만 매 라운드에 추가해주면 됨.
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "ARIA.h"

u8 CK1[16] = { 0x51, 0x7c, 0xc1, 0xb7, 0x27, 0x22, 0x0a, 0x94, 0xfe, 0x13, 0xab, 0xe8, 0xfa, 0x9a, 0x6e, 0xe0 };
u8 CK2[16] = { 0x6d, 0xb1, 0x4a, 0xcc, 0x9e, 0x21, 0xc8, 0x20, 0xff, 0x28, 0xb1, 0xd5, 0xef, 0x5d, 0xe2, 0xb0 };
u8 CK3[16] = { 0xdb, 0x92, 0x37, 0x1d, 0x21, 0x26, 0xe9, 0x70, 0x03, 0x24, 0x97, 0x75, 0x04, 0xe8, 0xc9, 0x0e };

/* Masking S-box를 생성하고 담을 변수 */
u8 maskS1box[256] = { 0x00 };
u8 maskS2box[256] = { 0x00 };
u8 maskinvS1box[256] = { 0x00 };
u8 maskinvS2box[256] = { 0x00 };

/*
랜덤으로 생성된 마스킹 값을 담을 배열
m XOR m' = m''
*/
u8 mask[3] = { 0x00 };

u32 u4byte_in(u8* x) {
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}
void u4byte_out(u8* x, u32 y) {
	x[0] = (y >> 24) & 0xff;
	x[1] = (y >> 16) & 0xff;
	x[2] = (y >> 8) & 0xff;
	x[3] = y & 0xff;
}

void Masking_AddRoundKey(u8 S[], u8 RK[]) {
	S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3];
	S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7];
	S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11];
	S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];
}
/* 
각 라운드에서 추가 연산에 해당하는 부분을 연산하기 위한 함수
(0000m''m''m''m'')2 형태로 XOR 연산이 추가됨.
*/
void Masking_AddRoundKey2(u8 S[]) {
	S[4] ^= mask[2]; S[5] ^= mask[2]; S[6] ^= mask[2]; S[7] ^= mask[2];
	S[12] ^= mask[2]; S[13] ^= mask[2]; S[14] ^= mask[2]; S[15] ^= mask[2];
}
void Masking_SubstLayer(u8 S[], int round) {
	if (round % 2) {
		S[0] = maskS1box[S[0]]; S[1] = maskS2box[S[1]]; S[2] = maskinvS1box[S[2]]; S[3] = maskinvS2box[S[3]];
		S[4] = maskS1box[S[4]]; S[5] = maskS2box[S[5]]; S[6] = maskinvS1box[S[6]]; S[7] = maskinvS2box[S[7]];
		S[8] = maskS1box[S[8]]; S[9] = maskS2box[S[9]]; S[10] = maskinvS1box[S[10]]; S[11] = maskinvS2box[S[11]];
		S[12] = maskS1box[S[12]]; S[13] = maskS2box[S[13]]; S[14] = maskinvS1box[S[14]]; S[15] = maskinvS2box[S[15]];
	}
	else {
		S[0] = maskinvS1box[S[0]]; S[1] = maskinvS2box[S[1]]; S[2] = maskS1box[S[2]]; S[3] = maskS2box[S[3]];
		S[4] = maskinvS1box[S[4]]; S[5] = maskinvS2box[S[5]]; S[6] = maskS1box[S[6]]; S[7] = maskS2box[S[7]];
		S[8] = maskinvS1box[S[8]]; S[9] = maskinvS2box[S[9]]; S[10] = maskS1box[S[10]]; S[11] = maskS2box[S[11]];
		S[12] = maskinvS1box[S[12]]; S[13] = maskinvS2box[S[13]]; S[14] = maskS1box[S[14]]; S[15] = maskS2box[S[15]];
	}
}
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

void Masking_RoundKeyGeneration(u8 W[], u8 RK[], int keysize) {
	u32 temp[16];
	/* W0 = temp[0] ~ temp[3]; W1 = temp[4] ~ temp[7];
	   W2 = temp[8] ~ temp[11]; W3 = temp[12] ~ temp[15] */
	u32 result[4];
	u8 masktemp;
	int i;

	for (i = 0; i < 64; i += 4)
		temp[i / 4] = u4byte_in(W + i);

	// ek1
	// (W0) XOR (W1 >> 19) XOR (m'' >> 3)16
	result[0] = (temp[7] << 13) ^ (temp[4] >> 19) ^ temp[0];
	result[1] = (temp[4] << 13) ^ (temp[5] >> 19) ^ temp[1];
	result[2] = (temp[5] << 13) ^ (temp[6] >> 19) ^ temp[2];
	result[3] = (temp[6] << 13) ^ (temp[7] >> 19) ^ temp[3];
	u4byte_out(RK, result[0]);
	u4byte_out(RK + 4, result[1]);
	u4byte_out(RK + 8, result[2]);
	u4byte_out(RK + 12, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[0] ^= masktemp; RK[1] ^= masktemp; RK[2] ^= masktemp; RK[3] ^= masktemp;
	RK[4] ^= masktemp; RK[5] ^= masktemp; RK[6] ^= masktemp; RK[7] ^= masktemp;
	RK[8] ^= masktemp; RK[9] ^= masktemp; RK[10] ^= masktemp; RK[11] ^= masktemp;
	RK[12] ^= masktemp; RK[13] ^= masktemp; RK[14] ^= masktemp; RK[15] ^= masktemp;

	// ek2
	// (W1) XOR (W2 >> 19) XOR (m'' >> 3)16
	result[0] = (temp[11] << 13) ^ (temp[8] >> 19) ^ temp[4];
	result[1] = (temp[8] << 13) ^ (temp[9] >> 19) ^ temp[5];
	result[2] = (temp[9] << 13) ^ (temp[10] >> 19) ^ temp[6];
	result[3] = (temp[10] << 13) ^ (temp[11] >> 19) ^ temp[7];
	u4byte_out(RK + 16, result[0]);
	u4byte_out(RK + 20, result[1]);
	u4byte_out(RK + 24, result[2]);
	u4byte_out(RK + 28, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[16] ^= masktemp; RK[17] ^= masktemp; RK[18] ^= masktemp; RK[19] ^= masktemp;
	RK[20] ^= masktemp; RK[21] ^= masktemp; RK[22] ^= masktemp; RK[23] ^= masktemp;
	RK[24] ^= masktemp; RK[25] ^= masktemp; RK[26] ^= masktemp; RK[27] ^= masktemp;
	RK[28] ^= masktemp; RK[29] ^= masktemp; RK[30] ^= masktemp; RK[31] ^= masktemp;

	// ek3
	// (W2) XOR (W3 >> 19) XOR (m'' >> 3)16
	result[0] = (temp[15] << 13) ^ (temp[12] >> 19) ^ temp[8];
	result[1] = (temp[12] << 13) ^ (temp[13] >> 19) ^ temp[9];
	result[2] = (temp[13] << 13) ^ (temp[14] >> 19) ^ temp[10];
	result[3] = (temp[14] << 13) ^ (temp[15] >> 19) ^ temp[11];
	u4byte_out(RK + 32, result[0]);
	u4byte_out(RK + 36, result[1]);
	u4byte_out(RK + 40, result[2]);
	u4byte_out(RK + 44, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[32] ^= masktemp; RK[33] ^= masktemp; RK[34] ^= masktemp; RK[35] ^= masktemp;
	RK[36] ^= masktemp; RK[37] ^= masktemp; RK[38] ^= masktemp; RK[39] ^= masktemp;
	RK[40] ^= masktemp; RK[41] ^= masktemp; RK[42] ^= masktemp; RK[43] ^= masktemp;
	RK[44] ^= masktemp; RK[45] ^= masktemp; RK[46] ^= masktemp; RK[47] ^= masktemp;

	// ek4
	// (W3) XOR (W0 >> 19) XOR (m'' >> 3)16
	result[0] = (temp[3] << 13) ^ (temp[0] >> 19) ^ temp[12];
	result[1] = (temp[0] << 13) ^ (temp[1] >> 19) ^ temp[13];
	result[2] = (temp[1] << 13) ^ (temp[2] >> 19) ^ temp[14];
	result[3] = (temp[2] << 13) ^ (temp[3] >> 19) ^ temp[15];
	u4byte_out(RK + 48, result[0]);
	u4byte_out(RK + 52, result[1]);
	u4byte_out(RK + 56, result[2]);
	u4byte_out(RK + 60, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[48] ^= masktemp; RK[49] ^= masktemp; RK[50] ^= masktemp; RK[51] ^= masktemp;
	RK[52] ^= masktemp; RK[53] ^= masktemp; RK[54] ^= masktemp; RK[55] ^= masktemp;
	RK[56] ^= masktemp; RK[57] ^= masktemp; RK[58] ^= masktemp; RK[59] ^= masktemp;
	RK[60] ^= masktemp; RK[61] ^= masktemp; RK[62] ^= masktemp; RK[63] ^= masktemp;

	// ek5
	// (W0) XOR (W1 >> 31) XOR (m'' >> 7)16
	result[0] = (temp[7] << 1) ^ (temp[4] >> 31) ^ temp[0];
	result[1] = (temp[4] << 1) ^ (temp[5] >> 31) ^ temp[1];
	result[2] = (temp[5] << 1) ^ (temp[6] >> 31) ^ temp[2];
	result[3] = (temp[6] << 1) ^ (temp[7] >> 31) ^ temp[3];
	u4byte_out(RK + 64, result[0]);
	u4byte_out(RK + 68, result[1]);
	u4byte_out(RK + 72, result[2]);
	u4byte_out(RK + 76, result[3]);

	masktemp = (mask[2] << 1) ^ (mask[2] >> 7);
	RK[64] ^= masktemp; RK[65] ^= masktemp; RK[66] ^= masktemp; RK[67] ^= masktemp;
	RK[68] ^= masktemp; RK[69] ^= masktemp; RK[70] ^= masktemp; RK[71] ^= masktemp;
	RK[72] ^= masktemp; RK[73] ^= masktemp; RK[74] ^= masktemp; RK[75] ^= masktemp;
	RK[76] ^= masktemp; RK[77] ^= masktemp; RK[78] ^= masktemp; RK[79] ^= masktemp;

	// ek6
	// (W1) XOR (W2 >> 31) XOR (m'' >> 7)16
	result[0] = (temp[11] << 1) ^ (temp[8] >> 31) ^ temp[4];
	result[1] = (temp[8] << 1) ^ (temp[9] >> 31) ^ temp[5];
	result[2] = (temp[9] << 1) ^ (temp[10] >> 31) ^ temp[6];
	result[3] = (temp[10] << 1) ^ (temp[11] >> 31) ^ temp[7];
	u4byte_out(RK + 80, result[0]);
	u4byte_out(RK + 84, result[1]);
	u4byte_out(RK + 88, result[2]);
	u4byte_out(RK + 92, result[3]);

	masktemp = (mask[2] << 1) ^ (mask[2] >> 7);
	RK[80] ^= masktemp; RK[81] ^= masktemp; RK[82] ^= masktemp; RK[83] ^= masktemp;
	RK[84] ^= masktemp; RK[85] ^= masktemp; RK[86] ^= masktemp; RK[87] ^= masktemp;
	RK[88] ^= masktemp; RK[89] ^= masktemp; RK[90] ^= masktemp; RK[91] ^= masktemp;
	RK[92] ^= masktemp; RK[93] ^= masktemp; RK[94] ^= masktemp; RK[95] ^= masktemp;

	// ek7
	// (W2) XOR (W3 >> 31) XOR (m'' >> 7)16
	result[0] = (temp[15] << 1) ^ (temp[12] >> 31) ^ temp[8];
	result[1] = (temp[12] << 1) ^ (temp[13] >> 31) ^ temp[9];
	result[2] = (temp[13] << 1) ^ (temp[14] >> 31) ^ temp[10];
	result[3] = (temp[14] << 1) ^ (temp[15] >> 31) ^ temp[11];
	u4byte_out(RK + 96, result[0]);
	u4byte_out(RK + 100, result[1]);
	u4byte_out(RK + 104, result[2]);
	u4byte_out(RK + 108, result[3]);

	masktemp = (mask[2] << 1) ^ (mask[2] >> 7);
	RK[96] ^= masktemp; RK[97] ^= masktemp; RK[98] ^= masktemp; RK[99] ^= masktemp;
	RK[100] ^= masktemp; RK[101] ^= masktemp; RK[102] ^= masktemp; RK[103] ^= masktemp;
	RK[104] ^= masktemp; RK[105] ^= masktemp; RK[106] ^= masktemp; RK[107] ^= masktemp;
	RK[108] ^= masktemp; RK[109] ^= masktemp; RK[110] ^= masktemp; RK[111] ^= masktemp;

	// ek8
	// (W3) XOR (W0 >> 31) XOR (m'' >> 7)16
	result[0] = (temp[3] << 1) ^ (temp[0] >> 31) ^ temp[12];
	result[1] = (temp[0] << 1) ^ (temp[1] >> 31) ^ temp[13];
	result[2] = (temp[1] << 1) ^ (temp[2] >> 31) ^ temp[14];
	result[3] = (temp[2] << 1) ^ (temp[3] >> 31) ^ temp[15];
	u4byte_out(RK + 112, result[0]);
	u4byte_out(RK + 116, result[1]);
	u4byte_out(RK + 120, result[2]);
	u4byte_out(RK + 124, result[3]);

	masktemp = (mask[2] << 1) ^ (mask[2] >> 7);
	RK[112] ^= masktemp; RK[113] ^= masktemp; RK[114] ^= masktemp; RK[115] ^= masktemp;
	RK[116] ^= masktemp; RK[117] ^= masktemp; RK[118] ^= masktemp; RK[119] ^= masktemp;
	RK[120] ^= masktemp; RK[121] ^= masktemp; RK[122] ^= masktemp; RK[123] ^= masktemp;
	RK[124] ^= masktemp; RK[125] ^= masktemp; RK[126] ^= masktemp; RK[127] ^= masktemp;

	// ek9
	// (W0) XOR (W1 << 61) XOR (m'' << 5)16
	result[0] = (temp[5] << 29) ^ (temp[6] >> 3) ^ temp[0];
	result[1] = (temp[6] << 29) ^ (temp[7] >> 3) ^ temp[1];
	result[2] = (temp[7] << 29) ^ (temp[4] >> 3) ^ temp[2];
	result[3] = (temp[4] << 29) ^ (temp[5] >> 3) ^ temp[3];
	u4byte_out(RK + 128, result[0]);
	u4byte_out(RK + 132, result[1]);
	u4byte_out(RK + 136, result[2]);
	u4byte_out(RK + 140, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[128] ^= masktemp; RK[129] ^= masktemp; RK[130] ^= masktemp; RK[131] ^= masktemp;
	RK[132] ^= masktemp; RK[133] ^= masktemp; RK[134] ^= masktemp; RK[135] ^= masktemp;
	RK[136] ^= masktemp; RK[137] ^= masktemp; RK[138] ^= masktemp; RK[139] ^= masktemp;
	RK[140] ^= masktemp; RK[141] ^= masktemp; RK[142] ^= masktemp; RK[143] ^= masktemp;

	// ek10
	// (W1) XOR (W2 << 61) XOR (m'' << 5)16
	result[0] = (temp[9] << 29) ^ (temp[10] >> 3) ^ temp[4];
	result[1] = (temp[10] << 29) ^ (temp[11] >> 3) ^ temp[5];
	result[2] = (temp[11] << 29) ^ (temp[8] >> 3) ^ temp[6];
	result[3] = (temp[8] << 29) ^ (temp[9] >> 3) ^ temp[7];
	u4byte_out(RK + 144, result[0]);
	u4byte_out(RK + 148, result[1]);
	u4byte_out(RK + 152, result[2]);
	u4byte_out(RK + 156, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[144] ^= masktemp; RK[145] ^= masktemp; RK[146] ^= masktemp; RK[147] ^= masktemp;
	RK[148] ^= masktemp; RK[149] ^= masktemp; RK[150] ^= masktemp; RK[151] ^= masktemp;
	RK[152] ^= masktemp; RK[153] ^= masktemp; RK[154] ^= masktemp; RK[155] ^= masktemp;
	RK[156] ^= masktemp; RK[157] ^= masktemp; RK[158] ^= masktemp; RK[159] ^= masktemp;

	// ek11
	// (W2) XOR (W3 << 61) XOR (m'' << 5)16
	result[0] = (temp[13] << 29) ^ (temp[14] >> 3) ^ temp[8];
	result[1] = (temp[14] << 29) ^ (temp[15] >> 3) ^ temp[9];
	result[2] = (temp[15] << 29) ^ (temp[12] >> 3) ^ temp[10];
	result[3] = (temp[12] << 29) ^ (temp[13] >> 3) ^ temp[11];
	u4byte_out(RK + 160, result[0]);
	u4byte_out(RK + 164, result[1]);
	u4byte_out(RK + 168, result[2]);
	u4byte_out(RK + 172, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[160] ^= masktemp; RK[161] ^= masktemp; RK[162] ^= masktemp; RK[163] ^= masktemp;
	RK[164] ^= masktemp; RK[165] ^= masktemp; RK[166] ^= masktemp; RK[167] ^= masktemp;
	RK[168] ^= masktemp; RK[169] ^= masktemp; RK[170] ^= masktemp; RK[171] ^= masktemp;
	RK[172] ^= masktemp; RK[173] ^= masktemp; RK[174] ^= masktemp; RK[175] ^= masktemp;

	// ek12
	// (W3) XOR (W0 << 61) XOR (m'' << 5)16
	result[0] = (temp[1] << 29) ^ (temp[2] >> 3) ^ temp[12];
	result[1] = (temp[2] << 29) ^ (temp[3] >> 3) ^ temp[13];
	result[2] = (temp[3] << 29) ^ (temp[0] >> 3) ^ temp[14];
	result[3] = (temp[0] << 29) ^ (temp[1] >> 3) ^ temp[15];
	u4byte_out(RK + 176, result[0]);
	u4byte_out(RK + 180, result[1]);
	u4byte_out(RK + 184, result[2]);
	u4byte_out(RK + 188, result[3]);

	masktemp = (mask[2] << 5) ^ (mask[2] >> 3);
	RK[176] ^= masktemp; RK[177] ^= masktemp; RK[178] ^= masktemp; RK[179] ^= masktemp;
	RK[180] ^= masktemp; RK[181] ^= masktemp; RK[182] ^= masktemp; RK[183] ^= masktemp;
	RK[184] ^= masktemp; RK[185] ^= masktemp; RK[186] ^= masktemp; RK[187] ^= masktemp;
	RK[188] ^= masktemp; RK[189] ^= masktemp; RK[190] ^= masktemp; RK[191] ^= masktemp;

	// ek13
	// (W0) XOR (W1 << 31) XOR (m'' << 7)16
	result[0] = (temp[4] << 31) ^ (temp[5] >> 1) ^ temp[0];
	result[1] = (temp[5] << 31) ^ (temp[6] >> 1) ^ temp[1];
	result[2] = (temp[6] << 31) ^ (temp[7] >> 1) ^ temp[2];
	result[3] = (temp[7] << 31) ^ (temp[4] >> 1) ^ temp[3];
	u4byte_out(RK + 192, result[0]);
	u4byte_out(RK + 196, result[1]);
	u4byte_out(RK + 200, result[2]);
	u4byte_out(RK + 204, result[3]);

	masktemp = (mask[2] << 7) ^ (mask[2] >> 1);
	RK[192] ^= masktemp; RK[193] ^= masktemp; RK[194] ^= masktemp; RK[195] ^= masktemp;
	RK[196] ^= masktemp; RK[197] ^= masktemp; RK[198] ^= masktemp; RK[199] ^= masktemp;
	RK[200] ^= masktemp; RK[201] ^= masktemp; RK[202] ^= masktemp; RK[203] ^= masktemp;
	RK[204] ^= masktemp; RK[205] ^= masktemp; RK[206] ^= masktemp; RK[207] ^= masktemp;

	if (keysize == 192) {
		// ek14
		// (W1) XOR (W2 << 31) XOR (m'' << 7)16
		result[0] = (temp[8] << 31) ^ (temp[9] >> 1) ^ temp[4];
		result[1] = (temp[9] << 31) ^ (temp[10] >> 1) ^ temp[5];
		result[2] = (temp[10] << 31) ^ (temp[11] >> 1) ^ temp[6];
		result[3] = (temp[11] << 31) ^ (temp[8] >> 1) ^ temp[7];
		u4byte_out(RK + 208, result[0]);
		u4byte_out(RK + 212, result[1]);
		u4byte_out(RK + 216, result[2]);
		u4byte_out(RK + 220, result[3]);

		masktemp = (mask[2] << 7) ^ (mask[2] >> 1);
		RK[208] ^= masktemp; RK[209] ^= masktemp; RK[210] ^= masktemp; RK[211] ^= masktemp;
		RK[212] ^= masktemp; RK[213] ^= masktemp; RK[214] ^= masktemp; RK[215] ^= masktemp;
		RK[216] ^= masktemp; RK[217] ^= masktemp; RK[218] ^= masktemp; RK[219] ^= masktemp;
		RK[220] ^= masktemp; RK[221] ^= masktemp; RK[222] ^= masktemp; RK[223] ^= masktemp;

		// ek15
		// (W2) XOR (W3 << 31) XOR (m'' << 7)16
		result[0] = (temp[12] << 31) ^ (temp[13] >> 1) ^ temp[8];
		result[1] = (temp[13] << 31) ^ (temp[14] >> 1) ^ temp[9];
		result[2] = (temp[14] << 31) ^ (temp[15] >> 1) ^ temp[10];
		result[3] = (temp[15] << 31) ^ (temp[12] >> 1) ^ temp[11];
		u4byte_out(RK + 224, result[0]);
		u4byte_out(RK + 228, result[1]);
		u4byte_out(RK + 232, result[2]);
		u4byte_out(RK + 236, result[3]);

		masktemp = (mask[2] << 7) ^ (mask[2] >> 1);
		RK[224] ^= masktemp; RK[225] ^= masktemp; RK[226] ^= masktemp; RK[227] ^= masktemp;
		RK[228] ^= masktemp; RK[229] ^= masktemp; RK[230] ^= masktemp; RK[231] ^= masktemp;
		RK[232] ^= masktemp; RK[233] ^= masktemp; RK[234] ^= masktemp; RK[235] ^= masktemp;
		RK[236] ^= masktemp; RK[237] ^= masktemp; RK[238] ^= masktemp; RK[239] ^= masktemp;
	}
	else if (keysize == 256) {
		// ek14
		// (W1) XOR (W2 << 31) XOR (m'' << 7)16
		result[0] = (temp[8] << 31) ^ (temp[9] >> 1) ^ temp[4];
		result[1] = (temp[9] << 31) ^ (temp[10] >> 1) ^ temp[5];
		result[2] = (temp[10] << 31) ^ (temp[11] >> 1) ^ temp[6];
		result[3] = (temp[11] << 31) ^ (temp[8] >> 1) ^ temp[7];
		u4byte_out(RK + 208, result[0]);
		u4byte_out(RK + 212, result[1]);
		u4byte_out(RK + 216, result[2]);
		u4byte_out(RK + 220, result[3]);

		masktemp = (mask[2] << 7) ^ (mask[2] >> 1);
		RK[208] ^= masktemp; RK[209] ^= masktemp; RK[210] ^= masktemp; RK[211] ^= masktemp;
		RK[212] ^= masktemp; RK[213] ^= masktemp; RK[214] ^= masktemp; RK[215] ^= masktemp;
		RK[216] ^= masktemp; RK[217] ^= masktemp; RK[218] ^= masktemp; RK[219] ^= masktemp;
		RK[220] ^= masktemp; RK[221] ^= masktemp; RK[222] ^= masktemp; RK[223] ^= masktemp;

		// ek15
		// (W2) XOR (W3 << 31) XOR (m'' << 7)16
		result[0] = (temp[12] << 31) ^ (temp[13] >> 1) ^ temp[8];
		result[1] = (temp[13] << 31) ^ (temp[14] >> 1) ^ temp[9];
		result[2] = (temp[14] << 31) ^ (temp[15] >> 1) ^ temp[10];
		result[3] = (temp[15] << 31) ^ (temp[12] >> 1) ^ temp[11];
		u4byte_out(RK + 224, result[0]);
		u4byte_out(RK + 228, result[1]);
		u4byte_out(RK + 232, result[2]);
		u4byte_out(RK + 236, result[3]);

		masktemp = (mask[2] << 7) ^ (mask[2] >> 1);
		RK[224] ^= masktemp; RK[225] ^= masktemp; RK[226] ^= masktemp; RK[227] ^= masktemp;
		RK[228] ^= masktemp; RK[229] ^= masktemp; RK[230] ^= masktemp; RK[231] ^= masktemp;
		RK[232] ^= masktemp; RK[233] ^= masktemp; RK[234] ^= masktemp; RK[235] ^= masktemp;
		RK[236] ^= masktemp; RK[237] ^= masktemp; RK[238] ^= masktemp; RK[239] ^= masktemp;

		// ek16
		// (W3) XOR (W0 << 31) XOR (m'' << 7)16
		result[0] = (temp[0] << 31) ^ (temp[1] >> 1) ^ temp[12];
		result[1] = (temp[1] << 31) ^ (temp[2] >> 1) ^ temp[13];
		result[2] = (temp[2] << 31) ^ (temp[3] >> 1) ^ temp[14];
		result[3] = (temp[3] << 31) ^ (temp[0] >> 1) ^ temp[15];
		u4byte_out(RK + 240, result[0]);
		u4byte_out(RK + 244, result[1]);
		u4byte_out(RK + 248, result[2]);
		u4byte_out(RK + 252, result[3]);

		masktemp = (mask[2] << 7) ^ (mask[2] >> 1);
		RK[240] ^= masktemp; RK[241] ^= masktemp; RK[242] ^= masktemp; RK[243] ^= masktemp;
		RK[244] ^= masktemp; RK[245] ^= masktemp; RK[246] ^= masktemp; RK[247] ^= masktemp;
		RK[248] ^= masktemp; RK[249] ^= masktemp; RK[250] ^= masktemp; RK[251] ^= masktemp;
		RK[252] ^= masktemp; RK[253] ^= masktemp; RK[254] ^= masktemp; RK[255] ^= masktemp;

		// ek17
		// (W0) XOR (W1 << 19) XOR (m'' << 3)16
		result[0] = (temp[4] << 19) ^ (temp[5] >> 13) ^ temp[0];
		result[1] = (temp[5] << 19) ^ (temp[6] >> 13) ^ temp[1];
		result[2] = (temp[6] << 19) ^ (temp[7] >> 13) ^ temp[2];
		result[3] = (temp[7] << 19) ^ (temp[4] >> 13) ^ temp[3];
		u4byte_out(RK + 256, result[0]);
		u4byte_out(RK + 260, result[1]);
		u4byte_out(RK + 264, result[2]);
		u4byte_out(RK + 268, result[3]);

		masktemp = (mask[2] << 3) ^ (mask[2] >> 5);
		RK[256] ^= masktemp; RK[257] ^= masktemp; RK[258] ^= masktemp; RK[259] ^= masktemp;
		RK[260] ^= masktemp; RK[261] ^= masktemp; RK[262] ^= masktemp; RK[263] ^= masktemp;
		RK[264] ^= masktemp; RK[265] ^= masktemp; RK[266] ^= masktemp; RK[267] ^= masktemp;
		RK[268] ^= masktemp; RK[269] ^= masktemp; RK[270] ^= masktemp; RK[271] ^= masktemp;
	}
}
/* 모든 W 값들과 라운드 키들은 (m'')16으로 마스킹 된 형태여야 함. */
void Masking_ARIA_KeySchedule_Initialization(u8 MK[], u8 KL[16], u8 KR[16], u8 W[], u8 RK[], int keysize) {
	u8 temp[16];
	int i;

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

	/* W0 : (W[0] ~ W[15]) ^ (m'')16 */
	W[0] = KL[0] ^ mask[2]; W[1] = KL[1] ^ mask[2]; W[2] = KL[2] ^ mask[2]; W[3] = KL[3] ^ mask[2];
	W[4] = KL[4] ^ mask[2]; W[5] = KL[5] ^ mask[2]; W[6] = KL[6] ^ mask[2]; W[7] = KL[7] ^ mask[2];
	W[8] = KL[8] ^ mask[2]; W[9] = KL[9] ^ mask[2]; W[10] = KL[10] ^ mask[2]; W[11] = KL[11] ^ mask[2];
	W[12] = KL[12] ^ mask[2]; W[13] = KL[13] ^ mask[2]; W[14] = KL[14] ^ mask[2]; W[15] = KL[15] ^ mask[2];

	/* KR ^ (m'm'mmmmm'm')2*/
	KR[0] ^= mask[1]; KR[1] ^= mask[1]; KR[2] ^= mask[0]; KR[3] ^= mask[0];
	KR[4] ^= mask[0]; KR[5] ^= mask[0]; KR[6] ^= mask[1]; KR[7] ^= mask[1];
	KR[8] ^= mask[1]; KR[9] ^= mask[1]; KR[10] ^= mask[0]; KR[11] ^= mask[0];
	KR[12] ^= mask[0]; KR[13] ^= mask[0]; KR[14] ^= mask[1]; KR[15] ^= mask[1];

	/* W1 : W[16] ~ W[31] */
	temp[0] = W[0] ^ mask[1] ^ CK1[0]; temp[1] = W[1] ^ mask[1] ^ CK1[1]; temp[2] = W[2] ^ mask[0] ^ CK1[2]; temp[3] = W[3] ^ mask[0] ^ CK1[3];
	temp[4] = W[4] ^ mask[1] ^ CK1[4]; temp[5] = W[5] ^ mask[1] ^ CK1[5]; temp[6] = W[6] ^ mask[0] ^ CK1[6]; temp[7] = W[7] ^ mask[0] ^ CK1[7];
	temp[8] = W[8] ^ mask[1] ^ CK1[8]; temp[9] = W[9] ^ mask[1] ^ CK1[9]; temp[10] = W[10] ^ mask[0] ^ CK1[10]; temp[11] = W[11] ^ mask[0] ^ CK1[11];
	temp[12] = W[12] ^ mask[1] ^ CK1[12]; temp[13] = W[13] ^ mask[1] ^ CK1[13]; temp[14] = W[14] ^ mask[0] ^ CK1[14]; temp[15] = W[15] ^ mask[0] ^ CK1[15];

	/* 
	논문의 그림을 보면 각 라운드에는 추가연산으로 (0000m''m''m''m'')2가 적용되지만
	라운드 키를 생성하는 과정에서 연산되는 라운드 함수 내에서는 추가 연산을 적용하지 않는다.
	*/
	Masking_SubstLayer(temp, 1);
	DiffLayer(temp);

	W[16] = temp[0] ^ KR[0]; W[17] = temp[1] ^ KR[1]; W[18] = temp[2] ^ KR[2]; W[19] = temp[3] ^ KR[3];
	W[20] = temp[4] ^ KR[4]; W[21] = temp[5] ^ KR[5]; W[22] = temp[6] ^ KR[6]; W[23] = temp[7] ^ KR[7];
	W[24] = temp[8] ^ KR[8]; W[25] = temp[9] ^ KR[9]; W[26] = temp[10] ^ KR[10]; W[27] = temp[11] ^ KR[11];
	W[28] = temp[12] ^ KR[12]; W[29] = temp[13] ^ KR[13]; W[30] = temp[14] ^ KR[14]; W[31] = temp[15] ^ KR[15];

	/* W2 : W[32] ~ W[47]
	   DiffLayer(SubstLayer(W1 XOR CK2)) XOR W0*/
	temp[0] = W[16] ^ mask[0] ^ CK2[0]; temp[1] = W[17] ^ mask[0] ^ CK2[1]; temp[2] = W[18] ^ mask[1] ^ CK2[2]; temp[3] = W[19] ^ mask[1] ^ CK2[3];
	temp[4] = W[20] ^ mask[0] ^ CK2[4]; temp[5] = W[21] ^ mask[0] ^ CK2[5]; temp[6] = W[22] ^ mask[1] ^ CK2[6]; temp[7] = W[23] ^ mask[1] ^ CK2[7];
	temp[8] = W[24] ^ mask[0] ^ CK2[8]; temp[9] = W[25] ^ mask[0] ^ CK2[9]; temp[10] = W[26] ^ mask[1] ^ CK2[10]; temp[11] = W[27] ^ mask[1] ^ CK2[11];
	temp[12] = W[28] ^ mask[0] ^ CK2[12]; temp[13] = W[29] ^ mask[0] ^ CK2[13]; temp[14] = W[30] ^ mask[1] ^ CK2[14]; temp[15] = W[31] ^ mask[1] ^ CK2[15];

	Masking_SubstLayer(temp, 2);
	DiffLayer(temp);

	W[32] = temp[0] ^ W[0] ^ mask[1]; W[33] = temp[1] ^ W[1] ^ mask[1]; W[34] = temp[2] ^ W[2] ^ mask[0]; W[35] = temp[3] ^ W[3] ^ mask[0];
	W[36] = temp[4] ^ W[4] ^ mask[0]; W[37] = temp[5] ^ W[5] ^ mask[0]; W[38] = temp[6] ^ W[6] ^ mask[1]; W[39] = temp[7] ^ W[7] ^ mask[1];
	W[40] = temp[8] ^ W[8] ^ mask[1]; W[41] = temp[9] ^ W[9] ^ mask[1]; W[42] = temp[10] ^ W[10] ^ mask[0]; W[43] = temp[11] ^ W[11] ^ mask[0];
	W[44] = temp[12] ^ W[12] ^ mask[0]; W[45] = temp[13] ^ W[13] ^ mask[0]; W[46] = temp[14] ^ W[14] ^ mask[1]; W[47] = temp[15] ^ W[15] ^ mask[1];

	/* W3 : W[48] ~ W[63]
	   DiffLayer(SubstLayer(W2 XOR CK3)) XOR W1 */
	temp[0] = W[32] ^ mask[1] ^ CK3[0]; temp[1] = W[33] ^ mask[1] ^ CK3[1]; temp[2] = W[34] ^ mask[0] ^ CK3[2]; temp[3] = W[35] ^ mask[0] ^ CK3[3];
	temp[4] = W[36] ^ mask[1] ^ CK3[4]; temp[5] = W[37] ^ mask[1] ^ CK3[5]; temp[6] = W[38] ^ mask[0] ^ CK3[6]; temp[7] = W[39] ^ mask[0] ^ CK3[7];
	temp[8] = W[40] ^ mask[1] ^ CK3[8]; temp[9] = W[41] ^ mask[1] ^ CK3[9]; temp[10] = W[42] ^ mask[0] ^ CK3[10]; temp[11] = W[43] ^ mask[0] ^ CK3[11];
	temp[12] = W[44] ^ mask[1] ^ CK3[12]; temp[13] = W[45] ^ mask[1] ^ CK3[13]; temp[14] = W[46] ^ mask[0] ^ CK3[14]; temp[15] = W[47] ^ mask[0] ^ CK3[15];

	Masking_SubstLayer(temp, 1);
	DiffLayer(temp);

	W[48] = temp[0] ^ W[16] ^ mask[0]; W[49] = temp[1] ^ W[17] ^ mask[0]; W[50] = temp[2] ^ W[18] ^ mask[1]; W[51] = temp[3] ^ W[19] ^ mask[1];
	W[52] = temp[4] ^ W[20] ^ mask[1]; W[53] = temp[5] ^ W[21] ^ mask[1]; W[54] = temp[6] ^ W[22] ^ mask[0]; W[55] = temp[7] ^ W[23] ^ mask[0];
	W[56] = temp[8] ^ W[24] ^ mask[0]; W[57] = temp[9] ^ W[25] ^ mask[0]; W[58] = temp[10] ^ W[26] ^ mask[1]; W[59] = temp[11] ^ W[27] ^ mask[1];
	W[60] = temp[12] ^ W[28] ^ mask[1]; W[61] = temp[13] ^ W[29] ^ mask[1]; W[62] = temp[14] ^ W[30] ^ mask[0]; W[63] = temp[15] ^ W[31] ^ mask[0];

	Masking_RoundKeyGeneration(W, RK, keysize);
}

void Masking_ARIA_ENC(u8 PT[], u8 CT[], int keysize, u8 RK[]) {
	int Nr = keysize / 32 + 8;
	int i;
	u8 temp[16];

	/* first input masking with (m'm'mm)4*/
	temp[0] = PT[0] ^ mask[1]; temp[1] = PT[1] ^ mask[1]; temp[2] = PT[2] ^ mask[0]; temp[3] = PT[3] ^ mask[0];
	temp[4] = PT[4] ^ mask[1]; temp[5] = PT[5] ^ mask[1]; temp[6] = PT[6] ^ mask[0]; temp[7] = PT[7] ^ mask[0];
	temp[8] = PT[8] ^ mask[1]; temp[9] = PT[9] ^ mask[1]; temp[10] = PT[10] ^ mask[0]; temp[11] = PT[11] ^ mask[0];
	temp[12] = PT[12] ^ mask[1]; temp[13] = PT[13] ^ mask[1]; temp[14] = PT[14] ^ mask[0]; temp[15] = PT[15] ^ mask[0];

	for (i = 1; i < Nr; i++) {
		/* 1 ~ Nr - 1 rounds */
		Masking_AddRoundKey(temp, RK + 16 * (i - 1));
		Masking_SubstLayer(temp, i);
		DiffLayer(temp);
		Masking_AddRoundKey2(temp);
	}
	/* last round */
	Masking_AddRoundKey(temp, RK + 16 * (i - 1));
	Masking_SubstLayer(temp, i);
	Masking_AddRoundKey(temp, RK + 16 * i);

	/* taking off masking */
	CT[0] = temp[0] ^ mask[1]; CT[1] = temp[1] ^ mask[1]; CT[2] = temp[2] ^ mask[0]; CT[3] = temp[3] ^ mask[0];
	CT[4] = temp[4] ^ mask[1]; CT[5] = temp[5] ^ mask[1]; CT[6] = temp[6] ^ mask[0]; CT[7] = temp[7] ^ mask[0];
	CT[8] = temp[8] ^ mask[1]; CT[9] = temp[9] ^ mask[1]; CT[10] = temp[10] ^ mask[0]; CT[11] = temp[11] ^ mask[0];
	CT[12] = temp[12] ^ mask[1]; CT[13] = temp[13] ^ mask[1]; CT[14] = temp[14] ^ mask[0]; CT[15] = temp[15] ^ mask[0];
}

void MaskingSboxComputation() {
	for (int i = 0; i < 256; i++) {
		maskS1box[i ^ mask[0]] = S1box[i] ^ mask[1];
		maskS2box[i ^ mask[0]] = S2box[i] ^ mask[1];
		maskinvS1box[i ^ mask[1]] = inv_S1box[i] ^ mask[0];
		maskinvS2box[i ^ mask[1]] = inv_S2box[i] ^ mask[0];
	}
}

int main() {

	int i;
	int keysize = 256;
	u8 PT[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	u8 MK[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
	u8 KL[16] = { 0x00 };
	u8 KR[16] = { 0x00 };
	u8 CT[16] = { 0x00 };
	u8 W[64] = { 0x00 };
	/* enough space for 256-bits key generation */
	/* 128-bit : 13 x 16 = 208 */
	/* 192-bit : 15 x 16 = 240 */
	/* 256-bit : 17 x 16 = 272 */
	u8 RK[272] = { 0x00 };
	srand(time(NULL));

	mask[0] = rand() % 255;
	mask[1] = rand() % 255;
	mask[2] = mask[0] ^ mask[1];
	printf("Masking : %02x %02x %02x\n", mask[0], mask[1], mask[2]);
	printf("===============================================================\n");

	MaskingSboxComputation();
	for (int i = 0; i < 16; i++) {
		KL[i] = MK[i];
		KR[i] = MK[i + 16];
	}

	printf("Plaintext : ");
	for (int i = 0; i < 16; i++)
		printf("%02x ", PT[i]);
	printf("\n");

	printf("Masterkey : ");
	for (int i = 0; i < 32; i++)
		printf("%02x ", MK[i]);
	printf("\n================================================================\n");

	printf("KL: ");
	for (int i = 0; i < 16; i++)
		printf("%02x ", KL[i]);
	printf("\n");

	printf("KR: ");
	for (int i = 0; i < 16; i++)
		printf("%02x ", KR[i]);
	printf("\n");

	Masking_ARIA_KeySchedule_Initialization(MK, KL, KR, W, RK, keysize);
	printf("==========================<Round Keys>==========================\n");
	for (int i = 0; i < 272; i++) {
		if (i % 16 == 0)
			printf("%2d round key : ", ((i / 16) + 1));
		printf("%02x ", RK[i]);
		if (i % 16 == 15)
			printf("\n");
	}
	printf("===============================================================\n");
	Masking_ARIA_ENC(PT, CT, keysize, RK);

	printf("Ciphertext: ");
	for (i = 0; i < 16; i++)
		printf("%02x ", CT[i]);
	printf("\n");

	return 0;
}
