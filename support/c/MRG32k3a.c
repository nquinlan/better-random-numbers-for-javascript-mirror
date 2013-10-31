#include <stdio.h>
#include <stdint.h>
#include "mash.h"
 
// Copyright (c) 2002 Pierre L'Ecuyer, DIRO, Université de Montréal.
//  http://www.iro.umontreal.ca/~lecuyer/
#define norm 2.328306549295728e-10
#define m1    4294967087.0
#define m2    4294944443.0
#define a12      1403580.0
#define a13n      810728.0
#define a21       527612.0
#define a23n     1370589.0
 
static double s10 = 12345, s11 = 12345, s12 = 123,
  s20 = 12345, s21 = 12345, s22 = 123;
 
double MRG32k3a(void) {
  long k;
  double p1, p2;
  /* Component 1 */
  p1 = a12 * s11 - a13n * s10;
  k = p1 / m1;
  p1 -= k * m1;
  if (p1 < 0.0)
    p1 += m1;
  s10 = s11;
  s11 = s12;
  s12 = p1;
  /* Component 2 */
  p2 = a21 * s22 - a23n * s20;
  k = p2 / m2;
  p2 -= k * m2;
  if (p2 < 0.0)
    p2 += m2;
  s20 = s21;
  s21 = s22;
  s22 = p2;
  /* Combination */
  if (p1 <= p2)
    return ((p1 - p2 + m1) * norm);
  else
    return ((p1 - p2) * norm);
}
 
int main(int argc, unsigned char *argv[]) {
  double buffer[256];
  int i;
 
  for (i = 1; i < argc; i++) {
    s10 += mash(argv[i]) * 0x100000000ULL;   // 2^32
    s11 += mash(argv[i]) * 0x100000000ULL;   // 2^32
    s12 += mash(argv[i]) * 0x100000000ULL;   // 2^32
    s20 += mash(argv[i]) * 0x100000000ULL;   // 2^32
    s21 += mash(argv[i]) * 0x100000000ULL;   // 2^32
    s22 += mash(argv[i]) * 0x100000000ULL;   // 2^32
  }
 
  s10 = (uint64_t) s10 % (uint32_t) m1;
  s11 = (uint64_t) s11 % (uint32_t) m1;
  s12 = (uint64_t) s12 % (uint32_t) m1;
  s20 = (uint64_t) s20 % (uint32_t) m2;
  s21 = (uint64_t) s21 % (uint32_t) m2;
  s22 = (uint64_t) s22 % (uint32_t) m2;
 
  while (1) {
    for (i = 0; i < 256; i++) {
      buffer[i] = MRG32k3a();
    }
    fwrite(buffer, sizeof buffer, 1, stdout);
  }
}