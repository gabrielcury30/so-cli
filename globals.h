#ifndef GLOBALS_H
#define GLOBALS_H

#include <ncurses.h>
#include <stdbool.h>

#define MAX_PROCESSES 8
#define CELL_WIDTH 3

// Process states
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
// Processes and algorithms
extern int TOTAL_TIME;
extern Process processes[MAX_PROCESSES];
extern int num_processes;
extern int current_time;
extern int current_algorithm;
extern const char *algorithm_names[];

// Scheduler configuration
extern int quantum;
extern int overhead_time;

// Screen and interface
extern int screen_height;
extern int screen_width;
extern bool screen_resized;

// Controls and state
extern bool simulation_running;
extern int animation_speed;

// Global functions
void update_screen_size();
bool is_screen_too_small();
void initialize_globals();

#endif
