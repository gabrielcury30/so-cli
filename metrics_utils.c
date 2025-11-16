#include "globals.h"

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
        if (last_exec != -1) end_time = last_exec + 1; // End as exclusive time

        // Ajuste temporário: reduzir 1 (resultado pedagógico pedido: incrementar depois)
        if (end_time != -1) end_time = end_time - 1;

        processes[i].metrics[MI_START] = (start == -1) ? -1 : start;
        processes[i].metrics[MI_END] = end_time;
        processes[i].metrics[MI_WAIT] = wait_count;

        int turnaround = 0;
        if (end_time != -1) {
            turnaround = (end_time - processes[i].arrival_time) + 1; // Add +1 as requested
            if (turnaround < 0) turnaround = 0; // Protect against negative values
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
