#ifndef FORGE_H
#define FORGE_H

#include "mec/base.h"
#include "mec/mem.h"
#include "mec/dynarr.h"
#include "mec/str.h"
#include "mec/fs.h"

#include <stdlib.h>

typedef void *FORGE;
typedef STR_Buffer FORGE_Target;

int forge_run_command (STR_View command);

FORGE forge_new (MEM_Allocator alloc);
void forge_free (FORGE *p_forge);
FORGE_Target *forge_add_target (FORGE forge, STR_View program);
void forge (FORGE forge);
void forge_target_add_argument (FORGE_Target *p_target, STR_View arg);
bool forge_is_stale (FS_Path output, const FS_Path_List source);

#ifdef FORGE_IMPL
typedef struct FORGE_Header
{
  FORGE_Target *targets;
  MEM_Allocator alloc;
} FORGE_Header;

int forge_run_command (STR_View command)
{
  $mem_scope_init (64 * MEM_KB, &s_context.temp_allocator);
  STR_C command_c = str_c_new (command, s_context.temp_allocator);
  $debug ("forge running '"SV_FMT"'", SV_ARG (command));
  return system (command_c);
}

FORGE forge_new (MEM_Allocator alloc)
{
  FORGE_Header *header = $mem_new (FORGE_Header, alloc);
  if (header == NULL)
    {
      $error_stack_push (error (ERROR_OUT_OF_MEMORY, "failed to allocate forge header"));
      return NULL;
    }

  FORGE_Target *targets = $dynarr_new (FORGE_Target, 1, alloc);
  if (targets == NULL)
    {
      $error_stack_push (error (ERROR_OUT_OF_MEMORY, "failed to allocate forge targets"));
      $mem_free (&header, alloc);
      return NULL;
    }

  header->targets = targets;
  header->alloc = alloc;

  return (FORGE)header;
}

void forge_free (FORGE *p_forge)
{
  $assert (p_forge != NULL && *p_forge != NULL, "");
  FORGE_Header *header = (FORGE_Header *)(*p_forge);
  MEM_Allocator alloc = header->alloc;

  for (usize i = 0; i < $dynarr_length (header->targets); ++i)
    str_free (&header->targets[i]);

  $dynarr_free (&header->targets);
  $mem_free (p_forge, alloc);
}

FORGE_Target *forge_add_target (FORGE forge, STR_View program)
{
  $assert (forge != NULL, "");

  FORGE_Header *header = (FORGE_Header *)forge;
  
  FORGE_Target target = str_new (program, header->alloc);
  if (target == NULL)
    {
      $error_stack_push (error (ERROR_OUT_OF_MEMORY, "failed to allocate forge target"));
      return NULL;
    }

  $dynarr_append (&header->targets, &target);
  return &header->targets[$dynarr_length (header->targets) - 1];
}

void forge (FORGE forge)
{
  $assert (forge != NULL, "");

  FORGE_Header *header = (FORGE_Header *)forge;
  for (usize i = 0; i < $dynarr_length (header->targets); ++i)
    {
      STR_View command = str_view_from_buffer (header->targets[i]);
      int $unused status = forge_run_command (command);
    }
}

void forge_target_add_argument (FORGE_Target *p_target, STR_View arg)
{
  str_append (p_target, str_view (" "));
  str_append (p_target, arg);
}

bool forge_is_stale (FS_Path output, const FS_Path_List source)
{
  uint64 out_time = 0;
  {
    $mem_scope_init (1 * MEM_KB, &s_context.temp_allocator);
    STR_C output_c = str_c_new (output, s_context.temp_allocator);
    out_time       = sys_file_mtime (output_c);
  }

  if (out_time == 0)
    return true;

  if (source == NULL)
    return false;

  for (usize i = 0; i < $dynarr_length (source); ++i)
    {
      $mem_scope_init (1 * MEM_KB, &s_context.temp_allocator);
      STR_C source_c = str_c_new (fs_path_from_buffer (source[i]), s_context.temp_allocator);

      if (sys_file_mtime (source_c) > out_time)
        return true;
    }
  return false;
}
#endif // FORGE_IMPL
#endif // FORGE_H
