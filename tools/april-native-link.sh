#!/bin/sh
# Builds a native executable from LLVM IR (.ll), by hand: `april compile --native` does it,
# the IR is in the directory of the builds (see APRIL_CACHE) as program.ll.
#
#   tools/april-native-link.sh <program.ll> [output] [libs-dir]
#
# Needs clang++ with LLVM 15+ (the IR uses opaque pointers), set CLANG to use another
# one. The libs dir is where the static libraries of april are: it defaults to
# libs/ (Makefile build) or build/ (CMake build), whichever has libnative.a.
set -e

IR="$1"
if [ -z "$IR" ]; then
    echo "usage: $0 <program.ll> [output] [libs-dir]" >&2
    exit 2
fi

OUT="${2:-${IR%.ll}}"
LIBS="$3"
HOME_DIR="$(cd "$(dirname "$0")/.." && pwd)"

if [ -z "$LIBS" ]; then
    for dir in "$HOME_DIR/libs" "$HOME_DIR/build"; do
        if [ -f "$dir/libnative.a" ]; then
            LIBS="$dir"
            break
        fi
    done
fi

if [ -z "$LIBS" ] || [ ! -f "$LIBS/libnative.a" ]; then
    echo "error: libnative.a not found: build april first, or pass the libs dir" >&2
    exit 1
fi

CLANG="${CLANG:-clang++}"

# The libraries reference each other: link them as a group.
exec "$CLANG" -O2 "$IR" -o "$OUT" \
    -Wl,--start-group "$LIBS"/lib*.a -Wl,--end-group \
    -lboost_log -lboost_system -lboost_filesystem -lpthread
