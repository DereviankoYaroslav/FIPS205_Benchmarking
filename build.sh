#!/bin/sh
# Build and run both SPHINCS+ baselines for one parameter set.
# Usage: ./build.sh 128f        (any of 128s 128f 192s 192f 256s 256f)
# Works on Linux and in MSYS2/MinGW shells on Windows.
set -e
PS=${1:-128f}
CFLAGS="-O3 -std=c99 -march=native -fomit-frame-pointer"
SRC="hash_sha2.c hash_sha2x8.c thash_sha2_simple.c thash_sha2_simplex8.c sha2.c sha256x8.c sha512x4.c sha256avx.c address.c merkle.c wots.c utils.c utilsx8.c fors.c sign.c"
ROOT=$(cd "$(dirname "$0")" && pwd)
for V in master-7ec789a fips205-ef36d20 round3; do
  D="$ROOT/sphincsplus-$V/sha2-avx2"
  H=bench_fips205.c; [ "$V" = master-7ec789a ] && H=bench_master.c
  OUT="$ROOT/out/$V/$PS"; mkdir -p "$OUT"
  ( cd "$D" && gcc $CFLAGS -I. -DPARAMS=sphincs-sha2-$PS -o "$OUT/bench" $SRC \
        "$ROOT/harness/bench_randombytes.c" "$ROOT/harness/$H" )
  ( cd "$OUT" && ./bench )
done
