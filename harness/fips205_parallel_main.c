/*
 * Benchmark main for FIPS_205_PARALLEL. Use it INSTEAD of main.c (keep a copy of yours),
 * with the parameter set selected in FIPS_205_Params.h matching the SPHINCS+ build.
 * Same key seed and message as the SPHINCS+ harnesses; deterministic signing (addrnd = PK.seed).
 * Writes the FIPS205_sign_internal_new__ signature to sig_ours.bin.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "FIPS_205_Params.h"
#include "FIPS_205_internal.h"
#include "avx_constants.h"
#include "bench_common.h"   /* copy harness/bench_common.h next to this file */
SUCCESS FIPS205_verify_internal(const uint8_t* M, uint32_t M_len, const uint8_t* SIG, uint32_t SIG_len, const uint8_t* PK);
int main(void)
{
    set_avx_constants();
    static uint8_t SK[4 * FIPS205_N], M[BENCH_MSG_LEN], s1[FIPS205_SIG_BYTES], s2[FIPS205_SIG_BYTES];
    for (int i = 0; i < 3 * FIPS205_N; i++) SK[i] = bench_seed_byte(i);   /* SK.seed || SK.prf || PK.seed */
    for (int i = 0; i < BENCH_MSG_LEN; i++) M[i] = (uint8_t)i;
    uint8_t* PK = SK + 2 * FIPS205_N;                                     /* PK.seed || PK.root */
    uint64_t k = ~0ULL, a = ~0ULL, b = ~0ULL, v = ~0ULL, t;
    for (int r = 0; r < BENCH_RUNS; r++) {
        t = __rdtsc(); FIPS205_keygen_internal(SK + 3 * FIPS205_N, SK, SK + FIPS205_N, PK); t = __rdtsc() - t; if (t < k) k = t;
        t = __rdtsc(); FIPS205_sign_internal(s1, M, BENCH_MSG_LEN, SK, PK); t = __rdtsc() - t; if (t < a) a = t;
        t = __rdtsc(); FIPS205_sign_internal_new__(s2, M, BENCH_MSG_LEN, SK, PK); t = __rdtsc() - t; if (t < b) b = t;
        t = __rdtsc(); SUCCESS ok = FIPS205_verify_internal(M, BENCH_MSG_LEN, s2, FIPS205_SIG_BYTES, PK); t = __rdtsc() - t; if (t < v) v = t;
        if (ok != OK) printf("VERIFY FAILED\n");
    }
    FILE* f = fopen("sig_ours.bin", "wb"); fwrite(s2, 1, FIPS205_SIG_BYTES, f); fclose(f);
    printf("FIPS_205_PARALLEL keygen %llu  sign_internal %llu  sign_internal_new__ %llu  verify %llu\n",
           (unsigned long long)k, (unsigned long long)a, (unsigned long long)b, (unsigned long long)v);
    printf("sign_internal == sign_internal_new__: %s   (sig_ours.bin, %d bytes)\n",
           memcmp(s1, s2, FIPS205_SIG_BYTES) == 0 ? "YES" : "NO", (int)FIPS205_SIG_BYTES);
    return 0;
}
