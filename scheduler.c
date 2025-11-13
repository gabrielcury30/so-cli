#include "scheduler.h"
#include "globals.h"
#include <stdlib.h>

void initialize_default_processes() {
    // Example process 1
    processes[0].id = 1;
    processes[0].arrival_time = 0;
    processes[0].execution_time = 5;
    processes[0].remaining_time = processes[0].execution_time;
    processes[0].priority = 1;
    processes[0].deadline = 10;

    // Example process 2
    processes[1].id = 2;
    processes[1].arrival_time = 2;
    processes[1].execution_time = 3;
    processes[1].remaining_time = processes[1].execution_time;
    processes[1].priority = 2;
    processes[1].deadline = 8;

    // Example process 3
    processes[2].id = 3;
    processes[2].arrival_time = 4;
    processes[2].execution_time = 4;
    processes[2].remaining_time = processes[2].execution_time;
    processes[2].priority = 3;
    processes[2].deadline = 15;

    // Example process 4
    processes[3].id = 4;
    processes[3].arrival_time = 6;
    processes[3].execution_time = 6;
    processes[3].remaining_time = processes[3].execution_time;
    processes[3].priority = 5;
    processes[3].deadline = 21;

    // Example process 5
    processes[4].id = 5;
    processes[4].arrival_time = 10;
    processes[4].execution_time = 2;
    processes[4].remaining_time = processes[4].execution_time;
    processes[4].priority = 4;
    processes[4].deadline = 18;

    // Example process 6
    processes[5].id = 6;
    processes[5].arrival_time = 12;
    processes[5].execution_time = 12;
    processes[5].remaining_time = processes[5].execution_time;
    processes[5].priority = 6;
    processes[5].deadline = 35;

    num_processes = 6;

    // Allocate and initialize timeline arrays
    for (int i = 0; i < num_processes; i++) {
        processes[i].timeline = malloc(TOTAL_TIME * sizeof(ProcessState));
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }
}

void execute_fifo() {
    int running_process = -1;
    int process_completed = 0;

    for (int t = 0; t < TOTAL_TIME && process_completed < num_processes; t++) {
        // If there are any process executing, find next on queue
        if (running_process == -1) {
            // Choose process that got in earlier and yet no finished
            int earliest_arrival = TOTAL_TIME + 1;
            running_process = -1;

            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time <= t &&
                    processes[i].remaining_time > 0 &&
                    processes[i].arrival_time < earliest_arrival) {
                    earliest_arrival = processes[i].arrival_time;
                    running_process = i;
                }
            }
        }

        // Update states
        for (int i = 0; i < num_processes; i++) {
            if (i == running_process && running_process != -1) {
                processes[i].timeline[t] = EXECUTING;
                processes[i].remaining_time--;

                // Check if process is done
                if (processes[i].remaining_time <= 0) {
                    process_completed++;
                    running_process = -1;  // Free CPU for next process
                }
            } else if (processes[i].arrival_time <= t && processes[i].remaining_time > 0) {
                processes[i].timeline[t] = WAITING;
            } else if (processes[i].arrival_time > t) {
                processes[i].timeline[t] = NOT_ARRIVED;
            } else {
                processes[i].timeline[t] = COMPLETED;
            }
        }
    }
}

void execute_sjf() {
    for (int t = 0; t < TOTAL_TIME; t++) {
        int running_process = -1;
        int shortest_time = TOTAL_TIME + 1;

        // Find process with shortest remaining time
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= t &&
                processes[i].remaining_time > 0 &&
                processes[i].remaining_time < shortest_time) {
                shortest_time = processes[i].remaining_time;
                running_process = i;
            }
        }

        // Update states
        for (int i = 0; i < num_processes; i++) {
            if (i == running_process && running_process != -1) {
                processes[i].timeline[t] = EXECUTING;
                processes[i].remaining_time--;
            } else if (processes[i].arrival_time <= t && processes[i].remaining_time > 0) {
                processes[i].timeline[t] = WAITING;
            } else if (processes[i].arrival_time > t) {
                processes[i].timeline[t] = NOT_ARRIVED;
            } else {
                processes[i].timeline[t] = COMPLETED;
            }
        }
    }
}

void execute_edf() {
    int current_quantum = 0;
    int running_process = -1;
    int overhead_remaining = 0;

    // Reset remaining times and initialize overhead flag
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].execution_time;
        processes[i].overhead = false;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }

    for (int t = 0; t < TOTAL_TIME; t++) {
        // Check if current process finished its quantum or completed
        if (running_process != -1) {
            bool needs_preemption = (current_quantum >= quantum);
            bool has_finished = (processes[running_process].remaining_time <= 0);

            if (needs_preemption || has_finished) {
                // If quantum over and not finishe, add overhead
                if (needs_preemption && processes[running_process].remaining_time > 0) {
                    processes[running_process].overhead = true;
                    overhead_remaining = overhead_time;
                }
                running_process = -1;
                current_quantum = 0;
            }
        }

        // Get next process (EDF selection)
        if (running_process == -1 && overhead_remaining == 0) {
            int earliest_deadline = TOTAL_TIME + 1;
            running_process = -1;

            // Find process with earliest deadline
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time <= t &&
                    processes[i].remaining_time > 0 &&
                    processes[i].deadline < earliest_deadline) {
                    earliest_deadline = processes[i].deadline;
                    running_process = i;
                }
            }
        }

        // Update states
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].overhead) {
                processes[i].timeline[t] = OVERHEAD;

                overhead_remaining--;
                if (overhead_remaining == 0) {
                    processes[i].overhead = false;
                }
            } else if (i == running_process && running_process != -1) {
                if (t - processes[i].arrival_time >= processes[i].deadline) {
                    processes[i].timeline[t] = DEADLINE_MISSED;
                    processes[i].remaining_time--;
                    current_quantum++;
                } else {
                    processes[i].timeline[t] = EXECUTING;
                    processes[i].remaining_time--;
                    current_quantum++;
                }
            } else if (processes[i].arrival_time <= t && processes[i].remaining_time > 0) {
                processes[i].timeline[t] = WAITING;
            } else if (processes[i].arrival_time > t) {
                processes[i].timeline[t] = NOT_ARRIVED;
            } else {
                processes[i].timeline[t] = COMPLETED;
            }
        }
    }
}

void execute_rr() {
    int current_quantum = 0;
    int running_process = -1;
    int process_queue[MAX_PROCESSES];
    int queue_size = 0;
    int overhead_remaining = 0;

    // Initialize queue with processes in arrival order
    for (int t = 0; t < TOTAL_TIME; t++) {

        // Add newly arrived processes to queue
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == t) {
                process_queue[queue_size++] = i;
            }
        }

        // Check if current process finished its quantum or completed
        if (running_process != -1) {
            bool needs_preemption = (current_quantum >= quantum);
            bool has_finished = (processes[running_process].remaining_time <= 0);

            if (needs_preemption || has_finished) {
                // If quantum over and not finished, add overhead
                if (needs_preemption && processes[running_process].remaining_time > 0) {
                    processes[running_process].overhead = true;
                    process_queue[queue_size++] = running_process;
                    overhead_remaining = overhead_time;
                }
                running_process = -1;
                current_quantum = 0;
            }
        }

        // Get next process from queue
        if (running_process == -1 && queue_size > 0 && overhead_remaining == 0) {
            running_process = process_queue[0];
            // Remove from queue
            for (int i = 0; i < queue_size - 1; i++) {
                process_queue[i] = process_queue[i + 1];
            }
            queue_size--;
        }

        // Update states
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].overhead) {
                processes[i].timeline[t] = OVERHEAD;

                overhead_remaining--;
                if (overhead_remaining == 0) {
                    processes[i].overhead = false;
                }
            }
            else if (i == running_process && running_process != -1) {
                processes[i].timeline[t] = EXECUTING;
                processes[i].remaining_time--;
                current_quantum++;
            } else if (processes[i].arrival_time <= t && processes[i].remaining_time > 0) {
                processes[i].timeline[t] = WAITING;
            } else if (processes[i].arrival_time > t) {
                processes[i].timeline[t] = NOT_ARRIVED;
            } else {
                processes[i].timeline[t] = COMPLETED;
            }
        }
    }
}

void run_current_algorithm() {
    switch (current_algorithm) {
        case 0: execute_fifo(); break;
        case 1: execute_sjf(); break;
        case 2: execute_edf(); break;
        case 3: execute_rr(); break;
    }
}

void reset_simulation() {
    current_time = 0;
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].execution_time;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }
}
