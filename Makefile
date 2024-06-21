SRC_DIR	:= src
OBJ_DIR := build
BIN_DIR	:= bin

SATSOLVER	:= SatSolver.cpp
CUBICGRAPH	:= CubicGraph.cpp

EXE	:= $(BIN_DIR)/med
SRC := $(wildcard $(SRC_DIR)/*.cpp)
ifndef NOSAT
SRC := $(filter-out $(SRC_DIR)/$(CUBICGRAPH), $(SRC))
else
SRC := $(filter-out $(SRC_DIR)/$(SATSOLVER), $(SRC))
endif
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC			:= g++
CFLAGS		:= -Wall -std=c++17
CPPFLAGS	:= -Iinclude -MMD -MP
LDLIBS		:= -lcryptominisat5


.PHONY: all clean

all: $(EXE)
	@echo BUILD SUCCEEDED

ifndef NOSAT
$(EXE): $(OBJ) | $(BIN_DIR)
	@echo LINKING..... $(CC) "$^" $(LDLIBS) -o "$@"
	@$(CC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo COMPILING... $(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@" -DSAT
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -DSAT

else
$(EXE): $(OBJ) | $(BIN_DIR)
	@echo LINKING..... $(CC) "$^" -o "$@"
	@$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo COMPILING... $(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

endif

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo CLEANING UP
	@rm -f -r $(OBJ_DIR)
	@rm -f -r $(BIN_DIR)
