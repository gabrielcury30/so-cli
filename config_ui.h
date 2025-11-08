#ifndef CONFIG_UI_H
#define CONFIG_UI_H

#include "globals.h"

// Function declarations
void show_configuration_screen();
int get_int_input(int y, int x, const char* prompt, int min_val, int max_val, int default_val);
void edit_process_screen();
void show_main_menu();

#endif
