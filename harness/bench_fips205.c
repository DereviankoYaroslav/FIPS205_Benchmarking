/* Benchmark for sphincs/sphincsplus branch bas/fips205, sha2-avx2.
 * Uses the FIPS 205 internal functions (slh_sign_internal / slh_verify_internal),
 * deterministic signing (addrnd = NULL -> PK.seed), and writes the signature to sig_spx.bin. */
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
        t = __rdtsc(); crypto_sign_signature_internal(sig, &siglen, M, BENCH_MSG_LEN, NULL, 0, sk, NULL); t = __rdtsc() - t; if (t < s) s = t;
        t = __rdtsc(); int bad = crypto_sign_verify_internal(sig, siglen, M, BENCH_MSG_LEN, NULL, 0, pk); t = __rdtsc() - t; if (t < v) v = t;
        if (bad) printf("VERIFY FAILED\n");
    }
    FILE *f = fopen("sig_spx.bin", "wb"); fwrite(sig, 1, siglen, f); fclose(f);
    printf("SPHINCS+ fips205 keygen %llu  sign %llu  verify %llu  (sig_spx.bin, %zu bytes)\n",
           (unsigned long long)k, (unsigned long long)s, (unsigned long long)v, siglen);
    return 0;
}
