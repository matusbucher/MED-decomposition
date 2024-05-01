SRC_DIR	:= src
OBJ_DIR := build
BIN_DIR	:= bin

EXE	:= $(BIN_DIR)/med
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC			:= g++
CFLAGS		:= -Wall -std=c++17
CPPFLAGS	:= -Iinclude -MMD -MP
LDFLAGS		:= #-Llib
LDLIBS		:= #-lminisat

.PHONY: all clean

all: $(EXE)
	@echo BUILD SUCCEEDED

$(EXE): $(OBJ) | $(BIN_DIR)
	@echo LINKING..... $(CC) $(LDFLAGS) "$^" $(LDLIBS) -o "$@"
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo COMPILING... $(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo CLEANING UP
	@rm -f -r $(OBJ_DIR)
	@rm -f -r $(BIN_DIR)

-include $(OBJ:.o=.d)

debug:
	echo Hello World!
