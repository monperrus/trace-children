#ifndef TRACE_CHILDREN_H
#define TRACE_CHILDREN_H

#include <time.h>
#include <sys/types.h>

#define INDENT_LEVEL 4

struct ProcessStats {
    pid_t pid;
    double wall_time_ms;
    double user_time_ms;
    double system_time_ms;

    struct timespec last_update_time;

    struct ProcessStats** children;
};

void printProcessStats(struct ProcessStats* process);

void printProcessStatsPID(pid_t pid);

void dealloc(struct ProcessStats* process);

#ifndef tracechildrenuselib
#include "tracechildren_src/trace_children.c"
#endif

#endif // TRACE_CHILDREN_H
