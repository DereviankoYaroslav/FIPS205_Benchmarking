#!/bin/sh
# Build and run the SPHINCS+ baselines for one parameter set.
# Usage: ./build.sh 128f        (any of 128s 128f 192s 192f 256s 256f)
# Works on Linux, WSL and MSYS2/MinGW shells.
set -e
PS=${1:-128f}
CFLAGS="-O3 -std=c99 -march=native -fomit-frame-pointer"
SRC_V31="hash_sha2.c hash_sha2x8.c thash_sha2_simple.c thash_sha2_simplex8.c sha2.c sha256x8.c sha512x4.c sha256avx.c address.c merkle.c wots.c utils.c utilsx8.c fors.c sign.c"
SRC_R3="hash_sha256.c hash_sha256x8.c thash_sha256_simple.c thash_sha256_simplex8.c sha256.c sha256x8.c sha256avx.c address.c wots.c utils.c utilsx8.c fors.c sign.c"
SRC_OURS="AVX256.c AVX512.c avx_constants.c Common.c fips202.c FIPS205_internal.c FIPS_205_fors.c FIPS_205_fors_old.c FIPS_205_ht.c FIPS_205_ht_old.c FIPS_205_internal_old.c FIPS_205_json_tests.c FIPS_205_xmss_old.c FIPS_205_Addr.c FIPS_205_Common_fun_old.c FIPS_205_hashs.c FIPS_205_hashs_old.c FIPS_205_WOTS.c FIPS_205_wots_old.c FIPS_205_xmss.c OLD/address.c OLD/fors.c OLD/hash_sha256.c OLD/hash_sha256x8.c OLD/sha256.c OLD/sha256avx.c OLD/sha256x8.c OLD/sign.c OLD/thash_sha256_simple.c OLD/thash_sha256_simplex8.c OLD/utils.c OLD/utilsx8.c OLD/wots.c print.c SHA256_device.c sha256_old.c SHA512.c Test.c"
ROOT=$(cd "$(dirname "$0")" && pwd)

build_run() {   # $1 label, $2 source dir, $3 sources, $4 harness, $5 extra flags, $6 extra files
  OUT="$ROOT/out/$1/$PS"; mkdir -p "$OUT"
  if ! ( cd "$2" && gcc $CFLAGS $5 -I. -o "$OUT/bench" $3 $6 "$ROOT/harness/$4" ) > "$OUT/build.log" 2>&1; then
    echo "build of $1 failed:"
    tail -n 30 "$OUT/build.log"
    exit 1
  fi
  ( cd "$OUT" && OMP_NUM_THREADS=1 ./bench )
}

# Round 3 (2020): parameter set chosen through params.h; needs -fcommon
# (state_seeded is defined in a header); no SHA-512, no internal API.
R3="$ROOT/sphincsplus-round3-75b8ce5/sha256-avx2"
INC="$ROOT/out/round3-75b8ce5/$PS/inc"; mkdir -p "$INC"
echo "#include \"params/params-sphincs-sha256-$PS.h\"" > "$INC/params.h"

RB="$ROOT/harness/bench_randombytes.c"
build_run round3-75b8ce5 "$R3" "$SRC_R3" bench_round3.c "-fcommon -I$INC" "$RB"

# v3.1 (master) and FIPS 205 (bas/fips205): parameter set chosen with -DPARAMS
build_run master-7ec789a  "$ROOT/sphincsplus-master-7ec789a/sha2-avx2"  "$SRC_V31" bench_master.c  "-DPARAMS=sphincs-sha2-$PS" "$RB"
build_run fips205-ef36d20 "$ROOT/sphincsplus-fips205-ef36d20/sha2-avx2" "$SRC_V31" bench_fips205.c "-DPARAMS=sphincs-sha2-$PS" "$RB"
build_run fips205-parallel-14f642a "$ROOT/fips205-parallel-14f642a" "$SRC_OURS" fips205_parallel_main.c "-mavx2 -msha -mfma -fopenmp -D_PREDCALC -Wno-incompatible-pointer-types -DFIPS_205_MODE=SLH_DSA_SHA2_$PS" ""