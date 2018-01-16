#include "print_process_stats.h"
#include "stdio.h"

void printPrefix(int depth, int child_mask) {
    for (int i = 0; i <= depth; i += 1) {
        for (int j = 1; j < INDENT_LEVEL / 2; j += 1) {
            printf(" ");
        }

        if ((1 << i) & child_mask) {
            printf("║");
        } else {
            printf(" ");
        }

        for (int j = INDENT_LEVEL / 2; j < INDENT_LEVEL; j += 1) {
            printf(" ");
        }
    }
}

void printProcessStatsInternalInformation(struct ProcessStats* process, int depth, int child_mask) {
    printPrefix(depth, child_mask);
    printf("wall  %.3f\n", process->wall_time_ms * 0.001);

    printPrefix(depth, child_mask);
    printf("cpu   %.3f\n", (process->user_time_ms + process->system_time_ms) * 0.001);

    printPrefix(depth, child_mask);
    printf("sys   %.3f\n", process->system_time_ms * 0.001);
}

void printProcessStatsInternal(struct ProcessStats* process, int depth, int child_mask) {
    // print prefix for process
    if (process->children[0] != NULL) {
        child_mask |= (1 << depth);
        if (depth == 0) {
            printf("╔");
        } else {
            printf("╦"); // ╦
        }
    } else {
        printf("═");
    }

    for (int j = INDENT_LEVEL / 2; j < INDENT_LEVEL - 1; j += 1) {
        printf("═");
    }
    printf(" ");
    printf("pid   %d\n", process->pid);

    // print data about the process
    printProcessStatsInternalInformation(process, depth, child_mask);

    // calling on children with the cool-looking border UI
    struct ProcessStats** next_process = process->children;
    while (*next_process != NULL) {
        int next_mask = child_mask;
        int last_child = (*(next_process + 1)) == NULL;

        printPrefix(depth - 1, child_mask);

        for (int j = 1; j < INDENT_LEVEL / 2; j += 1) {
            printf(" ");
        }

        if (last_child) {
            printf("╚");
        } else {
            printf("╠");
        }

        for (int j = INDENT_LEVEL / 2; j < INDENT_LEVEL; j += 1) {
            printf("═");
        }

        for (int j = 1; j < INDENT_LEVEL / 2; j += 1) {
            printf("═");
        }

        if (last_child) {
            next_mask ^= (1 << depth);
        }

        printProcessStatsInternal(*next_process, depth + 1, next_mask);
        next_process++;
    }
}

