# Build exercise 5
# Makefile for building the 'zombie' and 'orphan' executables

# Compiler and tools
CC = gcc

# Directory structure
CUR_DIR := .
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin

# Create necessary directories
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Source files
ZOMBIE_SRC := $(SRC_DIR)/zombie.c
ORPHAN_SRC := $(SRC_DIR)/orphan.c

# Object files
ZOMBIE_OBJ := $(OBJ_DIR)/zombie.o
ORPHAN_OBJ := $(OBJ_DIR)/orphan.o

# Output executables
ZOMBIE_EXEC := $(BIN_DIR)/zombie
ORPHAN_EXEC := $(BIN_DIR)/orphan

# Default target builds both executables
all: $(ZOMBIE_EXEC) $(ORPHAN_EXEC)

# Compile zombie.c to object file in obj directory
$(OBJ_DIR)/zombie.o: $(SRC_DIR)/zombie.c
	$(CC) -c $< -o $@

# Compile orphan.c to object file in obj directory
$(OBJ_DIR)/orphan.o: $(SRC_DIR)/orphan.c
	$(CC) -c $< -o $@

# Link the zombie executable from its object file
$(ZOMBIE_EXEC): $(OBJ_DIR)/zombie.o
	$(CC) $(OBJ_DIR)/zombie.o -o $(ZOMBIE_EXEC)

# Link the orphan executable from its object file
$(ORPHAN_EXEC): $(OBJ_DIR)/orphan.o
	$(CC) $(OBJ_DIR)/orphan.o -o $(ORPHAN_EXEC)

# Clean target to remove the obj and bin directories
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

