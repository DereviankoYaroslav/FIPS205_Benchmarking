#!/bin/sh
# Rerun the already-built benchmarks for one parameter set, alternating implementations.
# Usage: ./run.sh 128s [rounds]       (run ./buildV2.sh <set> first)
PS=${1:-128f}
ROUNDS=${2:-3}
ROOT=$(cd "$(dirname "$0")" && pwd)
i=1
while [ "$i" -le "$ROUNDS" ]; do
  echo "--- round $i ---"
  for d in fips205-parallel-14f642a master-7ec789a fips205-ef36d20 round3-75b8ce5; do
    ( cd "$ROOT/out/$d/$PS" && OMP_NUM_THREADS=${THREADS:-1} ./bench )
  done
  i=$((i + 1))
done