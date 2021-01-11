#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "seed.h"


void F(u32 C[], u32 D[], u32 KL[], u32 KR[], int i)
{
	u32 CP[16] = { 0 };
	u32 DP[16] = { 0 };

	for (int i = 0; i < 16; i++) {
		CP[i] = G(G(G(((C[i] ^ KL[i]) ^ (D[i] ^ KR[i]))) ^ (C[i] ^ KL[i])) ^ G(( (C[i] ^ KL[i]) ^ (D[i] ^ KR[i]) ))) ^ G( G(( (C[i] ^ KL[i]) ^ (D[i] ^ KR[i]) )) ^ (C[i] ^ KL[i]));
		DP[i] = G(G(G(((C[i] ^ KL[i]) ^ (D[i] ^ KR[i]))) ^ (C[i] ^ KL[i])) ^ G(( (C[i] ^ KL[i]) ^ (D[i] ^ KR[i]) )));
	}
}
u32 G(u32 C)
{

	return (SS0[(C & 0xff)] ^ SS1[(C >> 8) & 0xff] ^ SS2[(C >> 16) & 0xff] ^ SS3[(C >> 24) & 0xff]);

}
const unsigned int Constant[16] = {
	0x9E3779B9, 0x3C6EF373, 0x78DDE6E6, 0xF1BBCDCC,
	0xE3779B99, 0xC6EF3733, 0x8DDE6E67, 0x1BBCDCCF,
	0x3779B99E, 0x6EF3733C, 0xDDE6E678, 0xBBCDCCF1,
	0x779B99E3, 0xEF3733C6, 0xDE6E678D, 0xBCDCCF1B
};
void SEED_KeyWordToByte(u32 W[], u8 RK[])
{
	int i;
	for (i = 0; i < 44; i++)
	{
		u4byte_out(RK + 4 * i, W[i]); //RK[4i]||RK[4i+1]||RK[4i+2]||RK[4i+3] <-- W[i]
	}
}
void SEED_KeySchedule(u8 MK[], u8 RK[])
{

	u32 KL[16] = { 0 };
	u32 KR[16] = { 0 };
	for (int i = 1; i <= 16; i++) {
		KL[i - 1] = G(MK[0] + MK[2] - Constant[i-1]);
		KR[i - 1] = G(MK[1] + MK[3] + Constant[i-1]);

		if (i % 2 == 1)
			return (MK[0] || MK[1]) >> 8;
		else
			return (MK[2] || MK[3]) << 8;

	}
	
}
void SEED_ENC(u32 PT[], u32 K[], u32 CT[])
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
/*void SEED_ENC(unsigned int PT[], unsigned int RK[], unsigned int CT[]) {
	int i = 0;
	for (; i < 15; i++) {
		unsigned int temp[2] = { PT[2], PT[3] };
		Xor64(PT, F(&PT[2], &RK[2 * i]));
	}

}*/



int main() {
	unsigned int PT[16] = { 0x00000000, };
	unsigned int CT[16] = { 0x00000000, };
	unsigned int RK[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
	unsigned int MK[4] = { 0x00000000, };

	SEED_KeySchedule();
	SEED_ENC(PT, RK, CT);
	return 0;
}