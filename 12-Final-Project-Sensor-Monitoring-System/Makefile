# Build Senior-Monitoring-System

# Compiler and tools
CC = gcc
#CFLAGS = -lpthread -Wall -g -I$(INC_DIR)
CFLAGS = -Wall -Wextra -g -std=c11 -D_POSIX_C_SOURCE=200809L -I$(INC_DIR)

# Directory structure
CUR_DIR := .
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin
LOG_DIR := $(CUR_DIR)/log

# Find all .c files in src/
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Convert .c filenames to .o for obj/
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Ensure necessary directories exist before compiling
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR)) 

# Define output file names
TARGET = $(BIN_DIR)/sensor_gateway

# Build target
all: $(TARGET)

# Rule to compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the final executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@

# Run Valgrind to check for memory leak
valgrind-check: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET) 8000

# Check for memory leaks using Valgrind and log output
valgrind-log: $(TARGET)
	mkdir -p $(LOG_DIR)
	nohup valgrind --leak-check=full --show-leak-kinds=all ./bin/$(TARGET) 8000 > log/valgrind.log 2>&1 &

# Clean all compiled files
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(LOG_DIR)
	@rm -rf logfifo
	
.PHONY: all clean valgrind-check valgrind-log
