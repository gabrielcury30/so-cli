#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/scheduler.h"
#include "../include/ui.h"
#include "../include/config_ui.h"
#include "../include/screen_utils.h"

void cleanup() {
    for (int i = 0; i < num_processes; i++) {
        free(processes[i].timeline);
        free(processes[i].page_fault_occurred);
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
    init_pair(5, COLOR_BLACK, COLOR_WHITE);    // Contrast/Deadline White
    init_pair(6, COLOR_BLACK, COLOR_CYAN);     // Page Fault Cyan

    show_configuration_screen();

    if (num_processes == 0) {
        initialize_default_processes();
    }

    int running = 0;
    bool follow_mode = true;  // When true, memory frame follows current_time

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
                    // Sync memory frame with current time
                    memory_animation_frame = current_time;
                }
                break;

            case KEY_RIGHT:
                if (current_time < TOTAL_TIME - 1) {
                    current_time++;
                    // Automatic Scroll when current_time is off screen
                    if (current_time >= time_offset + visible_columns) {
                        time_offset = current_time - visible_columns + 1;
                    }
                    // Update memory frame if in follow mode
                    if (follow_mode) {
                        memory_animation_frame = current_time;
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
                    // Update memory frame if in follow mode
                    if (follow_mode) {
                        memory_animation_frame = current_time;
                    }
                }
                break;

            case '<':  // Navigate memory animation backward
            case ',':
                if (memory_animation_frame > 0) {
                    memory_animation_frame--;
                    follow_mode = false;
                }
                break;

            case '>':  // Navigate memory animation forward
            case '.':
                if (memory_animation_frame < TOTAL_TIME - 1) {
                    memory_animation_frame++;
                    follow_mode = false;
                }
                break;

            case '/':  // Toggle follow mode
                follow_mode = !follow_mode;
                if (follow_mode) {
                    memory_animation_frame = current_time;
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

            case 'm':
                reset_simulation();
                show_configuration_screen();
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

            case KEY_F(5):
                current_algorithm = 4;
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
