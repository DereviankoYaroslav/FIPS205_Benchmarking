#ifndef BENCH_COMMON_H
#define BENCH_COMMON_H
#include <stdint.h>
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#define BENCH_RUNS 7      /* the minimum over this many runs is reported */
#define BENCH_MSG_LEN 33  /* message: bytes 0, 1, ..., 32 */
/* Key seed bytes: seed[i] = i*7 + 1 (SK.seed || SK.prf || PK.seed), identical in all harnesses */
static inline uint8_t bench_seed_byte(int i) { return (uint8_t)(i * 7 + 1); }
#endif
