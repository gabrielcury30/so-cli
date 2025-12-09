#ifndef UI_H
#define UI_H

#include "globals.h"

void draw_legend(int start_y, int start_x);
void draw_gantt_chart(int start_y, int start_x);
void draw_interface();
void draw_memory_visualization(int start_y, int start_x);
bool check_screen_size();
void show_screen_size_error();

#endif
