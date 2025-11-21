#ifndef GLOBALS_H
#define GLOBALS_H

#include <ncurses.h>
#include <stdbool.h>

#define MAX_PROCESSES 6
#define CELL_WIDTH 3
#define MAX_PAGES_PER_PROCESS 10
#define TOTAL_RAM_FRAMES 50
#define FRAME_SIZE_KB 4
#define TOTAL_RAM_KB 200

// Process states
typedef enum {
    NOT_ARRIVED = 0,
    EXECUTING = 1,
    OVERHEAD = 2,
    WAITING = 3,
    COMPLETED = 4,
    DEADLINE_MISSED = 5,
    PAGE_FAULT = 6
} ProcessState;

// Memory replacement policies
typedef enum {
    POLICY_FIFO = 0,
    POLICY_LRU = 1
} ReplacementPolicy;

// Frame structure (RAM)
typedef struct {
    int process_id;      // -1 if free
    int page_number;     // which page of the process
    int load_time;       // for FIFO
    int last_access;     // for LRU
} Frame;

// Page structure
typedef struct {
    bool in_ram;         // true if in RAM, false if in disk
    int frame_index;     // -1 if not in RAM
} Page;

// Final status of a process (result after simulation)
typedef enum {
    PS_PENDING = 0,
    PS_COMPLETED_ON_TIME = 1,
    PS_DEADLINE_MISSED = 2
} ProcessStatus;

// Indexes for the metrics array stored per process
typedef enum {
    MI_ARRIVAL = 0,
    MI_EXECUTION = 1,
    MI_DEADLINE = 2,
    MI_PRIORITY = 3,
    MI_START = 4,
    MI_END = 5,
    MI_WAIT = 6,
    MI_TURNAROUND = 7,
    MI_DEADLINE_OK = 8,
    MI_PAGE_FAULTS = 9,
    MI_COUNT = 10
} MetricsIndex;

// Summary statistics structure
typedef struct {
    double avg_arrival;
    double avg_execution;
    double avg_wait;
    double avg_turnaround;
    double throughput;        // processes completed per unit time
    double idle_percentage;   // % of time CPU was idle
    int context_switches;     // total number of context switches
} SummaryStats;

// Process structure
typedef struct {
    int id;
    int arrival_time;
    int execution_time;
    int remaining_time;
    int priority;
    int deadline;
    int num_pages;  // Number of pages (size) - user configurable
    bool overhead;
    double vruntime;  // Virtual runtime for CFS algorithm
    ProcessState *timeline;
    bool *page_fault_occurred;  // tracks if page fault happened at each time unit (for visual overlay)
    // Metrics array (9 positions): arrival, execution, deadline, priority,
    // start, end, wait, turnaround, deadline_ok?
    int metrics[MI_COUNT];
    // Final status after simulation (completed on time, missed, etc.)
    ProcessStatus final_status;
    
    // Memory management
    Page pages[MAX_PAGES_PER_PROCESS];
    int page_faults;
    int page_fault_remaining;  // time remaining blocked by page fault
    int next_page_to_access;   // for sequential page access
    int exec_units_since_page_access;  // counter for page access frequency
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

// Memory configuration
extern bool memory_enabled;
extern ReplacementPolicy replacement_policy;
extern Frame ram_frames[TOTAL_RAM_FRAMES];
extern int current_time_global;

// Memory history for animation (stores state at each time unit)
#define MAX_HISTORY_SIZE 500
typedef struct {
    int process_id;
    int page_number;
} FrameSnapshot;

extern FrameSnapshot ram_history[MAX_HISTORY_SIZE][TOTAL_RAM_FRAMES];
extern bool history_initialized;

// Memory visualization
extern int memory_animation_frame;  // current frame being displayed

// Screen and interface
extern int screen_height;
extern int screen_width;
extern bool screen_resized;

// Scroll
extern int time_offset;
extern int visible_columns;

// Controls and state
extern bool simulation_running;
extern int animation_speed;
// Metrics ready flag
extern bool metrics_computed;
// Summary statistics
extern SummaryStats summary_stats;

// Global functions
void update_screen_size();
bool is_screen_too_small();
void initialize_globals();

#endif
