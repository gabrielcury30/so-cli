CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L
LIBS = -lncurses -lm
SOURCES = main.c scheduler.c ui.c globals.c config_ui.c screen_utils.c memory.c metrics_utils.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = scheduler

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean
