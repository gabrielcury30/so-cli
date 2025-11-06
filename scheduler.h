#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <ncurses.h>

#define MAX_PROCESSES 10
#define TOTAL_TIME 20
#define CELL_WIDTH 3

typedef enum {
    NOT_ARRIVED = 0,
    EXECUTING = 1,
    OVERHEAD = 2,
    WAITING = 3,
    COMPLETED = 4
} ProcessState;

// Process structure
typedef struct {
    int id;
    int arrival_time;
    int execution_time;
    int remaining_time;
    int priority;
    int deadline;
    bool overhead;
    ProcessState *timeline;
} Process;

// Global variables
extern Process processes[MAX_PROCESSES];
extern int num_processes;
extern int current_time;
extern int current_algorithm;
extern const char *algorithm_names[];

void initialize_processes();
void execute_fifo();
void execute_sjf();
void execute_edf();
void execute_rr();
void run_current_algorithm();
void reset_simulation();

#endif
