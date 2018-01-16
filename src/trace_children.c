#include "trace_children.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "print_process_stats.h"
#include "utils.h"

void dealloc(struct ProcessStats* process) {
    struct ProcessStats** children = process->children;
    while (*children != NULL) {
        dealloc(*children);
        children += 1;
    }

    free(process);
}

void printProcessStats(struct ProcessStats* process) {
    // compute percentage data
    xsleepms(10); // give it a little time to settle in
    updateProcessStatsRecursive(process);

    for (int j = 1; j < INDENT_LEVEL / 2; j += 1) {
        printf(" ");
    }
    printProcessStatsInternal(process, 0, 1);
}

struct ProcessStats* getProcessStats(pid_t pid);

void printProcessStatsPID(pid_t pid) {
    struct ProcessStats* process = getProcessStats(pid);
    printProcessStats(process);
    dealloc(process);
}

void updateProcessStats(struct ProcessStats* process) {
    // read data from /proc/$/stat

    // compute time just after reading data so we know the correct time difference between 2 reads
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);

    process->wall_time_ms = 0;
    process->system_time_ms = 0;
    process->user_time_ms = 0;

    if (process->last_update_time.tv_sec != 0) {
        // not the first time we update the values
        // we can compute the %values (% cpu usage)
        // if it worked 10 cpu secs and the last update was 15 secs ago, process has 66% of the CPU
        
        // how many nano seconds passed since last update
        double nsec_elapsed = (current_time.tv_sec - process->last_update_time.tv_sec) * 1e9 + (current_time.tv_nsec - process->last_update_time.tv_nsec);
        // 1 sec = 1e9 nano_sec
    }

    // move data into process (like wall time, etc)

    process->last_update_time = current_time;
}

void updateProcessStatsRecursive(struct ProcessStats* process) {
    updateProcessStats(process);
    struct ProcessStats** children = process->children;
    while (*children != NULL) {
        updateProcessStatsRecursive(*children);
        children++;
    }
}

struct ProcessStats* getProcessStats(pid_t pid) {
    // read children pids from /proc/$(pid)/task/$(pid)/children
    char* pid_str = to_char(pid);
    int pid_size = strlen(pid_str);

    // compute file to read
    int total_size = 21 + 2 * pid_size;
    static char children_path[1024];
    children_path[total_size] = '\0';
    strcpy(children_path, "/proc/");
    strcpy(children_path + 6, pid_str);
    strcpy(children_path + 6 + pid_size, "/task/");
    strcpy(children_path + 6 + pid_size + 6, pid_str);
    strcpy(children_path + 6 + pid_size + 6 + pid_size, "/children");
   
    // opening file
    int fd = open(children_path, O_RDONLY);
    if (fd == -1) {
        perror(NULL);
        exit(errno);
    }

    // reading file
    int num_children = 0;
    static char buff[1024];
    int rc = xread(fd, buff, 1023);
    if (rc < 0) {
        perror(NULL);
        exit(errno);
    }

    buff[rc + 1] = '\0';

    // determining number of children based on the number of spaces
    char* c = buff;
    while (*c != '\0') {
        if (*c == ' ') {
            num_children += 1;
            c += 1;
        }
        c += 1;
    }

    // create the process object
    // with some okish default values
    struct ProcessStats* process = (struct ProcessStats*)xmalloc(sizeof(struct ProcessStats));
    process->pid = pid;
    process->last_update_time.tv_nsec = 0;
    process->last_update_time.tv_sec = 0;
    process->children = (struct ProcessStats**)xmalloc((num_children + 1) * sizeof(struct ProcessStats*));

    // getting children pids
    c = buff;
    int* children_pid = (int*)xmalloc((num_children + 1) * (sizeof(int)));
    for (int i = 0; i < num_children; i += 1) {
        int x = 0;
        while (*c != ' ') {
            x *= 10;
            x += *c - '0';
            c += 1;
        }
        c += 1;
        children_pid[i] = x;
    }

    // calling the function recursively for each child-pid
    for (int i = 0; i < num_children; i += 1) {
        process->children[i] = getProcessStats(children_pid[i]);
        if (process->children[i] == NULL) {
            // if somehow a child died durring our read or something, erase it
            i -= 1;
            num_children -= 1;
        }
    }

    // make last child NULL for sentinel
    process->children[num_children] = NULL;

    // erase malloced memory
    free(children_pid);

    // update child info
    updateProcessStats(process);
    return process;
}
