#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include "globals.h"
#include "scheduler.h"
#include "ui.h"
#include "config_ui.h"
#include "screen_utils.h"

void cleanup() {
    for (int i = 0; i < num_processes; i++) {
        free(processes[i].timeline);
    }
}

int main() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);
    curs_set(0);
    int ch;

    initialize_globals();
    update_screen_size();

    // Initialize colors
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);    // Null Gray
    init_pair(2, COLOR_BLACK, COLOR_GREEN);    // Executing Green
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);   // Waiting Yellow
    init_pair(4, COLOR_BLACK, COLOR_RED);      // Overhead Red
    init_pair(5, COLOR_BLACK, COLOR_WHITE);    // Contrast White
    init_pair(6, COLOR_BLACK, COLOR_WHITE);     // Deadline Missed White

    show_configuration_screen();

    if (num_processes == 0) {
        initialize_default_processes();
    }

    int running = 0;
    while ((ch = getch()) != 'q') {
        update_screen_size();

        if (is_screen_too_small()) {
            show_screen_size_error();
            continue;
        }

        switch (ch) {
            case ' ':
                running = !running;
                if (running) {
                    reset_simulation();
                    run_current_algorithm();
                }
                break;

            case KEY_RIGHT:
                if (current_time < TOTAL_TIME - 1) {
                    current_time++;
                    // Automatic Scroll when current_time is off screen
                    if (current_time >= time_offset + visible_columns) {
                        time_offset = current_time - visible_columns + 1;
                    }
                }
                break;

            case KEY_LEFT:
                if (current_time > 0) {
                    current_time--;
                    // Automatic Scroll when current_time is off screen
                    if (current_time < time_offset) {
                        time_offset = current_time;
                    }
                }
                break;

            case 'd': // Right Scroll
            case 'D':
                if (time_offset + visible_columns < TOTAL_TIME) {
                    time_offset++;
                }
                break;

            case 'a': // Left Scroll
            case 'A':
                if (time_offset > 0) {
                    time_offset--;
                }
                break;

            case 'h': // Home - go to beginning
                time_offset = 0;
                break;

            case 'e': // End - go to end
                time_offset = TOTAL_TIME - visible_columns;
                if (time_offset < 0) time_offset = 0;
                break;

            case KEY_F(1):
                current_algorithm = 0;
                reset_simulation();
                running = 0;
                break;

            case KEY_F(2):
                current_algorithm = 1;
                reset_simulation();
                running = 0;
                break;

            case KEY_F(3):
                current_algorithm = 2;
                reset_simulation();
                running = 0;
                break;

            case KEY_F(4):
                current_algorithm = 3;
                reset_simulation();
                running = 0;
                break;
        }

        draw_interface();
        msleep(10);
    }

    cleanup();
    endwin();
    return 0;
}
