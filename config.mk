SRC_DIR := src
INC_DIR := inc
LIB_DIR := lib

OUT_BIN_NAME := manual-hotkey

CC := gcc
LD := gcc

# compile with `-DLOG_DEBUG` if you want `util/log.c` functions to output the
# line number (of C source files) on which they occur.
CFLAGS = -I$(INC_DIR) -c -std=c99 -pedantic -pthread
LDFLAGS = -lpthread -pthread
