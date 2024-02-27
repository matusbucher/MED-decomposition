SRC_DIR	:= src
OBJ_DIR := build
BIN_DIR	:= bin

EXE	:= $(BIN_DIR)/med
SRC	:= $(wildcard $(SRC_DIR)/*.cpp)
OBJ	:= $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC			:= g++
CFLAGS		:= -Wall -std=c++17
CPPFLAGS	:= -Iinclude -MMD -MP
LDFLAGS		:= -Llib -Wl,-rpath=lib
LDLIBS		:=

.PHONY: all clean

all: $(EXE)
	@echo BUILD SUCCEEDED

$(EXE): $(OBJ) | $(BIN_DIR)
	@echo LINKING..... $(CC) $(CPPFLAGS) "$^" -o "$@" $(LDFLAGS) $(LDLIBS)
	@$(CC) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo COMPILING... $(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir $(OBJ_DIR)
	@if not exist "$(BIN_DIR)" mkdir $(BIN_DIR)

clean:
	@echo CLEANING UP
	@if exist "$(OBJ_DIR)" rmdir /s /q $(OBJ_DIR)
	@if exist "$(BIN_DIR)" rmdir /s /q $(BIN_DIR)
