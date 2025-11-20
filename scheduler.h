#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "globals.h"
#include <math.h>

void initialize_default_processes();
void execute_fifo();
void execute_sjf();
void execute_edf();
void execute_rr();
void execute_cfs();
void run_current_algorithm();
void reset_simulation();

#endif
