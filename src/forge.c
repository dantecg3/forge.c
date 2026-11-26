#define MEC_IMPL
#define MEC_NO_EMBED
#define FORGE_IMPL
#include "mec/mec.h"
#include "forge/forge.h"

#define SOURCE_DIRS_COUNT 3

static const char *LEVEL_COLORS[] = {
  "\x1b[90m",   // TRACE: gray
  "\x1b[36m",   // DEBUG: cyan
  "\x1b[32m",   // INFO:  green
  "\x1b[33m",   // WARN:  yellow
  "\x1b[31m",   // ERROR: red
  "\x1b[1;41m", // FATAL: bold on red background
};
#define ANSI_RESET "\x1b[0m"

static void colored_stdout_sink (void $unused *context, LOG_Level level, const char *msg)
{
  printf ("%s%s" ANSI_RESET "\n", LEVEL_COLORS[level], msg);
}

static inline void error_stack_sink (ERROR err)
{
  $error (ERR_FMT " %s", err.code, err.msg);
}

static bool is_c_source (FS_Path path)
{
  STR_View ext = fs_path_extension (path);
  return str_view_equal (ext, str_view (".c")) || str_view_equal (ext, str_view (".h"));
}

int main (int $unused argc, char $unused **argv)
{
  mec_init_context (error_stack_sink);
  log_add_sink (&s_context.logger, colored_stdout_sink, NULL);

  forge_run_command (str_view ("mkdir -p bin"));

  FS_Path source_dirs[SOURCE_DIRS_COUNT] = {
    fs_path ("src"),
    fs_path ("incl"),
    fs_path ("vendor/mec/incl"),
  };
  FS_Path output = fs_path ("bin/test");

  FS_Find_Options opt = { 0 };
  $bitset_set (opt.flags, FS_FIND_FILES);
  $bitset_set (opt.flags, FS_FIND_RECURSIVE);
  opt.filter_proc = is_c_source;

  bool stale = false;
  for (usize i = 0; i < SOURCE_DIRS_COUNT && !stale; ++i)
    {
      FS_Path_List sources = fs_find (source_dirs[i], opt, s_context.default_allocator);
      stale                = forge_is_stale (output, sources);

      if (sources != NULL)
        fs_find_list_free (&sources);
    }

  if (!stale)
    {
      $debug ("bin/test is up to date");
      return 0;
    }

  FORGE f = forge_new (s_context.default_allocator);
  if (f == NULL)
    {
      $error ("failed to create forge");
      return -1;
    }

  FORGE_Target *build = forge_add_target (f, str_view ("cc"));
  if (build == NULL)
    {
      $error ("failed to create build");
      forge_free (&f);
      return -1;
    }

  forge_target_add_argument (build, str_view ("-std=c99 -Wall -Wextra -pedantic"));
  forge_target_add_argument (build, str_view ("-Iincl -Ivendor/mec/incl"));
  forge_target_add_argument (build, str_view ("src/test.c"));
  forge_target_add_argument (build, str_view ("-o bin/test"));

  forge (f);
  forge_free (&f);

  return 0;
}
