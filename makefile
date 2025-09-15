CC := gcc
AR = ar

# out directories
LIB_NAME := threadpool
BUILD_DIR := ./build
LIB_OUT_DIR := ./build/out/lib
OBJ_DIR := ./build/cache
INCLUDE_DIR := ./include
EXAMPLE_OUT_DIR := ./build/out/examples

# lib configuration
LIB_SRC_DIR := ./src
LIB_SRCS := $(shell find $(LIB_SRC_DIR) -name '*.c')
LIB_OBJS := $(LIB_SRCS:%=$(OBJ_DIR)/%.o)
LIB_DEPS := $(LIB_OBJS:%.o=%.d)
LIB_OUT_FILE := $(LIB_OUT_DIR)/lib$(LIB_NAME).a

# examples configuration
EXAMPLE_SRC_DIR := ./examples
EXAMPLE_OBJ_DIR := $(OBJ_DIR)/examples
EXAMPLE_SRCS := $(shell find $(EXAMPLE_SRC_DIR) -name '*.c')
EXAMPLE_OBJS := $(EXAMPLE_SRCS:$(EXAMPLE_SRC_DIR)/%=$(EXAMPLE_OBJ_DIR)/%.o)
EXAMPLE_SRC_DIRS := $(shell find $(EXAMPLE_SRC_DIR) -mindepth 1 -maxdepth 1 -type d )
EXAMPLE_BINS := $(EXAMPLE_SRC_DIRS:$(EXAMPLE_SRC_DIR)/%=$(EXAMPLE_OUT_DIR)/%)
EXAMPLE_DEPS := $(EXAMPLE_OBJS:%.o=%.d)

LIB_CFLAGS := -Wall -Wshadow -Wextra \
			  -I$(INCLUDE_DIR) \
			  -flto -fomit-frame-pointer -O2 -march=native \
			  $(CFLAGS) -ggdb
EXAMPLE_CFLAGS := -Wall -Wshadow -Wextra \
			  -I$(INCLUDE_DIR) \
			  -flto -fomit-frame-pointer -O2 -march=native \
			  $(CFLAGS) -ggdb

EXAMPLE_LDFLAGS := -flto -L$(LIB_OUT_DIR) -l$(LIB_NAME)

examples: $(EXAMPLE_BINS)
$(LIB_NAME): $(LIB_OUT_FILE)

-include $(LIB_DEPS)
-include $(EXAMPLE_DEPS)

$(LIB_OUT_FILE): $(LIB_OBJS)
	mkdir -p $(LIB_OUT_DIR)
	$(AR) rcs $@ $(LIB_OBJS)

$(OBJ_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(LIB_CFLAGS) -MMD -c $< -o $@

# Here I have a dependency on all OBJS but that is fine
$(EXAMPLE_OUT_DIR)/%: $(EXAMPLE_OBJS) $(LIB_OUT_FILE)
	mkdir -p $(dir $@)
	$(CC) $$(find $(EXAMPLE_OBJ_DIR)/$* -name '*.o') -o $@ $(EXAMPLE_LDFLAGS)

$(EXAMPLE_OBJ_DIR)/%.c.o: $(EXAMPLE_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(EXAMPLE_CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean examples $(LIB_NAME)
