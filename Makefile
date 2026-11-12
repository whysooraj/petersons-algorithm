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

serve:
	@echo "Serving at http://localhost:8000"
	python3 -m http.server 8000

.PHONY: all run clean serve
