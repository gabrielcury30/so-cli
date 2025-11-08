#ifndef SCREEN_UTILS_H
#define SCREEN_UTILS_H

void update_screen_size();
bool is_screen_too_small();
void show_screen_size_error();
int wait_for_adequate_screen();
void msleep(int milliseconds);

#endif
