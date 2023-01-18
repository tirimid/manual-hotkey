SRC_DIR := src
INC_DIR := inc
LIB_DIR := lib

OUT_BIN_NAME := manual-hot-key

CC := gcc
LD := gcc

CFLAGS = -I$(INC_DIR) -c -std=c99 -pedantic
LDFLAGS =
