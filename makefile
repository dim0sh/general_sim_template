CC := gcc

EXEC := general_sim_template

BUILD_DIR := ./build

SRC_DIRS := ./src
LIB_DIR := ./lib
RAYLIB_DIR := $(LIB_DIR)/raylib
TIMING_DIR := $(LIB_DIR)/timer
# DYNARRAY_DIR := $(LIB_DIR)/dynarray
MICROUI_DIR := $(LIB_DIR)/microui

TARGET := $(BUILD_DIR)/$(EXEC)

# SRCS := $(wildcard $(DYNARRAY_DIR)/*.c) $(wildcard $(MICROUI_DIR)/*.c) $(wildcard $(SRC_DIRS)/*.c)
SRCS := $(wildcard $(MICROUI_DIR)/*.c) $(wildcard $(SRC_DIRS)/*.c)
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
CFLAGS := -std=c99 -Wall -Wextra -Wformat -Wnull-dereference -Winfinite-recursion -Wuse-after-free -Wuninitialized -Wunused  -Wduplicated-cond -Wfree-nonheap-object -Wunsafe-loop-optimizations -Wmissing-field-initializers -Winline -Wreturn-local-addr -Wpedantic -O3 -flto 
# -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wmissing-noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=noreturn -Wconversion -Wfloat-equal -Wpointer-arith 
INCLUDES := -I$(LIB_DIR) -I$(SRC_DIRS)
LDINCLUDES :=  -L$(RAYLIB_DIR) -L$(TIMING_DIR)
LDLIBS := -lraylib -ltimer -lm

DLLS := $(RAYLIB_DIR)/raylib.dll $(TIMING_DIR)/timer.dll

$(TARGET): $(OBJ)
	@cp $(DLLS)  $(BUILD_DIR)
	$(CC) -o $@ $(OBJ) $(INCLUDES) $(LDINCLUDES) $(LDLIBS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)