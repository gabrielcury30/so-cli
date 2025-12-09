#include "../../include/globals.h"
#include <stdlib.h>

// Global variables definition
// Process and algorithms
int TOTAL_TIME = 20;
Process processes[MAX_PROCESSES];
int num_processes = 0;
int current_time = 0;
int current_algorithm = 0;
const char *algorithm_names[] = {"FIFO", "SJF", "EDF", "Round Robin", "CFS"};

// Scheduler configuration
int quantum = 2;
int overhead_time = 1;

// Memory configuration
bool memory_enabled = true;
ReplacementPolicy replacement_policy = POLICY_FIFO;
Frame ram_frames[TOTAL_RAM_FRAMES];
int current_time_global = 0;

// Memory history for animation
FrameSnapshot ram_history[MAX_HISTORY_SIZE][TOTAL_RAM_FRAMES];
bool history_initialized = false;

// Memory visualization
int memory_animation_frame = 0;

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

    // Initialize memory
    memory_enabled = false;
    replacement_policy = POLICY_FIFO;
    current_time_global = 0;
    memory_animation_frame = 0;

    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        ram_frames[i].process_id = -1;
        ram_frames[i].page_number = -1;
        ram_frames[i].load_time = 0;
        ram_frames[i].last_access = 0;
    }

    // Start process arrays
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].id = 0;
        processes[i].arrival_time = 0;
        processes[i].execution_time = 0;
        processes[i].remaining_time = 0;
        processes[i].priority = 0;
        processes[i].deadline = 0;
        processes[i].num_pages = 3;  // Default 3 pages
        processes[i].overhead = false;
        processes[i].vruntime = 0.0;
        processes[i].timeline = NULL;
        processes[i].final_status = PS_PENDING;
        processes[i].num_pages = 0;
        processes[i].page_faults = 0;
        processes[i].page_fault_remaining = 0;
        processes[i].next_page_to_access = 0;
        processes[i].exec_units_since_page_access = 0;

        for (int p = 0; p < MAX_PAGES_PER_PROCESS; p++) {
            processes[i].pages[p].in_ram = false;
            processes[i].pages[p].frame_index = -1;
        }

        for (int m = 0; m < MI_COUNT; m++) {
            processes[i].metrics[m] = 0;
        }
    }
}
