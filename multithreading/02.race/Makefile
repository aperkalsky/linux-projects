# Variables for compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = race
SRC = race.c

# Default target
all: $(TARGET)

# Compile target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean target to remove compiled binaries
clean:
	rm -f $(TARGET)
