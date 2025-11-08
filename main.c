#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "scheduler.h"
#include "ui.h"

// Aux function for delay in milliseconds
void msleep(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

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

    initialize_globals();
    update_screen_size();

    // Initialize colors
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);    // Gray Null
    init_pair(2, COLOR_BLACK, COLOR_GREEN);    // Green Executing
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);   // Yellow Waiting
    init_pair(4, COLOR_BLACK, COLOR_RED);      // Red Overhead
    init_pair(5, COLOR_BLACK, COLOR_WHITE);    // White Contrast

    if (is_screen_too_small()) {
            show_screen_size_error();
            // Wait for user to resize or quit
            while (getch() != 'q') {
                update_screen_size(); // Check if resized
                if (!is_screen_too_small()) {
                    break; // Screen became big enough
                }
                show_screen_size_error();
                msleep(100);
            }

            // If user left with 'Q'
            if (is_screen_too_small()) {
                endwin();
                return 1;
            }
        }

    initialize_processes();

    int ch;
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
