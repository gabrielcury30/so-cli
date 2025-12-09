#include "memory.h"
#include "globals.h"

void init_memory_system() {
    // Safety check
    if (num_processes == 0) {
        return;
    }

    // Initialize all frames as free
    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        ram_frames[i].process_id = -1;
        ram_frames[i].page_number = -1;
        ram_frames[i].load_time = 0;
        ram_frames[i].last_access = 0;
    }

    // Initialize all processes - start with all pages in DISK
    for (int i = 0; i < num_processes; i++) {
        // Use the num_pages configured by user (already set in process configuration)
        // If not set, default to 3 pages
        if (processes[i].num_pages <= 0) {
            processes[i].num_pages = 3;
        }
        // Limit to max pages per process
        if (processes[i].num_pages > MAX_PAGES_PER_PROCESS) {
            processes[i].num_pages = MAX_PAGES_PER_PROCESS;
        }

        processes[i].page_faults = 0;
        processes[i].page_fault_remaining = 0;
        processes[i].next_page_to_access = 0;
        processes[i].exec_units_since_page_access = 0;

        // Initialize all pages as not in RAM (in DISK)
        for (int p = 0; p < processes[i].num_pages; p++) {
            processes[i].pages[p].in_ram = false;
            processes[i].pages[p].frame_index = -1;
        }
    }

    current_time_global = 0;
}

int find_free_frame() {
    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        if (ram_frames[i].process_id == -1) {
            return i;
        }
    }
    return -1;  // No free frame
}

int find_victim_frame_fifo() {
    int oldest_time = current_time_global + 1;
    int victim = 0;

    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        if (ram_frames[i].process_id != -1 && ram_frames[i].load_time < oldest_time) {
            oldest_time = ram_frames[i].load_time;
            victim = i;
        }
    }

    return victim;
}

int find_victim_frame_lru() {
    int oldest_access = current_time_global + 1;
    int victim = 0;

    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        if (ram_frames[i].process_id != -1 && ram_frames[i].last_access < oldest_access) {
            oldest_access = ram_frames[i].last_access;
            victim = i;
        }
    }

    return victim;
}

void evict_page(int frame_index) {
    int proc_id = ram_frames[frame_index].process_id;
    int page_num = ram_frames[frame_index].page_number;

    // Find process by id
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].id == proc_id) {
            processes[i].pages[page_num].in_ram = false;
            processes[i].pages[page_num].frame_index = -1;
            break;
        }
    }

    ram_frames[frame_index].process_id = -1;
    ram_frames[frame_index].page_number = -1;
}

void load_page_to_ram(int process_index, int page_number) {
    int frame_index = find_free_frame();

    // If no free frame, evict one
    if (frame_index == -1) {
        if (replacement_policy == POLICY_FIFO) {
            frame_index = find_victim_frame_fifo();
        } else {
            frame_index = find_victim_frame_lru();
        }
        evict_page(frame_index);
    }

    // Load page into frame
    ram_frames[frame_index].process_id = processes[process_index].id;
    ram_frames[frame_index].page_number = page_number;
    ram_frames[frame_index].load_time = current_time_global;
    ram_frames[frame_index].last_access = current_time_global;

    processes[process_index].pages[page_number].in_ram = true;
    processes[process_index].pages[page_number].frame_index = frame_index;
}

// Check if process needs to be loaded into RAM (causes page fault if in disk)
bool check_page_fault(int process_index) {
    if (!memory_enabled) return false;

    // Safety check
    if (processes[process_index].num_pages <= 0) return false;

    // Check if ANY page of this process is already in RAM
    bool process_in_ram = false;
    for (int p = 0; p < processes[process_index].num_pages; p++) {
        if (processes[process_index].pages[p].in_ram) {
            process_in_ram = true;
            break;
        }
    }

    // If process is already in RAM, no page fault - just update access time
    if (process_in_ram) {
        // Update LRU times for all pages of this process
        for (int p = 0; p < processes[process_index].num_pages; p++) {
            if (processes[process_index].pages[p].in_ram) {
                int frame_idx = processes[process_index].pages[p].frame_index;
                if (frame_idx >= 0 && frame_idx < TOTAL_RAM_FRAMES) {
                    ram_frames[frame_idx].last_access = current_time_global;
                }
            }
        }
        return false;
    }

    // Process is in DISK - need to load it (PAGE FAULT!)
    processes[process_index].page_faults++;

    // Check if we have enough free frames
    int free_count = 0;
    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        if (ram_frames[i].process_id == -1) {
            free_count++;
        }
    }

    // If not enough space, evict another process
    while (free_count < processes[process_index].num_pages) {
        // Find process to evict using FIFO or LRU
        int victim_process_id = -1;

        if (replacement_policy == POLICY_FIFO) {
            // Find oldest loaded process
            int oldest_time = current_time_global + 1;
            for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
                if (ram_frames[i].process_id != -1 &&
                    ram_frames[i].process_id != processes[process_index].id &&
                    ram_frames[i].load_time < oldest_time) {
                    oldest_time = ram_frames[i].load_time;
                    victim_process_id = ram_frames[i].process_id;
                }
            }
        } else {  // LRU
            // Find least recently used process
            int oldest_access = current_time_global + 1;
            for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
                if (ram_frames[i].process_id != -1 &&
                    ram_frames[i].process_id != processes[process_index].id &&
                    ram_frames[i].last_access < oldest_access) {
                    oldest_access = ram_frames[i].last_access;
                    victim_process_id = ram_frames[i].process_id;
                }
            }
        }

        if (victim_process_id == -1) break;  // No victim found

        // Evict all pages of victim process
        for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
            if (ram_frames[i].process_id == victim_process_id) {
                // Find the process index
                for (int p = 0; p < num_processes; p++) {
                    if (processes[p].id == victim_process_id) {
                        int page_num = ram_frames[i].page_number;
                        if (page_num >= 0 && page_num < processes[p].num_pages) {
                            processes[p].pages[page_num].in_ram = false;
                            processes[p].pages[page_num].frame_index = -1;
                        }
                        break;
                    }
                }
                ram_frames[i].process_id = -1;
                ram_frames[i].page_number = -1;
                free_count++;
            }
        }
    }

    // Load all pages of current process into RAM
    int loaded = 0;
    for (int i = 0; i < TOTAL_RAM_FRAMES && loaded < processes[process_index].num_pages; i++) {
        if (ram_frames[i].process_id == -1) {
            ram_frames[i].process_id = processes[process_index].id;
            ram_frames[i].page_number = loaded;
            ram_frames[i].load_time = current_time_global;
            ram_frames[i].last_access = current_time_global;

            processes[process_index].pages[loaded].in_ram = true;
            processes[process_index].pages[loaded].frame_index = i;
            loaded++;
        }
    }

    return true;  // Page fault occurred
}

int count_pages_in_ram(int process_index) {
    int count = 0;
    for (int p = 0; p < processes[process_index].num_pages; p++) {
        if (processes[process_index].pages[p].in_ram) {
            count++;
        }
    }
    return count;
}

int count_pages_in_disk(int process_index) {
    return processes[process_index].num_pages - count_pages_in_ram(process_index);
}

void save_memory_state(int time_unit) {
    if (!memory_enabled || time_unit < 0 || time_unit >= MAX_HISTORY_SIZE) {
        return;
    }

    // Save current RAM state to history
    for (int i = 0; i < TOTAL_RAM_FRAMES; i++) {
        ram_history[time_unit][i].process_id = ram_frames[i].process_id;
        ram_history[time_unit][i].page_number = ram_frames[i].page_number;
    }

    history_initialized = true;
}
