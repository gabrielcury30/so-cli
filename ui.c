#include "ui.h"
#include "globals.h"

void draw_legend(int start_y, int start_x) {
    mvaddstr(start_y, start_x, "LEGEND:");
    mvaddstr(start_y + 2, start_x, "[  ] - Executing");
    mvaddstr(start_y + 3, start_x, "[  ] - Waiting");
    mvaddstr(start_y + 4, start_x, "[  ] - Overhead");

    // Legend colors
    attron(COLOR_PAIR(2));
    mvaddstr(start_y + 2, start_x + 1, "  ");
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvaddstr(start_y + 3, start_x + 1, "  ");
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    mvaddstr(start_y + 4, start_x + 1, "  ");
    attroff(COLOR_PAIR(4));
}

void draw_gantt_chart(int start_y, int start_x) {
    // Time header
    mvaddstr(start_y, start_x, "Time: ");
    for (int t = 0; t < TOTAL_TIME; t++) {
        if (t == current_time) {
            attron(A_BOLD | A_UNDERLINE);
            mvprintw(start_y, start_x + 6 + t * CELL_WIDTH, "%02d", t);
            attroff(A_BOLD | A_UNDERLINE);
        } else {
            mvprintw(start_y, start_x + 6 + t * CELL_WIDTH, "%02d", t);
        }
    }

    // Process rows
    for (int i = 0; i < num_processes; i++) {
        mvprintw(start_y + 2 + i, start_x, "P%d: ", processes[i].id);

        for (int t = 0; t < TOTAL_TIME; t++) {
            int color = 1;
            char cell_char = ' ';

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
                }
            }

            // Highlight current time column
            if (t == current_time) {
                attron(A_BOLD);
            }

            // Draw colored cell
            attron(COLOR_PAIR(color));
            for (int w = 0; w < CELL_WIDTH - 1; w++) {
                mvaddch(start_y + 2 + i, start_x + 6 + t * CELL_WIDTH + w, cell_char);
            }
            attroff(COLOR_PAIR(color));

            if (t == current_time) {
                attroff(A_BOLD);
            }
        }
    }
}

void show_screen_size_error() {
    clear();

    // Central error message
    int center_y = screen_height / 2;
    int center_x = screen_width / 2;

    attron(A_BOLD | COLOR_PAIR(4)); // Bold Red
    mvaddstr(center_y - 2, center_x - 12, "                        ");
    mvaddstr(center_y - 1, center_x - 12, "    SCREEN TOO SMALL!   ");
    mvaddstr(center_y,     center_x - 12, "                        ");
    attroff(A_BOLD | COLOR_PAIR(4));

    // Detailed information
    mvprintw(center_y + 2, center_x - 15, "Required minimum: 90 x 30");
    mvprintw(center_y + 3, center_x - 15, "Current size:     %2d x %2d", screen_width, screen_height);
    mvprintw(center_y + 4, center_x - 15, "Available time:   %2d units", TOTAL_TIME);

    // Instructions
    attron(A_BOLD);
    mvaddstr(center_y + 6, center_x - 10, "Please resize your terminal");
    mvaddstr(center_y + 7, center_x - 8, "or press 'Q' to quit");
    attroff(A_BOLD);

    refresh();
}

void draw_interface() {
    clear();

    if (is_screen_too_small()) {
        show_screen_size_error();
        return;
    }

    int center_x = screen_width / 2;
    int center_y = screen_height / 2;

    // Title
    attron(A_BOLD);
    mvprintw(1, 2, "CPU SCHEDULING ALGORITHM SIMULATOR - %s", algorithm_names[current_algorithm]);
    attroff(A_BOLD);

    // Gantt chart
    mvaddstr(3, 2, "GANTT CHART:");
    draw_gantt_chart(4, 2);

    // Legend
    draw_legend(4, screen_width - 20);

    // Controls
    mvaddstr(15, 2, "CONTROLS:");
    mvaddstr(16, 2, "F1-F4: Select algorithm (FIFO, SJF, EDF, RR)");
    mvaddstr(17, 2, "SPACE: Run/Reset simulation");
    mvaddstr(18, 2, "RIGHT ARROW: Advance time");
    mvaddstr(19, 2, "LEFT ARROW: Go back in time");
    mvaddstr(20, 2, "Q: Quit");

    // Process information
    mvaddstr(22, 2, "PROCESS INFORMATION:");
    for (int i = 0; i < num_processes; i++) {
        mvprintw(23 + i, 2, "P%d: Arrival=%d, Execution=%d, Deadline=%d, Remaining=%d",
                processes[i].id, processes[i].arrival_time,
                processes[i].execution_time, processes[i].deadline,
                processes[i].remaining_time);
    }

    // Current time indicator
    mvprintw(24+num_processes, 2, "Current Time: %d", current_time);

    refresh();
}

bool check_screen_size() {
    if (screen_height < 24 || screen_width < 80) {
        clear();
        mvaddstr(0, 0, "ERROR: Screen too small!");
        mvprintw(1, 0, "Required: 80x24, Current: %dx%d", screen_width, screen_height);
        refresh();
        return false;
    }
    return true;
}
