/* Benchmark for sphincs/sphincsplus master (SPHINCS+ v3.1), sha2-avx2.
 * master has no internal API; crypto_sign_signature draws optrand via randombytes(),
 * so its signatures are NOT FIPS 205 and are not written to a file. */
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "params.h"
#include "bench_common.h"
int main(void)
{
    uint8_t seed[3 * SPX_N], pk[SPX_PK_BYTES], sk[SPX_SK_BYTES], M[BENCH_MSG_LEN];
    static uint8_t sig[SPX_BYTES];
    size_t siglen;
    for (int i = 0; i < 3 * SPX_N; i++) seed[i] = bench_seed_byte(i);
    for (int i = 0; i < BENCH_MSG_LEN; i++) M[i] = (uint8_t)i;
    uint64_t k = ~0ULL, s = ~0ULL, v = ~0ULL, t;
    for (int r = 0; r < BENCH_RUNS; r++) {
        t = __rdtsc(); crypto_sign_seed_keypair(pk, sk, seed); t = __rdtsc() - t; if (t < k) k = t;
        t = __rdtsc(); crypto_sign_signature(sig, &siglen, M, BENCH_MSG_LEN, sk); t = __rdtsc() - t; if (t < s) s = t;
        t = __rdtsc(); int bad = crypto_sign_verify(sig, siglen, M, BENCH_MSG_LEN, pk); t = __rdtsc() - t; if (t < v) v = t;
        if (bad) printf("VERIFY FAILED\n");
    }
    printf("SPHINCS+ master  %-6s keygen %llu  sign %llu  verify %llu\n", "",
           (unsigned long long)k, (unsigned long long)s, (unsigned long long)v);
    return 0;
}
