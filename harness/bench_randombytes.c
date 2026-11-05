/*
 * Portable replacement for SPHINCS+ randombytes.c, FOR BENCHMARKING ONLY.
 * The upstream randombytes.c reads /dev/urandom, which does not exist on Windows.
 * This version fills the buffer from a fixed counter, so runs are reproducible.
 * It is NOT a secure random number generator.
 */
#include <stddef.h>
#include "randombytes.h"
void randombytes(unsigned char *x, unsigned long long xlen)
{
    static unsigned char c = 0x5A;
    for (unsigned long long i = 0; i < xlen; ++i) x[i] = c++;
}
