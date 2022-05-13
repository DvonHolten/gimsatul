#ifndef _watches_h_INCLUDED
#define _watches_h_INCLUDED

#include <stdbool.h>

struct clause;

struct watch
{
  unsigned short used;
  unsigned char glue;
  bool garbage:1;
  bool reason:1;
  bool redundant:1;
  unsigned middle;
  unsigned sum;
  struct clause *clause;
};

struct watches
{
  struct watch **begin, **end, **allocated;
};

struct references
{
  struct watch **begin, **end, **allocated;
  unsigned *binaries;
};

#define all_watches(ELEM,WATCHES) \
  all_pointers_on_stack (struct watch, ELEM, WATCHES)

struct ring;

void release_references (struct ring *);
void disconnect_references (struct ring *, struct watches *);
void reconnect_watches (struct ring *, struct watches *saved);

#endif