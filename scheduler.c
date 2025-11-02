#include "scheduler.h"
#include <stdlib.h>

Process processes[MAX_PROCESSES];
int num_processes = 0;
int current_time = 0;
int current_algorithm = 0;
const char *algorithm_names[] = {"FIFO", "SJF", "EDF", "Round Robin"};

void initialize_processes() {
    // Example process 1
    processes[0].id = 1;
    processes[0].arrival_time = 0;
    processes[0].execution_time = 5;
    processes[0].remaining_time = 5;
    processes[0].priority = 1;
    processes[0].deadline = 10;

    // Example process 2
    processes[1].id = 2;
    processes[1].arrival_time = 2;
    processes[1].execution_time = 3;
    processes[1].remaining_time = 3;
    processes[1].priority = 2;
    processes[1].deadline = 8;

    // Example process 3
    processes[2].id = 3;
    processes[2].arrival_time = 4;
    processes[2].execution_time = 4;
    processes[2].remaining_time = 4;
    processes[2].priority = 1;
    processes[2].deadline = 15;

    num_processes = 3;

    // Allocate and initialize timeline arrays
    for (int i = 0; i < num_processes; i++) {
        processes[i].timeline = malloc(TOTAL_TIME * sizeof(ProcessState));
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }
}

void execute_fifo() {
    for (int t = 0; t < TOTAL_TIME; t++) {
        int running_process = -1;

        // Find first process that arrived and hasn't finished
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= t && processes[i].remaining_time > 0) {
                running_process = i;
                break;
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

void execute_sjf() {
    // Reset remaining times
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].execution_time;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }

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
    // Reset remaining times
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].execution_time;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }

    for (int t = 0; t < TOTAL_TIME; t++) {
        int running_process = -1;
        int earliest_deadline = TOTAL_TIME + 1;

        // Find process with earliest deadline
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= t &&
                processes[i].remaining_time > 0 &&
                processes[i].deadline < earliest_deadline) {
                earliest_deadline = processes[i].deadline;
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

void execute_rr() {
    // Reset remaining times
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].execution_time;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }

    int quantum = 2; // Time quantum for Round Robin
    int current_quantum = 0;
    int running_process = -1;
    int process_queue[MAX_PROCESSES];
    int queue_size = 0;

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
            if (current_quantum >= quantum || processes[running_process].remaining_time <= 0) {
                // Add back to queue if not completed
                if (processes[running_process].remaining_time > 0) {
                    process_queue[queue_size++] = running_process;
                }
                running_process = -1;
                current_quantum = 0;
            }
        }

        // Get next process from queue
        if (running_process == -1 && queue_size > 0) {
            running_process = process_queue[0];
            // Shift queue
            for (int i = 0; i < queue_size - 1; i++) {
                process_queue[i] = process_queue[i + 1];
            }
            queue_size--;
        }

        // Update states
        for (int i = 0; i < num_processes; i++) {
            if (i == running_process && running_process != -1) {
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
