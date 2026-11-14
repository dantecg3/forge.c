# Forge
Tool for building C projects, with a C program.

Targets C99 (`-std=c99`) on GCC and clang
## Dependencies

Forge depends on [Modern Engine Core (`mec.c`)](https://github.com/dantecg3/mec.c).

## Quick Start

```c
#define MEC_IMPL
#include "mec/mec.h"
#define FORGE_IMPL
#include "forge/forge.h"

int main (int $unused argc, char $unused **argv)
{
    mec_init_context ();

    forge_run_command (str_view ("mkdir -p bin"));

    FORGE f = forge_new (s_context.default_allocator);
    if (f == NULL)
    {
        $error ("failed to create forge context");
        return -1;
    }

    FORGE_Target build = forge_add_target (f, str_view ("cc"));
    if (build == NULL)
    {
        $error ("failed to add target");
        forge_free (&f);
        return -1;
    }

    forge_target_add_argument (&build, str_view (" -std=c99 -Wall -Wextra -pedantic"));
    forge_target_add_argument (&build, str_view (" -Iincl -Ivendor/mec/incl"));
    forge_target_add_argument (&build, str_view (" src/test.c"));
    forge_target_add_argument (&build, str_view (" -o bin/test"));

    forge (f);

    forge_free (&f);
    return 0;
}
