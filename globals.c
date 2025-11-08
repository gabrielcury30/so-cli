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

// Controls and state
bool simulation_running = false;
int animation_speed = 100; // ms

// Global functions implementation
void update_screen_size() {
    getmaxyx(stdscr, screen_height, screen_width);

    // Get TOTAL_TIME based on screen width
    int available_width = screen_width - 40; // Space for labels and legend
    TOTAL_TIME = available_width / CELL_WIDTH;

    // Check if screen is too small
    if (TOTAL_TIME < 15) {
        TOTAL_TIME = 15; // Minimum value to avoid crashes
    }

    // Maximum limit
    if (TOTAL_TIME > 70) TOTAL_TIME = 70;
}

bool is_screen_too_small() {
    return (screen_width < 90 || screen_height < 30 || TOTAL_TIME < 15);
}

void initialize_globals() {
    // Start default values
    TOTAL_TIME = 20;
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
    }
}
