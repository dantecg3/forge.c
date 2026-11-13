#define MEC_IMPL
#include "mec.h"

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

int main (void)
{
  mec_init_context ();
  log_add_sink (&s_context.logger, colored_stdout_sink, NULL);

  $info ("Hello, World!");

  return 0;
}
