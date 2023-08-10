CC = gcc
CFLAGS = -g -Wall -Wvla -Werror -Wno-error=unused-variable
TARGET = simulator

DEPS = $(wildcard *.h)
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)
