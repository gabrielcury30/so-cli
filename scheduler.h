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

#endif
