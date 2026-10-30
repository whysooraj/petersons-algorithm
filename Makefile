CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
TARGET = peterson

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ main.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
