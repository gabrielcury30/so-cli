#include <ncurses.h>
#include <time.h>
#include "screen_utils.h"
#include "globals.h"

// Aux function for delay in milliseconds
void msleep(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
