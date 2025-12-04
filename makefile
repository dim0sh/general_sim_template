CC := gcc

EXEC := general_sim_template

BUILD_DIR := ./build

SRC_DIRS := ./src
LIB_DIR := ./lib
RAYLIB_DIR := $(LIB_DIR)/raylib
TIMING_DIR := $(LIB_DIR)/timer
DYNARRAY_DIR := $(LIB_DIR)/dynarray
MICROUI_DIR := $(LIB_DIR)/microui

TARGET := $(BUILD_DIR)/$(EXEC)
SRCS := $(DYNARRAY_DIR)/dynarray $(MICROUI_DIR)/microui 
SRCS := $(SRCS) $(wildcard src/*.c)
OBJ := $(SRCS:%=$(BUILD_DIR)/%.o)

CFLAGS := -std=c99 -Wall -Wextra -O3 -flto 
INCLUDES := -I.$(LIB_DIR) -I.$(SRC_DIRS)
LDINCLUDES :=  -L$(RAYLIB_DIR) -L$(TIMING_DIR)
LDLIBS := -lraylib -ltimer -lm

DLLS := $(RAYLIB_DIR)/raylib.dll $(TIMING_DIR)/timer.dll

$(TARGET): $(OBJ)
	@cp $(DLLS)  $(BUILD_DIR)
	$(CC) -o $@ $(OBJ) $(INCLUDES) $(LDINCLUDES) $(LDLIBS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)