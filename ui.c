#include "ui.h"
#include "globals.h"
#include "memory.h"

void draw_legend(int start_y, int start_x) {
    mvaddstr(start_y, start_x, "LEGEND:");
    mvaddstr(start_y + 2, start_x, "[   ] - Executing");
    mvaddstr(start_y + 3, start_x, "[   ] - Waiting");
    mvaddstr(start_y + 4, start_x, "[   ] - Overhead");
    mvaddstr(start_y + 5, start_x, "[   ] - Page Fault");
    mvaddstr(start_y + 6, start_x, "[   ] - Deadline Missed");
    mvaddstr(start_y + 7, start_x, "[ | ] - Absolute Deadline");

    // Legend colors
    attron(COLOR_PAIR(2));
    mvaddstr(start_y + 2, start_x + 1, "   ");
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvaddstr(start_y + 3, start_x + 1, "   ");
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    mvaddstr(start_y + 4, start_x + 1, "   ");
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(6));
    mvaddstr(start_y + 5, start_x + 1, "   ");
    attroff(COLOR_PAIR(6));

    attron(COLOR_PAIR(5));
    mvaddstr(start_y + 6, start_x + 1, "   ");
    attroff(COLOR_PAIR(5));
}

void draw_gantt_chart(int start_y, int start_x) {
    // Time header
    mvaddstr(start_y, start_x, "Time: ");

    // Get visible range
    int start_time = time_offset;
    int end_time = time_offset + visible_columns;
    if (end_time > TOTAL_TIME) end_time = TOTAL_TIME;

    // Show only visible columns
    for (int t = start_time; t < end_time; t++) {
        int screen_col = start_x + 8 + (t - time_offset) * CELL_WIDTH;

        if (t == current_time) {
            attron(A_BOLD | A_UNDERLINE);
            mvprintw(start_y, screen_col, "%02d", t);
            attroff(A_BOLD | A_UNDERLINE);
        } else {
            mvprintw(start_y, screen_col, "%02d", t);
        }
    }

    // Scroll indicator if necessary
    if (time_offset > 0) {
        mvaddstr(start_y, start_x + 6, "<");
    }
    if (end_time < TOTAL_TIME) {
        mvaddstr(start_y, start_x + 10 + visible_columns * CELL_WIDTH - 2, ">");
    }

    // Process rows - only visible columns
    for (int i = 0; i < num_processes; i++) {
        mvprintw(start_y + 2 + i, start_x, "P%d: ", processes[i].id);

        for (int t = start_time; t < end_time; t++) {
            int screen_col = start_x + 8 + (t - time_offset) * CELL_WIDTH;
            int color = 1;
            char cell_char = ' ';
            bool show_page_fault = false;

            // If not current time, shows as not arrived
            if (t > current_time) {
                color = 1; // Gray
                cell_char = '_';
            } else {
                switch (processes[i].timeline[t]) {
                    case NOT_ARRIVED:
                        color = 1; // Gray
                        break;
                    case EXECUTING:
                        color = 2; // Green
                        // Check if page fault occurred during execution
                        if (memory_enabled && processes[i].page_fault_occurred && 
                            processes[i].page_fault_occurred[t]) {
                            show_page_fault = true;
                        }
                        break;
                    case WAITING:
                        color = 3; // Yellow
                        break;
                    case OVERHEAD:
                        color = 4; // Red
                        break;
                    case COMPLETED:
                        color = 1; // Gray
                        break;
                    case DEADLINE_MISSED:
                        color = 5; // White
                        break;
                    case PAGE_FAULT:
                        color = 6; // Cyan
                        break;
                }
            }

            // Highlight current time column
            if (t == current_time) {
                attron(A_BOLD);
            }

            // Draw colored cell
            attron(COLOR_PAIR(color));
            for (int w = 0; w < CELL_WIDTH - 1; w++) {
                mvaddch(start_y + 2 + i, screen_col + w, cell_char);
            }
            attroff(COLOR_PAIR(color));
            
            // If page fault occurred, overlay a visual indicator (e.g., 'F' or special char)
            if (show_page_fault) {
                attron(COLOR_PAIR(6) | A_BOLD);  // Cyan for page fault indicator
                mvaddch(start_y + 2 + i, screen_col, 'F');  // 'F' for Fault
                attroff(COLOR_PAIR(6) | A_BOLD);
            }

            // Here it considers the absolute deadline of each process by the calculation in abs_dead
            // Draws a vertical marker immediately to the right of the square and labels it with the process ID.
            int abs_dead = (processes[i].arrival_time + processes[i].deadline) - 1;
            // This condition ensures that the marker only appears in the EDF
            if (t == abs_dead && current_algorithm == 2) {
                int marker_col = screen_col + (CELL_WIDTH - 1); // Column immediately after the square
                mvaddch(start_y + 2 + i, marker_col, ACS_VLINE);
            }

            if (t == current_time) {
                attroff(A_BOLD);
            }
        }
    }

    // Show scroll information
    mvprintw(start_y + num_processes + 3, start_x,
             "Showing time %d to %d (Total: %d)",
             start_time, end_time - 1, TOTAL_TIME);
    
    // Show controls
    mvprintw(start_y + num_processes + 4, start_x,
             "Controls: SPACE=Run | </>=Navigate Memory | /=Follow | LEFT/RIGHT=Time | M=Menu | Q=Quit");
}


void draw_deadline_status(int y, int x, int satisfied, int cell_width) {
    if (satisfied) {
        attron(COLOR_PAIR(2)); // Green
        mvprintw(y, x, "%-*s", cell_width, "");
        attroff(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(4)); // Red
        mvprintw(y, x, "%-*s", cell_width, "");
        attroff(COLOR_PAIR(4));
    }
}

void draw_interface() {
    clear();

    // Title
    attron(A_BOLD | COLOR_PAIR(5)); // Contrast
    mvprintw(1, 2, "CPU SCHEDULING ALGORITHM SIMULATOR - %s", algorithm_names[current_algorithm]);
    attroff(A_BOLD | COLOR_PAIR(5));

    // Gantt chart
    mvaddstr(3, 2, "GANTT CHART:");
    draw_gantt_chart(4, 2);

    // Legend
    draw_legend(4, 75);

    // Controls
    mvaddstr(15, 2, "CONTROLS:");
    mvaddstr(16, 2, "F1-F5: Select algorithm (FIFO, SJF, EDF, RR, CFS)");
    mvaddstr(17, 2, "SPACE: Run/Reset simulation");
    mvaddstr(18, 2, "RIGHT|RIGHT ARROW: Advance or go back in time");
    mvaddstr(19, 2, "A|D: Scroll chart left and right");
    mvaddstr(20, 2, "H: Go to start, E: Go to end");
    mvaddstr(21, 2, "M: Configuration Menu");
    mvaddstr(22, 2, "Q: Quit");

    // Process information
    // mvaddstr(24, 2, "PROCESS INFORMATION:");
    // for (int i = 0; i < num_processes; i++) {
    //     mvprintw(25 + i, 2, "P%d: Arrival=%d, Execution=%d, Deadline=%d",
    //             processes[i].id, processes[i].arrival_time,
    //             processes[i].execution_time, processes[i].deadline);
    // }

    // Metrics table (shown after simulation finishes / when computed)
    int metrics_start_y = 24;
    if (metrics_computed) {
        // Title
        attron(A_BOLD);
        mvprintw(metrics_start_y, 2, "METRICS:");
        attroff(A_BOLD);

        // Table layout parameters
        int left_w = 18;      // width for metric name column
        int col_w = 12;       // width per process column
        int max_cols = 6;     // show up to 6 process columns

        // Table starting position and dimensions (robust borders and cells)
        int table_start_x = 2;
        int top = metrics_start_y + 2;          // top border row
        int header_row = top + 1;               // header row (process IDs)
        int first_metric_row = header_row + 1;  // first metric row
        int table_width = left_w + max_cols * col_w; // total inner width

        // Draw top border
        mvaddch(top, table_start_x, ACS_ULCORNER);
        for (int x = table_start_x + 1; x < table_start_x + table_width; x++) mvaddch(top, x, ACS_HLINE);
        mvaddch(top, table_start_x + table_width, ACS_URCORNER);

        // Draw header row vertical separators and left empty header cell
        mvaddch(header_row, table_start_x, ACS_VLINE);
        mvprintw(header_row, table_start_x + 1, "%-*s", left_w - 1, "");
        mvaddch(header_row, table_start_x + left_w, ACS_VLINE);
        for (int c = 0; c < max_cols; c++) {
            int col_start = table_start_x + left_w + c * col_w;
            mvaddch(header_row, col_start + col_w, ACS_VLINE);
            if (c < num_processes) {
                mvprintw(header_row, col_start + 1, "%-*s", col_w - 1, "");
                mvprintw(header_row, col_start + 1, "P%d", processes[c].id);
            }
        }

        // Draw horizontal separator under header
        mvaddch(header_row + 1, table_start_x, ACS_LTEE);
        for (int x = table_start_x + 1; x < table_start_x + table_width; x++) mvaddch(header_row + 1, x, ACS_HLINE);
        mvaddch(header_row + 1, table_start_x + table_width, ACS_RTEE);

        // Metric labels (rows) with borders
        const char *labels[MI_COUNT] = {
            "Arrival", "Execution", "Deadline", "Priority",
            "Start", "End", "Wait", "Turnaround", "Deadline?", "PgFaults"
        };

        for (int r = 0; r < MI_COUNT; r++) {
            int row_y = first_metric_row + r;

            // Left vertical border and metric name
            mvaddch(row_y, table_start_x, ACS_VLINE);

            // Skip deadline_ok label if not EDF algorithm, but still draw empty space
            if (r == MI_DEADLINE_OK && current_algorithm != 2) {
                mvprintw(row_y, table_start_x + 1, "%-*s", left_w - 1, ""); // Empty label
            } else if (r == MI_PAGE_FAULTS && !memory_enabled) {
                mvprintw(row_y, table_start_x + 1, "%-*s", left_w - 1, ""); // Empty label
            } else {
                mvprintw(row_y, table_start_x + 1, "%-*s", left_w - 1, labels[r]);
            }

            mvaddch(row_y, table_start_x + left_w, ACS_VLINE);

            // Cells per process
            for (int c = 0; c < max_cols; c++) {
                int col_start = table_start_x + left_w + c * col_w;
                // content area is col_start +1 .. col_start + col_w -1
                if (c < num_processes) {
                    int *m = processes[c].metrics;
                    if (r == MI_DEADLINE_OK && current_algorithm != 2) {
                        // Empty cell for deadline_ok when not EDF
                        mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, "");
                    } else if (r == MI_PAGE_FAULTS && !memory_enabled) {
                        // Empty cell for page faults when memory not enabled
                        mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, "");
                    } else if (r == MI_START) {
                        if (m[MI_START] < 0) mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, "");
                        else mvprintw(row_y, col_start + 1, "%-*d", col_w - 1, m[MI_START]);
                    } else if (r == MI_END) {
                        if (m[MI_END] <= 0) mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, "");
                        else mvprintw(row_y, col_start + 1, "%-*d", col_w - 1, m[MI_END]);
                    } else if (r == MI_DEADLINE_OK) {
                        draw_deadline_status(row_y, col_start + 1, m[MI_DEADLINE_OK], col_w - 1);
                    } else {
                        mvprintw(row_y, col_start + 1, "%-*d", col_w - 1, m[r]);
                    }
                } else {
                    mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, " ");
                }
                // right vertical border of the cell
                mvaddch(row_y, col_start + col_w, ACS_VLINE);
            }

            // Draw separator line below this row
            int sep_y = row_y + 1;
            if (r < MI_COUNT - 1) {
                mvaddch(sep_y, table_start_x, ACS_LTEE);
                for (int x = table_start_x + 1; x < table_start_x + table_width; x++) mvaddch(sep_y, x, ACS_HLINE);
                mvaddch(sep_y, table_start_x + table_width, ACS_RTEE);
            } else {
                // bottom border for last metric row
                mvaddch(sep_y, table_start_x, ACS_LLCORNER);
                for (int x = table_start_x + 1; x < table_start_x + table_width; x++) mvaddch(sep_y, x, ACS_HLINE);
                mvaddch(sep_y, table_start_x + table_width, ACS_LRCORNER);
            }
        }

        // Current time indicator placed after the table
        mvprintw(first_metric_row + MI_COUNT + 2, 2, "Current Time: %d", current_time);
        mvprintw(first_metric_row + MI_COUNT + 2, 20, "Quantum: %d", quantum);
        mvprintw(first_metric_row + MI_COUNT + 2, 34, "Overhead: %d", overhead_time);

        // Summary statistics (quantitative summary)
        int summary_y = first_metric_row + MI_COUNT + 4;
        attron(A_BOLD);
        mvaddstr(summary_y, 2, "SUMMARY:");
        attroff(A_BOLD);

        mvprintw(summary_y + 1, 2, "Average Arrival Time: %.2f  |  Average Execution Time: %.2f  |  Average Waiting Time: %.2f  |  Average Turnaround: %.2f",
                 summary_stats.avg_arrival, summary_stats.avg_execution, summary_stats.avg_wait, summary_stats.avg_turnaround);
        mvprintw(summary_y + 2, 2, "Throughput: %.4f process / time unit  |  Idleness: %.2f%%  |  Context Switches: %d",
                 summary_stats.throughput, summary_stats.idle_percentage, summary_stats.context_switches);
        
        // Memory visualization (if enabled)
        if (memory_enabled) {
            draw_memory_visualization(summary_y + 4);
        }
    } else {
        // Current time indicator
        mvprintw(metrics_start_y, 2, "Current Time: %d", current_time);
    }

    refresh();
}

void draw_memory_visualization(int start_y) {
    int x = 2;
    int y = start_y;
    
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(y, x, "MEMORY VISUALIZATION (Time: %d):", memory_animation_frame);
    attroff(A_BOLD | COLOR_PAIR(2));
    
    // Display controls
    mvprintw(y, x + 40, "[< >: Navigate | Space: Follow]");
    
    // Determine which process is currently executing at this frame
    int executing_process = -1;
    if (memory_animation_frame >= 0 && memory_animation_frame < TOTAL_TIME) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].timeline && 
                processes[i].timeline[memory_animation_frame] == EXECUTING) {
                executing_process = processes[i].id;
                break;
            }
        }
    }
    
    // Use historical RAM state if available, otherwise use current state
    bool use_history = history_initialized && 
                       memory_animation_frame >= 0 && 
                       memory_animation_frame < MAX_HISTORY_SIZE;
    
    // RAM Grid (50 frames = 10 cols x 5 rows)
    attron(A_BOLD);
    mvprintw(y + 2, x, "RAM (50 Frames):");
    attroff(A_BOLD);
    
    int ram_y = y + 3;
    int ram_x = x + 2;
    
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 10; col++) {
            int frame_idx = row * 10 + col;
            int pos_y = ram_y + row;
            int pos_x = ram_x + col * 5;  // 5 chars spacing for better visualization
            
            int proc_id = -1;
            if (use_history) {
                proc_id = ram_history[memory_animation_frame][frame_idx].process_id;
            } else {
                proc_id = ram_frames[frame_idx].process_id;
            }
            
            if (proc_id == -1) {
                // Empty frame - gray background
                attron(COLOR_PAIR(1));
                mvprintw(pos_y, pos_x, "[  ]");
                attroff(COLOR_PAIR(1));
            } else {
                // Frame occupied
                // Green only if this process is currently executing
                if (proc_id == executing_process) {
                    attron(COLOR_PAIR(2));  // Green - executing
                } else {
                    attron(COLOR_PAIR(3));  // Yellow - occupied but not executing
                }
                mvprintw(pos_y, pos_x, "[%d]", proc_id);
                
                if (proc_id == executing_process) {
                    attroff(COLOR_PAIR(2));
                } else {
                    attroff(COLOR_PAIR(3));
                }
            }
        }
    }
    
    // Disk visualization - show processes NOT in RAM at this frame
    int disk_y = ram_y + 7;
    attron(A_BOLD);
    mvprintw(disk_y, x, "DISK:");
    attroff(A_BOLD);
    
    // Build set of processes in RAM at this frame
    bool process_in_ram[MAX_PROCESSES] = {false};
    if (use_history) {
        for (int f = 0; f < TOTAL_RAM_FRAMES; f++) {
            int proc_id = ram_history[memory_animation_frame][f].process_id;
            if (proc_id > 0 && proc_id <= MAX_PROCESSES) {
                // Find process index by id
                for (int p = 0; p < num_processes; p++) {
                    if (processes[p].id == proc_id) {
                        process_in_ram[p] = true;
                        break;
                    }
                }
            }
        }
    } else {
        for (int f = 0; f < TOTAL_RAM_FRAMES; f++) {
            int proc_id = ram_frames[f].process_id;
            if (proc_id > 0 && proc_id <= MAX_PROCESSES) {
                for (int p = 0; p < num_processes; p++) {
                    if (processes[p].id == proc_id) {
                        process_in_ram[p] = true;
                        break;
                    }
                }
            }
        }
    }
    
    // Disk Grid - show processes not in RAM
    int disk_grid_y = disk_y + 1;
    int disk_grid_x = x + 2;
    int disk_items_shown = 0;
    
    // Show each process that's in DISK (not in RAM)
    for (int i = 0; i < num_processes; i++) {
        // Check if process has arrived at this frame
        bool has_arrived = memory_animation_frame >= processes[i].arrival_time;
        
        if (has_arrived && !process_in_ram[i] && processes[i].num_pages > 0) {
            // Show process pages in disk
            for (int p = 0; p < processes[i].num_pages && disk_items_shown < 50; p++) {
                int row = disk_items_shown / 10;
                int col = disk_items_shown % 10;
                int pos_y = disk_grid_y + row;
                int pos_x = disk_grid_x + col * 5;
                
                attron(COLOR_PAIR(4));  // Red for disk
                mvprintw(pos_y, pos_x, "[%d]", processes[i].id);
                attroff(COLOR_PAIR(4));
                
                disk_items_shown++;
            }
        }
    }
    
    // Fill remaining disk squares with empty
    for (int remaining = disk_items_shown; remaining < 50; remaining++) {
        int row = remaining / 10;
        int col = remaining % 10;
        int pos_y = disk_grid_y + row;
        int pos_x = disk_grid_x + col * 5;
        
        attron(COLOR_PAIR(1));
        mvprintw(pos_y, pos_x, "[  ]");
        attroff(COLOR_PAIR(1));
    }
    
    // Page fault statistics - in a separate area
    int pf_y = disk_grid_y + 6;
    attron(A_BOLD);
    mvprintw(pf_y, x, "Page Faults by Process:");
    attroff(A_BOLD);
    
    for (int i = 0; i < num_processes; i++) {
        mvprintw(pf_y + 1 + i, x + 2, "P%d: %d faults", 
            processes[i].id, processes[i].page_faults);
    }
    
    // Memory policy info
    int policy_y = pf_y + num_processes + 2;
    mvprintw(policy_y, x, "Policy: %s", 
        replacement_policy == POLICY_FIFO ? "FIFO" : "LRU");
}
