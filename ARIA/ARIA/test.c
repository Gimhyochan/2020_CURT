#include <stdio.h>
#include "ARIA.h"
/* 아래는 64비트로 쪼개서 다뤘을 때의 라운드키 생성 코드, 왜인진 몰라도 64로 다루려니 앞 32비트가 짤려서
	   32로 쪼개서 코드를 작성해봄 */
	   /*u64 temp[8]; // 키 초기화에서 생성된거 원본
	   /* W[0] = temp[0] ~ temp[1]; W[1] = temp[2] ~ temp[3]
		  W[2] = temp[4] ~ temp[5]; W[3] = temp[6] ~ temp[7]
	   u64 result[8]; // 키 비트 옮기기 위해서 연산된 결과를 담는 배열
	   int i;

	   for (i = 0; i < 64; i += 8) {
		   temp[i / 8] = u4byte_in64(W + i);
		   result[i / 8] = temp[i / 8];
	   }

	   // ek1
	   result[2] = (temp[3] & 0x7ffff) | ((temp[2] >> 19) & 0x1fffffffffff);
	   result[3] = (temp[2] & 0x7ffff) | ((temp[3] >> 19) & 0x1fffffffffff);
	   u4byte_out64(RK, temp[0] ^ result[2]);
	   u4byte_out64(RK + 8, temp[1] ^ result[3]);

	   // ek2
	   result[4] = (temp[5] & 0x7ffff) | ((temp[4] >> 19) & 0x1fffffffffff);
	   result[5] = (temp[4] & 0x7ffff) | ((temp[5] >> 19) & 0x1fffffffffff);
	   u4byte_out64(RK + 16, temp[2] ^ result[4]);
	   u4byte_out64(RK + 24, temp[3] ^ result[5]);

	   // ek3
	   result[6] = (temp[7] & 0x7ffff) | ((temp[6] >> 19) & 0x1fffffffffff);
	   result[7] = (temp[6] & 0x7ffff) | ((temp[7] >> 19) & 0x1fffffffffff);
	   u4byte_out64(RK + 32, temp[4] ^ result[6]);
	   u4byte_out64(RK + 40, temp[5] ^ result[7]);

	   // ek4
	   result[0] = (temp[1] & 0x7ffff) | ((temp[0] >> 19) & 0x1fffffffffff);
	   result[1] = (temp[0] & 0x7ffff) | ((temp[1] >> 19) & 0x1fffffffffff);
	   u4byte_out64(RK + 48, temp[6] ^ result[0]);
	   u4byte_out64(RK + 56, temp[7] ^ result[1]);

	   // ek5
	   result[2] = (temp[3] & 0x7fffffff) | ((temp[2] >> 31) & 0x1ffffffff);
	   result[3] = (temp[2] & 0x7fffffff) | ((temp[3] >> 19) & 0x1ffffffff);
	   u4byte_out64(RK + 64, temp[0] ^ result[2]);
	   u4byte_out64(RK + 72, temp[1] ^ result[3]);

	   // ek6
	   result[4] = (temp[5] & 0x7fffffff) | ((temp[4] >> 31) & 0x1ffffffff);
	   result[5] = (temp[4] & 0x7fffffff) | ((temp[5] >> 19) & 0x1ffffffff);
	   u4byte_out64(RK + 80, temp[2] ^ result[4]);
	   u4byte_out64(RK + 88, temp[3] ^ result[5]);

	   // ek7
	   result[6] = (temp[7] & 0x7fffffff) | ((temp[6] >> 31) & 0x1ffffffff);
	   result[7] = (temp[6] & 0x7fffffff) | ((temp[7] >> 19) & 0x1ffffffff);
	   u4byte_out64(RK + 96, temp[4] ^ result[6]);
	   u4byte_out64(RK + 104, temp[5] ^ result[7]);

	   // ek8
	   result[0] = (temp[1] & 0x7fffffff) | ((temp[0] >> 31) & 0x1ffffffff);
	   result[1] = (temp[0] & 0x7fffffff) | ((temp[1] >> 19) & 0x1ffffffff);
	   u4byte_out64(RK + 112, temp[6] ^ result[0]);
	   u4byte_out64(RK + 120, temp[7] ^ result[1]);

	   // ek9
	   result[2] = (temp[2] & 0x7) | ((temp[3] >> 3) & 0x1fffffffffffffff);
	   result[3] = (temp[3] & 0x7) | ((temp[2] >> 3) & 0x1fffffffffffffff);
	   u4byte_out64(RK + 128, temp[0] ^ result[2]);
	   u4byte_out64(RK + 136, temp[1] ^ result[3]);

	   // ek10
	   result[4] = (temp[4] & 0x7) | ((temp[5] >> 3) & 0x1fffffffffffffff);
	   result[5] = (temp[5] & 0x7) | ((temp[4] >> 3) & 0x1fffffffffffffff);
	   u4byte_out64(RK + 144, temp[2] ^ result[4]);
	   u4byte_out64(RK + 152, temp[3] ^ result[5]);

	   // ek11
	   result[6] = (temp[6] & 0x7) | ((temp[7] >> 3) & 0x1fffffffffffffff);
	   result[7] = (temp[7] & 0x7) | ((temp[6] >> 3) & 0x1fffffffffffffff);
	   u4byte_out64(RK + 160, temp[4] ^ result[6]);
	   u4byte_out64(RK + 168, temp[5] ^ result[7]);

	   // ek12
	   result[0] = (temp[0] & 0x7) | ((temp[1] >> 3) & 0x1fffffffffffffff);
	   result[1] = (temp[1] & 0x7) | ((temp[0] >> 3) & 0x1fffffffffffffff);
	   u4byte_out64(RK + 176, temp[6] ^ result[0]);
	   u4byte_out64(RK + 184, temp[7] ^ result[1]);

	   // ek13
	   result[2] = (temp[2] & 0x1ffffffff) | ((temp[3] >> 33) & 0x7fffffff);
	   result[3] = (temp[3] & 0x1ffffffff) | ((temp[2] >> 33) & 0x7fffffff);
	   u4byte_out64(RK + 192, temp[0] ^ result[2]);
	   u4byte_out64(RK + 200, temp[1] ^ result[3]); */

int main() {

	u16 a = 0x1234;
	u8 b = 0x63;
	printf("%x\n", (u8)(a ^ b));

	return 0;
}