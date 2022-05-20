#include "definition.h"
#include "macros.h"
#include "ruler.h"

static bool
find_binary_and_gate_clauses (struct ruler * ruler,
                              unsigned lit, struct clause * clause,
			      struct clauses * gate, struct clauses * nogate)
{
  assert (!clause->garbage);
  if (clause->size > CLAUSE_SIZE_LIMIT)
    return false;
  CLEAR (*gate);
  CLEAR (*nogate);
  signed char * marks = ruler->simplifier.marks;
  for (all_literals_in_clause (other, clause))
    if (other != lit)
      marks[other] = 1;
  unsigned not_lit = NOT (lit);
  struct clauses * not_lit_clauses = &OCCURRENCES (not_lit);
  unsigned marked = 0;
  for (all_clauses (not_lit_clause, *not_lit_clauses))
    if (binary_pointer (not_lit_clause))
      {
	unsigned other = other_pointer (not_lit_clause);
	unsigned not_other = NOT (other);
	if (marks[not_other])
	  {
	    PUSH (*gate, not_lit_clause);
	    marks[not_other] = 0;
	    marked++;
	  }
	else
	  PUSH (*nogate, not_lit_clause);
      }
    else
      PUSH (*nogate, not_lit_clause);
  for (all_literals_in_clause (other, clause))
    if (other != lit)
      marks[other] = 0;
  assert (marked < clause->size);
  return marked + 1 == clause->size;
}

static struct clause *
find_and_gate (struct ruler * ruler, unsigned lit,
               struct clauses * gate, struct clauses * nogate)
{
  for (all_clauses (clause, OCCURRENCES (lit)))
    if (!binary_pointer (clause))
      if (find_binary_and_gate_clauses (ruler, lit, clause,
	                                    gate, nogate))
	return clause;
  return 0;
}

static unsigned
find_equivalence_gate (struct ruler * ruler, unsigned lit)
{
  signed char * marks = ruler->simplifier.marks;
  for (all_clauses (clause, OCCURRENCES (lit)))
    if (binary_pointer (clause))
      marks[other_pointer (clause)] = 1;
  unsigned not_lit = NOT (lit);
  unsigned res = INVALID;
  for (all_clauses (clause, OCCURRENCES (not_lit)))
    if (binary_pointer (clause))
      {
	unsigned other = other_pointer (clause);
	unsigned not_other = NOT (other);
	if (marks[not_other])
	  {
	    res = other;
	    break;
	  }
      }
  for (all_clauses (clause, OCCURRENCES (lit)))
    if (binary_pointer (clause))
      marks[other_pointer (clause)] = 0;
  return res;
}

bool
find_definition (struct ruler * ruler, unsigned lit)
{
  struct clauses * gate = ruler->simplifier.gate;
  struct clauses * nogate = ruler->simplifier.nogate;
  {
    unsigned other = find_equivalence_gate (ruler, lit);
    if (other != INVALID)
      {
	ROG ("found equivalence %s equal to %s", ROGLIT (lit), ROGLIT (other));
	{
	  CLEAR (gate[0]);
	  CLEAR (nogate[0]);
	  unsigned not_other = NOT (other);
	  struct clause * lit_clause = tag_pointer (false, lit, not_other);
	  bool found = false;
	  PUSH (gate[0], lit_clause);
	  for (all_clauses (clause, OCCURRENCES (lit)))
	    if (clause == lit_clause)
	      found = true;
	    else
	      PUSH (nogate[0], clause);
	  assert (found), (void) found;
	}
	{
	  CLEAR (gate[1]);
	  CLEAR (nogate[1]);
	  unsigned not_lit = NOT (lit);
	  struct clause * not_lit_clause = tag_pointer (false, not_lit, other);
	  bool found = false;
	  PUSH (gate[1], not_lit_clause);
	  for (all_clauses (clause, OCCURRENCES (not_lit)))
	    if (clause == not_lit_clause)
	      found = true;
	    else
	      PUSH (nogate[1], clause);
	  assert (found), (void) found;
	}
	return true;
      }
  }
  unsigned resolve = lit;
  struct clause * base = find_and_gate (ruler, resolve, &gate[1], &nogate[1]);
  if (base)
    {
      assert (SIZE (gate[1]) == base->size - 1);
      CLEAR (gate[0]);
      CLEAR (nogate[0]);
      PUSH (gate[0], base);
      for (all_clauses (clause, OCCURRENCES (resolve)))
	if (clause != base)
	  PUSH (nogate[0], clause);
    }
  else
    {
      resolve = NOT (lit);
      base = find_and_gate (ruler, resolve, &gate[0], &nogate[0]);
      if (base)
	{
	  assert (SIZE (gate[0]) == base->size - 1);
	  CLEAR (gate[1]);
	  CLEAR (nogate[1]);
	  PUSH (gate[1], base);
	  for (all_clauses (clause, OCCURRENCES (resolve)))
	    if (clause != base)
	      PUSH (nogate[1], clause);
	}
    }
  if (!base)
    return false;
#ifdef LOGGING
  do
    {
      ROGPREFIX ("found %u-ary and-gate with %s defined as ",
                 base->size - 1, ROGLIT (resolve));
      bool first = true;
      for (all_literals_in_clause (other, base))
	{
	  if (other == resolve)
	    continue;
	  if (first)
	    first = false;
	  else
	    fputs (" & ", stdout);
	  unsigned not_other = NOT (other);
	  fputs (ROGLIT (not_other), stdout);
	}
      ROGSUFFIX ();
    }
  while (0);
#endif
  return true;
}

