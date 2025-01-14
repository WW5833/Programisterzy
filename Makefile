# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference -g3 -O0

RM := del

SRC_PATH := ./src/
OBJ_PATH := ./obj/
BIN_PATH := ./bin/

# Executable name
TARGET = $(BIN_PATH)Debug/Programisterzy

# Default target
all: $(TARGET)

.PHONY: all clean

SRC = $(wildcard $(SRC_PATH)**/*.c) $(wildcard $(SRC_PATH)*.c)

OBJ = $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRC))

# Build .o first
$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@echo [CC] $<
	@$(CC) $(C_FLAGS) -o $@ -c $< $(INC_PATH)                  

# Build final Binary
$(TARGET):  $(OBJ)
	@echo [INFO] Creating Binary Executable [$(TARGET)]
	@$(CC) -o $@ $^ $(LINKFLAGS)

# Clean all the object files and the binary
clean:   
	@echo "[Cleaning]"
	@cd $(OBJ_PATH) && $(RM) /s /q *
	@cd $(BIN_PATH) && $(RM) /s /q *