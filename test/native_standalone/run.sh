#!/bin/sh
# Builds and runs the standalone tests of the native back end
# (no boost, no april build, no LLVM needed).
set -e
cd "$(dirname "$0")"

g++ -std=c++20 -g -O0 -Wall -Wextra -fsanitize=address,undefined \
    -o "${TMPDIR:-/tmp}/april_ll_builder_test" ll_builder_test.cpp

ASAN_OPTIONS=detect_leaks=1 "${TMPDIR:-/tmp}/april_ll_builder_test"

# The clang driver (build / run), tested with fake clang++ scripts.
g++ -std=c++20 -g -O0 -Wall -Wextra -fsanitize=address,undefined \
    -o "${TMPDIR:-/tmp}/april_driver_test" driver_test.cpp ../../modules/native/driver.cpp

ASAN_OPTIONS=detect_leaks=1 "${TMPDIR:-/tmp}/april_driver_test"
