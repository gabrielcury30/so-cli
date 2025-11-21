#ifndef MEMORY_H
#define MEMORY_H

#include "globals.h"

// Initialize memory system
void init_memory_system();

// Find free or victim frame
int find_free_frame();
int find_victim_frame_fifo();
int find_victim_frame_lru();

// Page management
void evict_page(int frame_index);
void load_page_to_ram(int process_index, int page_number);
bool check_page_fault(int process_index);

// Statistics
int count_pages_in_ram(int process_index);
int count_pages_in_disk(int process_index);

// Memory history for animation
void save_memory_state(int time_unit);

#endif
