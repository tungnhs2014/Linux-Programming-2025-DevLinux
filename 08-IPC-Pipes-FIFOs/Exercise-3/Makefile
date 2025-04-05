# Build exercise 3

# Compiler and tools
CC = gcc

# Directory structure
CUR_DIR := .
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin

# Find all .c files in src/
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Convert .c filenames to .o for obj/
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Ensure necessary directories exist before compiling
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Define output file names
EXEC = $(BIN_DIR)/exam

# Build target
all: $(EXEC)

# Rule to compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@

# Rule to link the final executable
$(EXEC): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@

# Clean all compiled files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
