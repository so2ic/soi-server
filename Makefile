# Compiler
CC=gcc

# Compilation flags
CFLAGS= -Wall

# Source directories
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

# Find all .c files in src/ and its subdirectories
SRC=$(shell find $(SRC_DIR) -name '*.c')

# Generate corresponding .o files in obj/ preserving the subdirectory structure
OBJ=$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC:.c=.o))

# The final executable
EXEC=$(BIN_DIR)/exe

# Default target
all: $(EXEC)

# Rule to link object files and create the final executable
$(EXEC): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@

# Rule to compile .c files into .o files, ensuring correct directory creation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the build
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

