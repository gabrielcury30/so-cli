#include <ncurses.h>
#include <time.h>
#include "screen_utils.h"
#include "globals.h"

void update_screen_size() {
    getmaxyx(stdscr, screen_height, screen_width);
}

bool is_screen_too_small() {
    return (screen_width < 125 || screen_height < 45 || TOTAL_TIME < 15);
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
    mvprintw(center_y + 2, center_x - 15, "Required minimum: 125 x 45");
    mvprintw(center_y + 3, center_x - 15, "Current size:     %2d x %2d", screen_width, screen_height);

    // Instructions
    attron(A_BOLD);
    mvaddstr(center_y + 6, center_x - 10, "Please resize your terminal");
    mvaddstr(center_y + 7, center_x - 8, "or press 'Q' to quit");
    attroff(A_BOLD);

    refresh();
}

// Aux function for delay in milliseconds
void msleep(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
