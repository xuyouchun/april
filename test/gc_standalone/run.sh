#!/bin/sh
# Builds and runs the standalone gc tests (no boost, no april build needed).
# Usage: ./run.sh            (needs g++ with C++20 and ASAN/UBSAN)
set -e
cd "$(dirname "$0")"

SAN="-fsanitize=address,undefined -fno-omit-frame-pointer"
OUT="${TMPDIR:-/tmp}"

g++ -std=c++20 -g -O0 $SAN -I shim -o "$OUT/april_gc_test" \
    ../../modules/mm/gc_heap.cpp gc_test.cpp
g++ -std=c++20 -g -O0 $SAN -o "$OUT/april_gc_stack_test" gc_stack_test.cpp

# detect_leaks=0: the tests leak the heap instances on purpose.
ASAN_OPTIONS=detect_leaks=0 "$OUT/april_gc_test"
ASAN_OPTIONS=detect_leaks=0 "$OUT/april_gc_stack_test"
