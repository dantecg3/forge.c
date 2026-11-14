#!/bin/bash
set -e

mode="${1:-debug}"
flags="-std=c99 -Wall -Wextra -pedantic -Iincl -Ivendor/mec/incl"

case "$mode" in
    debug)    flags="$flags -O0 -g" ;;
    release)  flags="$flags -O2 -DNDEBUG" ;;
    tsan)     flags="$flags -O1 -g -fsanitize=thread" ;;
    asan)     flags="$flags -O1 -g -fsanitize=address,undefined" ;;
    *) echo "usage: $0 [debug|release|tsan|asan]"; exit 1 ;;
esac

echo "building 'forge.c' ($mode)"
cc $flags src/forge.c -o "forge"
echo "built forge"

