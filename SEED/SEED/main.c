#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "seed.h"
void SEED_ENC(unsigned int PT[], unsigned int RK[], unsigned int CT[]) {
	int i = 0;
	for (; i < 15; i++) {
		unsigned int temp[2] = { PT[2], PT[3] };
		Xor64(PT, F_function(&PT[2], &RK[2 * i]));
	}
	
}

void Merge_func();		// 8(또는 16)개의 char형 변수를 int 자료형에 합쳐주는 함수
void KeyGenerate();		// 키 생성 함수
void F();		// F 함수
void G();		// G 함수
void FeistelStructure();

void Merge_func(uint* dst, uchar* src)
{
	dst[0] = src[0x0] * 0x1000000 + src[0x1] * 0x10000 + src[0x2] * 0x100 + src[0x3] * 0x1;
	dst[1] = src[0x4] * 0x1000000 + src[0x5] * 0x10000 + src[0x6] * 0x100 + src[0x7] * 0x1;

}

void FeistelStructure(u32* L, u32* R, u32* K, int i)
{
	u32 LS[16] = { 0 };
	u32 RS[16] = { 0 };
	u32 FS[16] = { 0 };

	LS[0] = L[0];
	RS[0] = R[0];
	F(&FS[0], &RS[0], &K[1], i);
	LS[0] ^= FS[0];

	for (int i = 0; i < 15; i++) {
		LS[i + 1] = RS[i];
		RS[i + 1] = LS[i];
		F(&FS[i + 1], &RS[i + 1], &K[i + 2], i);
		LS[i + 1] ^= FS[i + 1];
	}
}

void F(u32* C, u32* D, u32* KL, u32* KR, int i)
{
	u32 CP[16] = { 0 };
	u32 DP[16] = { 0 };

	CP[i] = G[G[G{ (C ^ KL[i]) ^ (D ^ KR[i]) } ^ (C ^ KL[i])] ^ G{ (C ^ KL[i] ^ (D ^ KR[i] }] ^ G[G{ (C ^ KL[i]) ^ (D ^ KR[i]) } ^ (C ^ KL[i])];
	DP

}

void G(u32* In)
{
	// G 함수는 확장된 4바이트 SS-box들을 이용
	*In = (SS0[*In & 0xff] ^ SS1[(*In >> 8) & 0xff] ^ SS2[(*In >> 16) & 0xff] ^ SS3[(*In >> 24) & 0xff]);
}

// 라운드 상수 
const unsigned int KC[16] = {
	0x9E3779B9, 0x3C6EF373, 0x78DDE6E6, 0xF1BBCDCC,
	0xE3779B99, 0xC6EF3733, 0x8DDE6E67, 0x1BBCDCCF,
	0x3779B99E, 0x6EF3733C, 0xDDE6E678, 0xBBCDCCF1,
	0x779B99E3, 0xEF3733C6, 0xDE6E678D, 0xBCDCCF1B
};
void KeyGenerate(uint* K, uint* Key_R, int i)
{
	// G_function() 함수에 주소를 넣어줘야해서 tmp라는 변수를 따로 만들어줌.
	uint tmp[2] = { 0x00000000, 0x00000000 };
	tmp[0] = K[0] + K[2] - KC[i - 1];
	tmp[1] = K[1] - K[3] + KC[i - 1];

	G_function(&tmp[0]);
	G_function(&tmp[1]);

	Key_R[0] = tmp[0];
	Key_R[1] = tmp[1];

	// Test
	printf("%d Round KEY :\n%08X %08X\n\n", i, Key_R[0], Key_R[1]);

	// Rotation() 함수를 만들었는데, 이 함수에 필요한 다른 함수들이 너무 많아서
	// 그냥 다 지우고 하드 코딩함.
	uchar Rot_tmp[8] = { 0x00, };
	if (i % 2 == 1)
	{
		Rot_tmp[0] = (K[1] >> 0) & 0xff;
		Rot_tmp[1] = (K[0] >> 24) & 0xff;
		Rot_tmp[2] = (K[0] >> 16) & 0xff;
		Rot_tmp[3] = (K[0] >> 8) & 0xff;
		Rot_tmp[4] = (K[0] >> 0) & 0xff;
		Rot_tmp[5] = (K[1] >> 24) & 0xff;
		Rot_tmp[6] = (K[1] >> 16) & 0xff;
		Rot_tmp[7] = (K[1] >> 8) & 0xff;

		Merge_func(&K[0], &Rot_tmp[0]);
	}
	else
	{
		Rot_tmp[0] = (K[2] >> 16) & 0xff;
		Rot_tmp[1] = (K[2] >> 8) & 0xff;
		Rot_tmp[2] = (K[2] >> 0) & 0xff;
		Rot_tmp[3] = (K[3] >> 24) & 0xff;
		Rot_tmp[4] = (K[3] >> 16) & 0xff;
		Rot_tmp[5] = (K[3] >> 8) & 0xff;
		Rot_tmp[6] = (K[3] >> 0) & 0xff;
		Rot_tmp[7] = (K[2] >> 24) & 0xff;

		Merge_func(&K[2], &Rot_tmp[0]);
	}
}


int main()
{
	// char In[17]  = {0x00, };
	// char Key[17] = {0x00, };

	// uint K[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
	// uint L[2] = {0x00010203, 0x04050607};
	// uint R[2] = {0x08090a0b, 0x0c0d0e0f};

	// Merge_func(&L[0], &In[0]);
	// Merge_func(&R[0], &In[8]);
	// Merge_func(&K[0], &Key[0]);
	// Merge_func(&K[0], &Key[8]);

	// Test Vector
#if 0
	K[0] = 0x00000000;
	K[1] = 0x00000000;
	K[2] = 0x00000000;
	K[3] = 0x00000000;

	L[0] = 0x00010203;
	L[1] = 0x04050607;
	R[0] = 0x08090a0b;
	R[1] = 0x0c0d0e0f;
#endif

#if 0
	K[0] = 0x00010203;
	K[1] = 0x04050607;
	K[2] = 0x08090a0b;
	K[3] = 0x0c0d0e0f;

	L[0] = 0x00000000;
	L[1] = 0x00000000;
	R[0] = 0x00000000;
	R[1] = 0x00000000;
#endif

#if 0
	K[0] = 0x47064808;
	K[1] = 0x51e61be8;
	K[2] = 0x5d74bfb3;
	K[3] = 0xfd956185;

	L[0] = 0x83a2f8a2;
	L[1] = 0x88641fb9;
	R[0] = 0xa4e9a5cc;
	R[1] = 0x2f131c7d;
#endif

#if 0
	K[0] = 0x28DBC3BC;
	K[1] = 0x49FFD87D;
	K[2] = 0xCFA509B1;
	K[3] = 0x1D422BE7;

	L[0] = 0xB41E6BE2;
	L[1] = 0xEBA84A14;
	R[0] = 0x8E2EED84;
	R[1] = 0x593C5EC7;
#endif

	printf("Initial Value :\n%08x%08x %08x%08x\n\n", L[0], L[1], R[0], R[1]);

	int i = 0;
	for (i = 1; i <= 15; i++)
	{
		Feistel(&L[0], &R[0], &K[0], i);		// 암호화 부분(평문, 키, 라운드 수)
		printf("%d Round Ciphertext :\n", i);
		printf("%08X%08X %08X%08X\n", L[0], L[1], R[0], R[1]);	// 암호문을 출력
		printf("\n");
	}

	Feistel(&L[0], &R[0], &K[0], 16);
	printf("Ciphertext :\n");
	printf("%08X %08X %08X %08X\n", R[0], R[1], L[0], L[1]);

	return 0;
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