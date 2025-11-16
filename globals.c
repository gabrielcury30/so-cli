#include "globals.h"
#include <stdlib.h>

// Global variables definition
// Process and algorithms
int TOTAL_TIME = 20;
Process processes[MAX_PROCESSES];
int num_processes = 0;
int current_time = 0;
int current_algorithm = 0;
const char *algorithm_names[] = {"FIFO", "SJF", "EDF", "Round Robin"};

// Scheduler configuration
int quantum = 2;
int overhead_time = 1;

// Screen and interface
int screen_height = 0;
int screen_width = 0;
bool screen_resized = false;

int time_offset = 0;
int visible_columns = 21;

// Controls and state
bool simulation_running = false;
int animation_speed = 100; // ms
bool metrics_computed = false;
SummaryStats summary_stats = {0};

// Global functions implementation
void initialize_globals() {
    // Start default values
    TOTAL_TIME = 100;
    num_processes = 0;
    current_time = 0;
    current_algorithm = 0;
    quantum = 2;
    overhead_time = 1;
    simulation_running = false;
    animation_speed = 100;

    // Start process arrays
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].id = 0;
        processes[i].arrival_time = 0;
        processes[i].execution_time = 0;
        processes[i].remaining_time = 0;
        processes[i].priority = 0;
        processes[i].deadline = 0;
        processes[i].overhead = false;
        processes[i].timeline = NULL;
        processes[i].final_status = PS_PENDING;
        for (int m = 0; m < MI_COUNT; m++) {
            processes[i].metrics[m] = 0;
        }
    }
}
