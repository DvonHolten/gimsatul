#include "clause.h"
#include "stack.h"
#include "tagging.h"
#include "trace.h"
#include "utilities.h"

void
mark_clause (signed char * marks, struct clause * clause, unsigned except)
{
  if (binary_pointer (clause))
    mark_literal (marks, other_pointer (clause));
  else
    for (all_literals_in_clause (other, clause))
      if (other != except)
	mark_literal (marks, other);
}

void
unmark_clause (signed char * marks, struct clause * clause, unsigned except)
{
  if (binary_pointer (clause))
    unmark_literal (marks, other_pointer (clause));
  else
    for (all_literals_in_clause (other, clause))
      if (other != except)
	unmark_literal (marks, other);
}

void
really_trace_add_clause (struct buffer *buffer, struct clause *clause)
{
  really_trace_add_literals (buffer, clause->size, clause->literals, INVALID);
}

void
really_trace_delete_clause (struct buffer *buffer, struct clause *clause)
{
  if (!clause->garbage)
    really_trace_delete_literals (buffer, clause->size, clause->literals);
}
