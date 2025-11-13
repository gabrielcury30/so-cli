#include "ui.h"
#include "globals.h"

void draw_legend(int start_y, int start_x) {
    mvaddstr(start_y, start_x, "LEGEND:");
    mvaddstr(start_y + 2, start_x, "[  ] - Executing");
    mvaddstr(start_y + 3, start_x, "[  ] - Waiting");
    mvaddstr(start_y + 4, start_x, "[  ] - Overhead");
    mvaddstr(start_y + 5, start_x, "[  ] - Deadline Missed");

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

    attron(COLOR_PAIR(6) | A_BOLD);
    mvaddstr(start_y + 5, start_x + 1, "  ");
    attroff(COLOR_PAIR(6) | A_BOLD);
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
                mvaddch(start_y + 2 + i, screen_col + w, cell_char);
            }
            attroff(COLOR_PAIR(color));

            if (t == current_time) {
                attroff(A_BOLD);
            }
        }
    }

    // Show scroll information
    mvprintw(start_y + num_processes + 3, start_x,
             "Showing time %d to %d (Total: %d)",
             start_time, end_time - 1, TOTAL_TIME);
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
    mvprintw(center_y + 2, center_x - 15, "Required minimum: 106 x 33");
    mvprintw(center_y + 3, center_x - 15, "Current size:     %2d x %2d", screen_width, screen_height);

    // Instructions
    attron(A_BOLD);
    mvaddstr(center_y + 6, center_x - 10, "Please resize your terminal");
    mvaddstr(center_y + 7, center_x - 8, "or press 'Q' to quit");
    attroff(A_BOLD);

    refresh();
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
    mvaddstr(16, 2, "F1-F4: Select algorithm (FIFO, SJF, EDF, RR)");
    mvaddstr(17, 2, "SPACE: Run/Reset simulation");
    mvaddstr(18, 2, "RIGHT|RIGHT ARROW: Advance or go back in time");
    mvaddstr(19, 2, "A|D: Scroll chart left and right");
    mvaddstr(20, 2, "H: Go to start, E: Go to end");
    mvaddstr(21, 2, "Q: Quit");

    // Process information
    mvaddstr(23, 2, "PROCESS INFORMATION:");
    for (int i = 0; i < num_processes; i++) {
        mvprintw(24 + i, 2, "P%d: Arrival=%d, Execution=%d, Deadline=%d",
                processes[i].id, processes[i].arrival_time,
                processes[i].execution_time, processes[i].deadline);
    }

    // Current time indicator
    mvprintw(25 + num_processes, 2, "Current Time: %d", current_time);

    refresh();
}
