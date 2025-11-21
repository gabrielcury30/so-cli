#include <stdlib.h>
#include <string.h>
#include "config_ui.h"
#include "globals.h"
#include "ui.h"
#include "screen_utils.h"

void show_error_message(const char* message) {
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    int error_y = screen_height - 3;
    int error_x = 2;

    // Clear error area
    for (int i = 0; i < screen_width - 4; i++) {
        mvaddch(error_y, error_x + i, ' ');
    }

    attron(COLOR_PAIR(4) | A_BOLD);
    mvaddstr(error_y, error_x, message);
    attroff(COLOR_PAIR(4) | A_BOLD);
    refresh();
}

void clear_error_message() {
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    int error_y = screen_height - 3;
    for (int i = 0; i < screen_width - 4; i++) {
        mvaddch(error_y, 2 + i, ' ');
    }
    refresh();
}

int get_int_input(int y, int x, const char* prompt, int min_val, int max_val, int default_val) {
    char input[10];
    int value = default_val;
    int original_value = default_val;

    while (1) {
        clear_error_message();

        // Clear input area
        for (int i = 0; i < 4; i++) {
            mvaddch(y, x + strlen(prompt) + i, ' ');
        }

        // Show prompt and current value
        mvprintw(y, x, "%s [%d]: ", prompt, value);
        refresh();

        // Get input
        echo();
        nocbreak(); // Temporary disable cbreak to read input properly
        getnstr(input, sizeof(input) - 1);
        noecho();
        cbreak();

        if (strlen(input) == 0) {
            clear_error_message();
            return value; // Use current value if empty input
        }

        // Convert to integer
        char* endptr;
        value = strtol(input, &endptr, 10);

        if (endptr != input && value >= min_val && value <= max_val) {
            clear_error_message();
            break; // Valid input
        }

        // Show error
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg),
            "Invalid input! Enter value between %d and %d", min_val, max_val);
        show_error_message(error_msg);

        napms(1500); // Wait 1.5 seconds
        value = original_value;
    }

    return value;
}

void edit_process_screen() {
    int selected_process = 0;
    int ch;
    int screen_height, screen_width;

    while (1) {
        clear();
        getmaxyx(stdscr, screen_height, screen_width);

        // Calculate centered position
        int box_height = 15 + num_processes;
        int box_width = 60;
        int start_y = (screen_height - box_height) / 2;
        int start_x = (screen_width - box_width) / 2;

        // Draw frame
        attron(COLOR_PAIR(2));
        for (int i = 0; i < box_height; i++) {
            for (int j = 0; j < box_width; j++) {
                if (i == 0 || i == box_height - 1 || j == 0 || j == box_width - 1) {
                    mvaddch(start_y + i, start_x + j, ' ');
                }
            }
        }
        attroff(COLOR_PAIR(2));

        // Title (centered)
        attron(A_BOLD | COLOR_PAIR(2));
        mvaddstr(start_y, start_x + (box_width - 22) / 2, "PROCESS CONFIGURATION");
        attroff(A_BOLD | COLOR_PAIR(2));

        // Process list header
        mvaddstr(start_y + 3, start_x + 2, "ID");
        mvaddstr(start_y + 3, start_x + 8, "Arrival");
        mvaddstr(start_y + 3, start_x + 18, "Exec");
        mvaddstr(start_y + 3, start_x + 26, "Deadline");
        mvaddstr(start_y + 3, start_x + 37, "Priority");
        mvaddstr(start_y + 3, start_x + 48, "Pages");

        // List processes
        for (int i = 0; i < num_processes; i++) {
            if (i == selected_process) {
                attron(A_REVERSE | COLOR_PAIR(2));
            }

            mvprintw(start_y + 5 + i, start_x + 2, "P%d", processes[i].id);
            mvprintw(start_y + 5 + i, start_x + 8, "%d", processes[i].arrival_time);
            mvprintw(start_y + 5 + i, start_x + 18, "%d", processes[i].execution_time);
            mvprintw(start_y + 5 + i, start_x + 26, "%d", processes[i].deadline);
            mvprintw(start_y + 5 + i, start_x + 37, "%d", processes[i].priority);
            mvprintw(start_y + 5 + i, start_x + 48, "%d", processes[i].num_pages);

            if (i == selected_process) {
                attroff(A_REVERSE | COLOR_PAIR(2));
            }
        }

        // Controls (centered at bottom of box)
        int controls_y = start_y + box_height - 8;
        mvaddstr(controls_y, start_x + 2, "UP/DOWN: Select process");
        mvaddstr(controls_y + 1, start_x + 2, "ENTER: Edit selected process");
        mvaddstr(controls_y + 2, start_x + 2, "'A': Add new process");
        mvaddstr(controls_y + 3, start_x + 2, "'D': Delete process");
        mvaddstr(controls_y + 4, start_x + 2, "'M': Back to main menu");

        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
                if (selected_process > 0) selected_process--;
                break;
            case KEY_DOWN:
                if (selected_process < num_processes - 1) selected_process++;
                break;
            case '\n': // ENTER key
            case KEY_ENTER:
                if (num_processes > 0) {
                    // Edit selected process (centered input)
                    processes[selected_process].arrival_time = get_int_input(
                        start_y + 5 + selected_process, start_x + 6,
                        "", 0, TOTAL_TIME - 1,
                        processes[selected_process].arrival_time
                    );
                    processes[selected_process].execution_time = get_int_input(
                        start_y + 5 + selected_process, start_x + 16,
                        "", 1, TOTAL_TIME,
                        processes[selected_process].execution_time
                    );
                    processes[selected_process].deadline = get_int_input(
                        start_y + 5 + selected_process, start_x + 24,
                        "", processes[selected_process].arrival_time + 1,
                        TOTAL_TIME * 2, processes[selected_process].deadline
                    );
                    processes[selected_process].priority = get_int_input(
                        start_y + 5 + selected_process, start_x + 35,
                        "", 1, 10, processes[selected_process].priority
                    );
                    processes[selected_process].num_pages = get_int_input(
                        start_y + 5 + selected_process, start_x + 46,
                        "", 1, MAX_PAGES_PER_PROCESS,
                        processes[selected_process].num_pages > 0 ? processes[selected_process].num_pages : 3
                    );
                    processes[selected_process].remaining_time = processes[selected_process].execution_time;
                }
                break;
            case 'a':
            case 'A':
                if (num_processes < MAX_PROCESSES) {
                    int new_idx = num_processes;
                    processes[new_idx].id = new_idx + 1;
                    processes[new_idx].arrival_time = 0;
                    processes[new_idx].execution_time = 1;
                    processes[new_idx].remaining_time = 1;
                    processes[new_idx].deadline = 10;
                    processes[new_idx].priority = 1;
                    processes[new_idx].final_status = PS_PENDING;
                    processes[new_idx].timeline = NULL;
                    processes[new_idx].vruntime = 0.0;
                    processes[new_idx].overhead = false;
                    processes[new_idx].num_pages = 3; // Default to 3 pages
                    processes[new_idx].page_faults = 0;
                    processes[new_idx].page_fault_remaining = 0;
                    processes[new_idx].next_page_to_access = 0;
                    processes[new_idx].exec_units_since_page_access = 0;
                    
                    for (int p = 0; p < MAX_PAGES_PER_PROCESS; p++) {
                        processes[new_idx].pages[p].in_ram = false;
                        processes[new_idx].pages[p].frame_index = -1;
                    }
                    
                    for (int m = 0; m < MI_COUNT; m++) {
                        processes[new_idx].metrics[m] = 0;
                    }
                    num_processes++;
                    selected_process = num_processes - 1;
                }
                break;
            case 'd':
            case 'D':
                if (num_processes > 1) {
                    // Free timeline of the removed process (if allocated)
                    if (processes[selected_process].timeline) {
                        free(processes[selected_process].timeline);
                        processes[selected_process].timeline = NULL;
                    }

                    // Shift remaining processes left
                    for (int i = selected_process; i < num_processes - 1; i++) {
                        processes[i] = processes[i + 1];
                        processes[i].id = i + 1;
                    }

                    // Clear the now-unused last slot to avoid dangling pointers
                    processes[num_processes - 1].timeline = NULL;
                    processes[num_processes - 1].id = 0;
                    processes[num_processes - 1].arrival_time = 0;
                    processes[num_processes - 1].execution_time = 0;
                    processes[num_processes - 1].remaining_time = 0;
                    processes[num_processes - 1].priority = 0;
                    processes[num_processes - 1].deadline = 0;
                    processes[num_processes - 1].final_status = PS_PENDING;
                    for (int m = 0; m < MI_COUNT; m++) {
                        processes[num_processes - 1].metrics[m] = 0;
                    }

                    num_processes--;
                    if (selected_process >= num_processes) {
                        selected_process = num_processes - 1;
                    }
                }
                break;
            case 'm':
            case 'M':
                return;
        }
    }
}

void show_main_menu() {
    int selected = 0;
    int ch;
    int screen_height, screen_width;

    const char* menu_items[] = {
        "Start Simulation",
        "Configure Processes",
        "Set Quantum & Overhead",
        "Memory Configuration",
        "Exit"
    };
    int menu_size = 5;

    while (1) {
        clear();
        update_screen_size();
        if (is_screen_too_small()) {
            show_screen_size_error();
            napms(50);
            continue;
        }

        getmaxyx(stdscr, screen_height, screen_width);

        // Calculate dynamic box size and position
        int box_height = 20;
        int box_width = 50;
        int start_y = (screen_height - box_height) / 2;
        int start_x = (screen_width - box_width) / 2;

        // Draw frame
        attron(COLOR_PAIR(2)); // Green border
        for (int i = 0; i < box_height; i++) {
            for (int j = 0; j < box_width; j++) {
                if (i == 0 || i == box_height - 1 || j == 0 || j == box_width - 1) {
                    mvaddch(start_y + i, start_x + j, ' ');
                }
            }
        }
        attroff(COLOR_PAIR(2));

        // Title (centered)
        attron(A_BOLD | COLOR_PAIR(2));
        const char* title = "CPU SCHEDULER CONFIGURATION";
        mvaddstr(start_y, start_x + (box_width - strlen(title)) / 2, title);
        attroff(A_BOLD | COLOR_PAIR(2));

        // Menu items (centered)
        for (int i = 0; i < menu_size; i++) {
            int item_x = start_x + (box_width - strlen(menu_items[i])) / 2;

            if (i == selected) {
                attron(A_REVERSE | COLOR_PAIR(2));
            }

            mvaddstr(start_y + 5 + i, item_x, menu_items[i]);

            if (i == selected) {
                attroff(A_REVERSE | COLOR_PAIR(2));
            }
        }

        // Current configuration (centered)
        mvaddstr(start_y + 10, start_x + 2, "Current Configuration:");
        mvprintw(start_y + 11, start_x + 2, "Quantum: %d | Overhead: %d", quantum, overhead_time);
        mvprintw(start_y + 12, start_x + 2, "Total Time: %d | Processes: %d", TOTAL_TIME, num_processes);
        mvprintw(start_y + 13, start_x + 2, "Memory: %s | Policy: %s",
                 memory_enabled ? "ON" : "OFF",
                 replacement_policy == POLICY_FIFO ? "FIFO" : "LRU");

        // Controls (centered at bottom)
        mvaddstr(start_y + box_height - 3, start_x + 2, "UP/DOWN: Navigate | ENTER: Select | Q: Quit");

        refresh();

        ch = getch();
        if (ch == ERR) continue;

        switch (ch) {
            case KEY_UP:
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
                if (selected < menu_size - 1) selected++;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                switch (selected) {
                    case 0: // Start Simulation
                        // timeout(100);
                        // flushinp();
                        return;
                    case 1: // Configure Processes
                        edit_process_screen();
                        break;
                    case 2: // Set Quantum & Overhead
                        quantum = get_int_input(
                            start_y + 14, start_x + 17,
                            "Quantum", 2, 10, quantum
                        );
                        overhead_time = get_int_input(
                            start_y + 14, start_x + 17,
                            "Overhead", 1, 5, overhead_time
                        );
                        break;
                    case 3: // Memory Configuration
                        {
                            int mem_choice = 0;
                            clear();
                            mvaddstr(5, 5, "Memory Configuration");
                            mvaddstr(7, 5, "Enable Memory? (0=NO, 1=YES): ");
                            mem_choice = get_int_input(7, 37, "", 0, 1, memory_enabled ? 1 : 0);
                            memory_enabled = (mem_choice == 1);
                            
                            if (memory_enabled) {
                                int policy_choice = get_int_input(
                                    9, 5, "Policy (0=FIFO, 1=LRU)", 0, 1, 
                                    replacement_policy == POLICY_FIFO ? 0 : 1
                                );
                                replacement_policy = (policy_choice == 0) ? POLICY_FIFO : POLICY_LRU;
                            }
                        }
                        break;
                    case 4: // Exit
                        endwin();
                        exit(0);
                }
                flushinp();
                break;
            case 'q':
            case 'Q':
                endwin();
                exit(0);
        }
    }
}

void show_configuration_screen() {
    // Initialize default values if not set
    if (quantum == 0) quantum = 2;
    if (overhead_time == 0) overhead_time = 1;

    // Show main menu
    show_main_menu();

    // Re-initialize processes with new configuration
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].timeline) {
            free(processes[i].timeline);
        }
        processes[i].timeline = malloc(TOTAL_TIME * sizeof(ProcessState));
        if (processes[i].timeline == NULL) {
            // Malloc failed, exit gracefully
            endwin();
            fprintf(stderr, "Error: Failed to allocate memory for process timeline\n");
            exit(1);
        }
        processes[i].remaining_time = processes[i].execution_time;
        for (int t = 0; t < TOTAL_TIME; t++) {
            processes[i].timeline[t] = NOT_ARRIVED;
        }
    }
}
