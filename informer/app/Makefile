# Compiler and Flags
CC = gcc
# CXX = g++ # Use this if compiling C++ instead of C

# Compiler flags: 
# -Wall enables all warnings
# -g includes debugging symbols
CFLAGS = -Wall -g
CXXFLAGS = -Wall -g

# Linker flags (e.g., -lm for math library, -lpthread)
LDFLAGS = 

# Target executable name
TARGET = cpu_temp_reader

# Find all source files in the current directory
# To specify a specific list of files, use: SRCS = main.c utils.c
SRCS = $(wildcard *.c)

# Generate object file names from source files
OBJS = $(SRCS:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: clean

# Remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
