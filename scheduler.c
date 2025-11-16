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

void execute_cfs() {
    int running_process = -1;
    int process_completed = 0;
    int overhead_remaining = 0;

    // Initialize vruntime for all processes
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].execution_time;
        processes[i].vruntime = 0.0;
        processes[i].overhead = false;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }

    for (int t = 0; t < TOTAL_TIME && process_completed < num_processes; t++) {
        // Check if current process finished or needs preemption
        if (running_process != -1) {
            bool has_finished = (processes[running_process].remaining_time <= 0);

            if (has_finished) {
                process_completed++;
                running_process = -1;
            } else {
                // Check if preemption is needed (another process has lower vruntime)
                double current_vruntime = processes[running_process].vruntime;
                int preempt_candidate = -1;

                for (int i = 0; i < num_processes; i++) {
                    if (i != running_process &&
                        processes[i].arrival_time <= t &&
                        processes[i].remaining_time > 0 &&
                        processes[i].vruntime < current_vruntime) {
                        preempt_candidate = i;
                        break;
                    }
                }

                // If preemption needed, mark overhead for current process
                if (preempt_candidate != -1) {
                    processes[running_process].overhead = true;
                    overhead_remaining = overhead_time;
                    running_process = -1;
                }
            }
        }

        // Get next process from ready queue if no process is running and overhead finished
        if (running_process == -1 && overhead_remaining == 0) {
            double min_vruntime = 1e9;
            int selected_process = -1;

            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time <= t &&
                    processes[i].remaining_time > 0 &&
                    processes[i].vruntime < min_vruntime) {
                    min_vruntime = processes[i].vruntime;
                    selected_process = i;
                }
            }

            running_process = selected_process;
        }

        // Update states for all processes
        for (int i = 0; i < num_processes; i++) {
            // First, handle overhead state
            if (processes[i].overhead) {
                processes[i].timeline[t] = OVERHEAD;
                overhead_remaining--;
                if (overhead_remaining == 0) {
                    processes[i].overhead = false;
                }
            }
            // Then handle execution state
            else if (i == running_process && running_process != -1) {
                processes[i].timeline[t] = EXECUTING;
                
                // Delta T: real CPU time used (only for EXECUTING, not OVERHEAD)
                int delta_t = 1;  // 1 unit of CPU time this timeslice
                
                // Calculate weight based on priority: w(priority) = 1.25^(priority-1)
                double priority_weight = pow(1.25, processes[i].priority - 1);
                
                // Update vruntime: vruntime += delta_t * w(priority)
                processes[i].vruntime += delta_t * priority_weight;
                
                // Decrease remaining time (only during actual execution)
                processes[i].remaining_time--;
            }
            // Then handle waiting/other states
            else if (processes[i].arrival_time <= t && processes[i].remaining_time > 0) {
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
        case 4: execute_cfs(); break;
    }
    // After running the chosen algorithm compute the summary metrics
    compute_metrics_for_all();
    compute_summary_stats();
}

// Compute metrics for every process based on its timeline
void compute_metrics_for_all() {
    // Reset metrics flag
    metrics_computed = false;

    for (int i = 0; i < num_processes; i++) {
        // Basic copies
        processes[i].metrics[MI_ARRIVAL] = processes[i].arrival_time;
        processes[i].metrics[MI_EXECUTION] = processes[i].execution_time;
        processes[i].metrics[MI_DEADLINE] = processes[i].deadline;
        processes[i].metrics[MI_PRIORITY] = processes[i].priority;

        // Safety check: ensure timeline is allocated
        if (!processes[i].timeline) {
            processes[i].metrics[MI_START] = -1;
            processes[i].metrics[MI_END] = -1;
            processes[i].metrics[MI_WAIT] = 0;
            processes[i].metrics[MI_TURNAROUND] = 0;
            processes[i].metrics[MI_DEADLINE_OK] = 0;
            processes[i].final_status = PS_PENDING;
            continue;
        }

        int start = -1;
        int last_exec = -1;
        int wait_count = 0;
        for (int t = 0; t < TOTAL_TIME; t++) {
            ProcessState st = processes[i].timeline[t];
            if ((st == EXECUTING) || (st == DEADLINE_MISSED)) {
                if (start == -1) start = t;
                last_exec = t;
            }
            if (st == WAITING) {
                wait_count++;
            }
        }

        int end_time = -1;
        if (last_exec != -1) end_time = last_exec + 1; // end as exclusive time

        // Ajuste temporário: reduzir 1 (resultado pedagógico pedido: incrementar depois)
        if (end_time != -1) end_time = end_time - 1;

        processes[i].metrics[MI_START] = (start == -1) ? -1 : start;
        processes[i].metrics[MI_END] = end_time;
        processes[i].metrics[MI_WAIT] = wait_count;

        int turnaround = 0;
        if (end_time != -1) {
            turnaround = (end_time - processes[i].arrival_time) + 1; // add +1 as requested
            if (turnaround < 0) turnaround = 0; // protect against negative values
        }
        processes[i].metrics[MI_TURNAROUND] = turnaround;

        int deadline_ok = 0;
        if (end_time != -1) {
            // Deadline is relative to arrival_time (as used in EDF)
            if (turnaround <= processes[i].deadline) {
                deadline_ok = 1;
                processes[i].final_status = PS_COMPLETED_ON_TIME;
            } else {
                deadline_ok = 0;
                processes[i].final_status = PS_DEADLINE_MISSED;
            }
        } else {
            processes[i].final_status = PS_PENDING;
        }
        processes[i].metrics[MI_DEADLINE_OK] = deadline_ok;
    }

    metrics_computed = true;
}

// Compute summary statistics for the entire simulation
void compute_summary_stats() {
    // Initialize summary stats
    summary_stats.avg_arrival = 0;
    summary_stats.avg_execution = 0;
    summary_stats.avg_wait = 0;
    summary_stats.avg_turnaround = 0;
    summary_stats.throughput = 0;
    summary_stats.idle_percentage = 0;
    summary_stats.context_switches = 0;

    if (num_processes == 0) return;

    // Calculate averages
    int total_execution = 0;
    int total_wait = 0;
    int total_turnaround = 0;
    int completed_count = 0;

    for (int i = 0; i < num_processes; i++) {
        summary_stats.avg_arrival += processes[i].metrics[MI_ARRIVAL];
        total_execution += processes[i].metrics[MI_EXECUTION];
        total_wait += processes[i].metrics[MI_WAIT];
        total_turnaround += processes[i].metrics[MI_TURNAROUND];
        if (processes[i].metrics[MI_END] > 0) {
            completed_count++;
        }
    }

    summary_stats.avg_arrival /= num_processes;
    summary_stats.avg_execution = total_execution / (double)num_processes;
    summary_stats.avg_wait = total_wait / (double)num_processes;
    summary_stats.avg_turnaround = total_turnaround / (double)num_processes;

    // Throughput: completed processes per unit time
    if (TOTAL_TIME > 0) {
        summary_stats.throughput = completed_count / (double)TOTAL_TIME;
    }

    // Count idle time and context switches (only count when OVERHEAD occurs)
    int idle_time = 0;

    for (int t = 0; t < TOTAL_TIME; t++) {
        int executing_process = -1;
        bool has_overhead = false;

        // Find which process (if any) is executing at time t and check for overhead
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].timeline) {
                if (processes[i].timeline[t] == EXECUTING) {
                    executing_process = i;
                }
                if (processes[i].timeline[t] == OVERHEAD) {
                    has_overhead = true;
                }
            }
        }

        // Count idle time
        if (executing_process == -1 && !has_overhead) {
            idle_time++;
        }

        // Count context switches: only when OVERHEAD occurs (represents forced context switch)
        if (has_overhead) {
            summary_stats.context_switches++;
        }
    }

    // Calculate idle percentage
    if (TOTAL_TIME > 0) {
        summary_stats.idle_percentage = (idle_time / (double)TOTAL_TIME) * 100.0;
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
