#include <stdio.h>
#include <stdint.h>
#include "mash.h"
 
// George Marsaglia's LFIB4,
//http://groups.google.com/group/sci.crypt/msg/eb4ddde782b17051
const double norm21 = 4.76837158203125e-7;      // 2^-21
 
static double s[256];
static uint8_t k0 = 0, k1 = 58, k2 = 119, k3 = 178;
 
static inline double lfib4(void) {
  double x;
 
  k0++;
  k1++;
  k2++;
  k3++;
 
  x = s[k0] - s[k1];
  if (x < 0.0) {
    x += 1.0;
  }
  x -= s[k2];
  if (x < 0.0) {
    x += 1.0;
  }
  x -= s[k3];
  if (x < 0.0) {
    x += 1.0;
  }
 
  return s[k0] = x;
}
 
int main(int argc, unsigned char *argv[]) {
  double buffer[256];
  int i, j;
 
  for (j = 0; j < 256; j++) {
    s[j] = mash(" ");
    s[j] -= mash(" ") * norm21;  // 2^-21
    if (s[j] < 0.0) {
      s[j] += 1.0;
    }
  }
  for (i = 1; i < argc; i++) {
    for (j = 0; j < 256; j++) {
      s[j] -= mash(argv[i]);
      s[j] -= mash(argv[i]) * norm21;   // 2^-21
      if (s[j] < 0.0) {
        s[j] += 1.0;
      }
    }
  }
 
  while (1) {
    for (i = 0; i < 256; i++) {
      buffer[i] = lfib4();
    }
    fwrite(buffer, sizeof buffer, 1, stdout);
  }
}