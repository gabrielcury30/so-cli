#include "ui.h"
#include "globals.h"

void draw_legend(int start_y, int start_x) {
    mvaddstr(start_y, start_x, "LEGEND:");
    mvaddstr(start_y + 2, start_x, "[  ] - Executing");
    mvaddstr(start_y + 3, start_x, "[  ] - Waiting");
    mvaddstr(start_y + 4, start_x, "[  ] - Overhead");
    mvaddstr(start_y + 5, start_x, "[  ] - Deadline Missed");
    mvaddstr(start_y + 6, start_x, "[ | ] - Absolute Deadline");

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

    attron(COLOR_PAIR(6));
    mvaddstr(start_y + 5, start_x + 1, "  ");
    attroff(COLOR_PAIR(6));
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
                    case DEADLINE_MISSED:
                        color = 6; // White Bold
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

            // Aqui considera o deadline absoluto de cada processo pelo calculo em abs_dead,
            // Desenha um marcador vertical imediatamente a direita do quadrado e rotula com o id do processo.
            int abs_dead = (processes[i].arrival_time + processes[i].deadline) - 1;
            // A condicao garante que o marcador so apareca no algoritmo EDF
            if (t == abs_dead && current_algorithm == 2) {
                int marker_col = screen_col + (CELL_WIDTH - 1); // Coluna imediatamente após o quadrado
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

    // Metrics table (shown after simulation finishes / when computed)
    int metrics_start_y = 25 + num_processes;
    if (metrics_computed) {
        // Title
        attron(A_BOLD);
        mvprintw(metrics_start_y, 2, "METRICAS:");
        attroff(A_BOLD);

        // Decorative separator under title
        mvaddstr(metrics_start_y + 1, 2, "------------------------------------------------------------------------------------");

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
            "chegada", "execucao", "deadline", "prioridade",
            "inicio(s)", "termino", "espera", "turnaround", "deadline_ok"
        };

        for (int r = 0; r < MI_COUNT; r++) {
            // Skip deadline_ok row if not EDF algorithm
            if (r == MI_DEADLINE_OK && current_algorithm != 2) {
                continue;
            }

            int row_y = first_metric_row + r;

            // Left vertical border and metric name
            mvaddch(row_y, table_start_x, ACS_VLINE);
            mvprintw(row_y, table_start_x + 1, "%-*s", left_w - 1, labels[r]);
            mvaddch(row_y, table_start_x + left_w, ACS_VLINE);

            // Cells per process
            for (int c = 0; c < max_cols; c++) {
                int col_start = table_start_x + left_w + c * col_w;
                // content area is col_start +1 .. col_start + col_w -1
                if (c < num_processes) {
                    int *m = processes[c].metrics;
                    if (r == MI_START) {
                        if (m[MI_START] < 0) mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, "");
                        else mvprintw(row_y, col_start + 1, "%-*d", col_w - 1, m[MI_START]);
                    } else if (r == MI_END) {
                        if (m[MI_END] <= 0) mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, "");
                        else mvprintw(row_y, col_start + 1, "%-*d", col_w - 1, m[MI_END]);
                    } else if (r == MI_DEADLINE_OK) {
                        mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, m[MI_DEADLINE_OK] ? "SIM" : "NAO");
                    } else {
                        mvprintw(row_y, col_start + 1, "%-*d", col_w - 1, m[r]);
                    }
                } else {
                    mvprintw(row_y, col_start + 1, "%-*s", col_w - 1, " ");
                }
                // right vertical border of the cell
                mvaddch(row_y, col_start + col_w, ACS_VLINE);
            }

            // draw separator line below this row
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

        // Summary statistics (quantitative summary)
        int summary_y = first_metric_row + MI_COUNT + 4;
        attron(A_BOLD);
        mvaddstr(summary_y, 2, "RESUMO QUANTITATIVO:");
        attroff(A_BOLD);

        mvprintw(summary_y + 1, 2, "Media de Chegada: %.2f  |  Media de Execucao: %.2f  |  Media de Espera: %.2f  |  Media de Turnaround: %.2f",
                 summary_stats.avg_arrival, summary_stats.avg_execution, summary_stats.avg_wait, summary_stats.avg_turnaround);
        mvprintw(summary_y + 2, 2, "Throughput: %.4f proc/unid_tempo  |  Ociosidade: %.2f%%  |  Trocas de Contexto: %d",
                 summary_stats.throughput, summary_stats.idle_percentage, summary_stats.context_switches);
    } else {
        // Current time indicator
        mvprintw(metrics_start_y, 2, "Current Time: %d", current_time);
    }

    refresh();
}
