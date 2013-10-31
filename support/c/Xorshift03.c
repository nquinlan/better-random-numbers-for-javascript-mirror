#include <stdio.h>
#include <stdint.h>
#include "mash.h"
 
// George Marsaglia, 13 May 2003
// http://groups.google.com/group/comp.lang.c/msg/e3c4ea1169e463ae
 
const double norm32 = 2.3283064365386963e-10;   // 2^-32
 
static uint32_t x = 123456789, y = 362436069,
  z = 521288629, w = 88675123, v = 886756453;
 
double xorshift03(void) {
  unsigned long t;
  t = (x ^ (x >> 7));
  x = y;
  y = z;
  z = w;
  w = v;
  v = (v ^ (v << 6)) ^ (t ^ (t << 13));
  return (y + y + 1#41; * v * norm32;      // 2^-32
}
 
int main(int argc, unsigned char *argv[]) {
  double buffer[256];
  int i;
 
  for (i = 1; i < argc; i++) {
    x ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    y ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    z ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    v ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    w ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
  }
 
  while (1) {
    for (i = 0; i < 256; i++) {
      buffer[i] = xorshift03();
    }
    fwrite(buffer, sizeof buffer, 1, stdout);
  }
}