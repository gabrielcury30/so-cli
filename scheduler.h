#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "globals.h"

void initialize_default_processes();
void execute_fifo();
void execute_sjf();
void execute_edf();
void execute_rr();
void run_current_algorithm();
void reset_simulation();
// Metrics utilities
void compute_metrics_for_all();
void compute_summary_stats();

#endif
