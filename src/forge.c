#define MEC_IMPL
#define FORGE_IMPL
#include "mec/mec.h"
#include "forge/forge.h"

static const char *LEVEL_COLORS[] = {
  "\x1b[90m",   // TRACE: gray
  "\x1b[36m",   // DEBUG: cyan
  "\x1b[32m",   // INFO:  green
  "\x1b[33m",   // WARN:  yellow
  "\x1b[31m",   // ERROR: red
  "\x1b[1;41m", // FATAL: bold on red background
};
#define ANSI_RESET "\x1b[0m"

static void colored_stdout_sink (void $unused *context , LOG_Level level, const char *msg)
{
  printf ("%s%s" ANSI_RESET "\n", LEVEL_COLORS[level], msg);
}

int main (int argc, char **argv)
{
  mec_init_context ();
  log_add_sink (&s_context.logger, colored_stdout_sink, NULL);

  FORGE f = forge_new (s_context.default_allocator);
  if (f == NULL)
    {
      $error ("failed to create forge");
      return -1;
    }
  
  forge_run_command (str_view ("mkdir -p bin"));

  FORGE_Target *build = forge_add_target (f, str_view ("cc"));
  if (build == NULL)
    {
      $error ("failed to create build");
      return -1;
    }

  forge_target_add_argument (build, str_view ("-std=c99 -Wall -Wextra -pedantic"));
  forge_target_add_argument (build, str_view ("-Iincl -Ivendor/mec/incl"));
  forge_target_add_argument (build, str_view ("src/test.c"));
  forge_target_add_argument (build, str_view ("-o bin/test"));

  forge (f);

  return 0;
}
