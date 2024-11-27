# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Target executable
TARGET = $(BUILD_DIR)/download

# Default rule: build the executable
all: $(TARGET)

# Build the executable by linking object files
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the build directory
clean:
	rm -rf $(BUILD_DIR)

# Run the program with a predefined argument
run: $(TARGET)
	$(TARGET) ftp://ftp.up.pt/pub/gnu/emacs/elisp-manual-21-2.8.tar.gz


runa: $(TARGET)
	$(TARGET) $(ARGS)

# Phony targets
.PHONY: all clean
