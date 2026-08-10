# Variables for compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread
LIBS = -lm
TARGET = conc
SRC = conc.c

# Default target
all: $(TARGET)

# Compile target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean target to remove compiled binaries
clean:
	rm -f $(TARGET)
