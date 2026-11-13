# Forge
Tool for building C projects, with a C program.

Targets C99 (`-std=c99`) on GCC and clang

## Quick Start

1. Add `forge` to your project (e.g., as a Git submodule or a header in your include path).
2. Create a `forge.c` build script in your root directory.
3. Define `FORGE_IMPL` in that single file before including the header:

```c
#define FORGE_IMPL
#include "forge/forge.h"

int main(int argc, char **argv)
{
    // configure targets, source files, and build steps here
    ...    
    return 0;
}.
