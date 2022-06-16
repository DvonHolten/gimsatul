#ifndef _statistics_h_INCLUDED
#define _statistics_h_INCLUDED

#include <stdint.h>

/*------------------------------------------------------------------------*/

struct ring;
struct ruler;

/*------------------------------------------------------------------------*/

#define SIZE_VISITS 7

struct context
{
  uint64_t ticks;
  uint64_t propagations;
  uint64_t conflicts;
  uint64_t decisions;
#ifdef METRICS
  uint64_t visits[SIZE_VISITS];
#endif
};

#define SEARCH_CONTEXT 0
#define PROBING_CONTEXT 1
#define WALK_CONTEXT 2
#define SIZE_CONTEXTS 3

struct ring_statistics
{
  uint64_t flips;
  uint64_t probings;
  uint64_t reductions;
  uint64_t rephased;
  uint64_t restarts;
  uint64_t simplifications;
  uint64_t switched;
  uint64_t walked;

  struct context contexts[SIZE_CONTEXTS];

  struct
  {
    uint64_t learned;
#ifdef METRICS
    uint64_t deduced;
    uint64_t minimized;
    uint64_t shrunken;
#endif
  } literals;

  unsigned active;
  unsigned failed;
  unsigned fixed;
  unsigned lifted;

  size_t irredundant;
  size_t redundant;

  struct
  {
    uint64_t strengthened;
    uint64_t succeeded;
    uint64_t implied;
  } vivify;

  struct
  {
    uint64_t clauses;
#ifdef METRICS
    uint64_t units;
    uint64_t binary;
    uint64_t glue1;
    uint64_t tier1;
    uint64_t tier2;
    uint64_t tier3;
#endif
  } learned;

  struct
  {
    uint64_t units;
    uint64_t binary;
    uint64_t clauses;
    uint64_t glue1;
    uint64_t tier1;
    uint64_t tier2;
  } exported, imported;
};

#define SEARCH_CONFLICTS \
  ring->statistics.contexts[SEARCH_CONTEXT].conflicts

#define SEARCH_TICKS \
  ring->statistics.contexts[SEARCH_CONTEXT].ticks

#define PROBING_TICKS \
  ring->statistics.contexts[PROBING_CONTEXT].ticks

struct ruler_statistics
{
  uint64_t garbage;
  uint64_t binaries;
  unsigned active;
  unsigned original;
  uint64_t deduplicated;
  unsigned eliminated;
  unsigned definitions;
  uint64_t strengthened;
  uint64_t subsumed;
  uint64_t substituted;
  uint64_t selfsubsumed;
  uint64_t simplifications;
  size_t weakened;
  struct
  {
    uint64_t elimination;
    uint64_t subsumption;
  } ticks;
  struct
  {
    unsigned simplifying;
    unsigned solving;
    unsigned total;
  } fixed;
};

/*------------------------------------------------------------------------*/
#ifndef QUIET

void print_ring_statistics (struct ring *);
void print_ruler_statistics (struct ruler *);

#endif
/*------------------------------------------------------------------------*/

#endif
