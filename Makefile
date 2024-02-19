CC			:= g++
CFLAGS		:= -Wall -std=c++17
CPPFLAGS	:= -Iinclude -MMD -MP
LDFLAGS		:= -Llib
LDLIBS		:= -lm

SRC_DIR		:= src
HEAD_DIR	:= include
OBJ_DIR 	:= build
BIN_DIR		:= bin

EXECUTABLE	:= $(BIN_DIR)/main
SOURCES		:= $(wildcard $(SRC_DIR)/*.cpp)
INCLUDES	:= $(wildcard $(HEAD_DIR)/*.h)
OBJECTS		:= $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	@echo LINKING... $(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | makedirs
	@echo COMPILING... $(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


makedirs:
	@if not exist "$(OBJ_DIR)" mkdir $(OBJ_DIR)
	@if not exist "$(BIN_DIR)" mkdir $(BIN_DIR)

clean:
	@echo CLEANING UP
	@if exist "$(OBJ_DIR)" rmdir /s /q $(OBJ_DIR)
	@if exist "$(BIN_DIR)" rmdir /s /q $(BIN_DIR)
