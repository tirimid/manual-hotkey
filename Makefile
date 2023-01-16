include config.mk

SOURCES := $(shell find $(SRC_DIR) -name *.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(LIB_DIR)/%.o, $(SOURCES))

all: $(OUT_BIN_NAME)

clean:
	rm -f $(OUT_BIN_NAME)
	rm -rf $(LIB_DIR)/*

$(OUT_BIN_NAME): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(LIB_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $@
	rmdir $@
	$(CC) $(CFLAGS) -o $@ $<
