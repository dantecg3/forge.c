#!/bin/bash
set -e

mode="${1:-debug}"
flags="-std=c99 -Wall -Wextra -pedantic -Iincl"

case "$mode" in
    debug)    flags="$flags -O0 -g" ;;
    release)  flags="$flags -O2 -DNDEBUG" ;;
    tsan)     flags="$flags -O1 -g -fsanitize=thread" ;;
    asan)     flags="$flags -O1 -g -fsanitize=address,undefined" ;;
    *) echo "usage: $0 [debug|release|tsan|asan]"; exit 1 ;;
esac

output="bin/${mode}"

echo "building 'forge.c' ($mode)"

mkdir -p "$output"
cc $flags src/forge.c -o "$output/forge"

echo "built $output/forge"

