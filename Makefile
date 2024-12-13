# Compiler
CC = gcc

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
all: build/downloads $(TARGET)

build/downloads:
	mkdir -p downloads

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
	rm -rf downloads

# Run the program with a predefined argument

run: $(TARGET)
	$(TARGET) $(ARGS)

# Phony targets
.PHONY: all clean
