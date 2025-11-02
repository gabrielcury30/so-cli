#include "ui.h"

void draw_legend(int start_y, int start_x) {
    mvaddstr(start_y, start_x, "LEGEND:");
    mvaddstr(start_y + 1, start_x, "[  ] - Not arrived");
    mvaddstr(start_y + 2, start_x, "[  ] - Executing");
    mvaddstr(start_y + 3, start_x, "[  ] - Overhead");
    mvaddstr(start_y + 4, start_x, "[  ] - Waiting");
    mvaddstr(start_y + 5, start_x, "[  ] - Completed");

    // Legend colors
    attron(COLOR_PAIR(1));
    mvaddstr(start_y + 1, start_x + 1, "  ");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvaddstr(start_y + 2, start_x + 1, "  ");
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvaddstr(start_y + 3, start_x + 1, "  ");
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    mvaddstr(start_y + 4, start_x + 1, "  ");
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(5));
    mvaddstr(start_y + 5, start_x + 1, "  ");
    attroff(COLOR_PAIR(5));
}

void draw_gantt_chart(int start_y, int start_x) {
    // Time header
    mvaddstr(start_y, start_x, "Time: ");
    for (int t = 0; t < TOTAL_TIME; t++) {
        mvprintw(start_y, start_x + 6 + t * CELL_WIDTH, "%2d", t);
    }

    // Process rows
    for (int i = 0; i < num_processes; i++) {
        mvprintw(start_y + 2 + i, start_x, "P%d: ", processes[i].id);

        for (int t = 0; t < TOTAL_TIME; t++) {
            int color = 1;

            switch (processes[i].timeline[t]) {
                case NOT_ARRIVED:
                    color = 1; // Gray
                    break;
                case EXECUTING:
                    color = 2; // Green
                    break;
                case OVERHEAD:
                    color = 3; // Yellow
                    break;
                case WAITING:
                    color = 4; // Blue
                    break;
                case COMPLETED:
                    color = 5; // Red
                    break;
            }

            // Draw colored cell
            attron(COLOR_PAIR(color));
            for (int w = 0; w < CELL_WIDTH - 1; w++) {
                mvaddch(start_y + 2 + i, start_x + 6 + t * CELL_WIDTH + w, ' ');
            }
            attroff(COLOR_PAIR(color));
        }
    }
}

void draw_interface() {
    clear();

    // Title
    attron(A_BOLD);
    mvprintw(1, 2, "CPU SCHEDULING ALGORITHM SIMULATOR - %s", algorithm_names[current_algorithm]);
    attroff(A_BOLD);

    // Gantt chart
    mvaddstr(3, 2, "GANTT CHART:");
    draw_gantt_chart(4, 2);

    // Legend
    draw_legend(4, 65);

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
    mvprintw(27, 2, "Current Time: %d", current_time);

    refresh();
}
