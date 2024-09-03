CC = gcc
CFLAGS = -Wall -O2

TARGET = spinning_cube

SRCS = spinning_cube.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lm

clean:
	rm -f $(TARGET)

.PHONY: clean all
