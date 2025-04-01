# Build Chat Application

# Compiler and tools
CC = gcc
CFLAGS = -lpthread -Wall -g -I$(INC_DIR)

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
EXEC = $(BIN_DIR)/chat_app

# Build target
all: $(EXEC)

# Rule to compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the final executable
$(EXEC): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@

# Run Valgrind to check for memory leak
valgrind-check: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(EXEC) 8000

# Check for memory leaks using Valgrind and log output
valgrind-log: $(EXEC)
	mkdir -p $(LOG_DIR)
	nohup valgrind --leak-check=full --show-leak-kinds=all ./bin/chat_app 8000 > log/valgrind.log 2>&1 &

# Clean all compiled files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	rm -rf $(LOG_DIR)

.PHONY: all clean valgrind-check valgrind-log
