#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void Keyschedule(unsigned char MK[], unsigned char RK[]) {

}

HLIGHT_ENC(unsigned char PT[], unsigned char RK[], unsigned char CT[]) {

}
int main() {
	unsigned char PT[8];
	unsigned char MK[16];
	unsigned char CT[8];
	unsigned char RK[1000];
	Keyschedule(MK, RK);
	HLIGHT_ENC(PT, RK, CT);
}