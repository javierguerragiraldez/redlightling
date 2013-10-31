
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

#include "utils_l.h"

static uint64_t sipKey[2];
const size_t keySize = sizeof(sipKey);

/* Initialize hash seed with the best randomness available. */
void initHashSeed(void) {
	if (!getRandom(sipKey, sizeof(sipKey))) {
		uint32_t *randwords = (uint32_t*) sipKey;
		struct timeval tv;
		out_log("Could not open /dev/urandom. Using time and pid as random seed.");
		gettimeofday(&tv,NULL);
		randwords[0] = tv.tv_sec * tv.tv_usec;
		randwords[1] = tv.tv_sec;
		randwords[2] = tv.tv_usec;
		randwords[3] = getpid();
	}
}


#define ROTATE(x,b) x = (x << b) | (x >> (64 - b))

#define ROUND \
  v0 += v1; v2 += v3; \
  ROTATE(v1,13); ROTATE(v3,16); \
  v1 ^= v0; v3 ^= v2; \
  ROTATE(v0,32); \
  v2 += v1; v0 += v3; \
  ROTATE(v1,17); ROTATE(v3,21); \
  v1 ^= v2; v3 ^= v0; \
  ROTATE(v2,32);

uint64_t siphash(const uint8_t *in, size_t inlen, const uint64_t k[2])
{
  uint64_t v0 = k[0];
  uint64_t v1 = k[1];
  uint64_t lastblock = inlen << 56;
  uint64_t v2 = v0;
  uint64_t v3 = v1;
  int i;

  v0 ^= 0x736f6d6570736575;
  v1 ^= 0x646f72616e646f6d;
  v2 ^= 0x6c7967656e657261;
  v3 ^= 0x7465646279746573;

  while (inlen >= 8) { /* floor(inlen/8) normal loops */
    uint64_t mi = *(uint64_t *) in;
    in += 8;
    v3 ^= mi;
    ROUND
    ROUND
    v0 ^= mi;
    inlen -= 8;
  }
  /* now inlen <= 7; 1 extra loop; total loops: ceil((inlen+1)/8) */
  for (i = 0;i < inlen;++i) i[(char *) &lastblock] = i[in];
  v3 ^= lastblock;
  ROUND
  ROUND
  v0 ^= lastblock;

  v2 ^= 0xff;
  ROUND
  ROUND
  ROUND
  ROUND
  return (v0 ^ v1) ^ (v2 ^ v3);
}