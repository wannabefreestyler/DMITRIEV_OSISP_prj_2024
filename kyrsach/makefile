CC = gcc
DEBUG_FLAGS = -g -ggdb -std=c11 -pedantic -W -Wall -Wextra
RELEASE_FLAGS = 

ifeq ($(MODE),debug)
    CFLAGS = $(DEBUG_FLAGS)
    BUILD_DIR = build/debug
else
    CFLAGS = $(RELEASE_FLAGS)
    BUILD_DIR = build/release
endif

.PHONY: all clean

all: build_dir start

build_dir:
	mkdir -p $(BUILD_DIR)

start: $(BUILD_DIR)/format_fat12

$(BUILD_DIR)/format_fat12: $(BUILD_DIR)/format_fat12.o
	$(CC) $< -o $@

$(BUILD_DIR)/format_fat12.o: src/format_fat12.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BUILD_DIR)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

release: CFLAGS += $(RELEASE_FLAGS)
release: all