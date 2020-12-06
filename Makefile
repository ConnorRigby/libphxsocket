BUILD=_build
PREFIX=bin

SRC=$(wildcard src/*.c)
OBJ = $(SRC:src/%.c=$(BUILD)/%.o)

CFLAGS=-Iinclude \
			 -Ithirdparty

LDFLAGS=-lwebsockets -lcap

.PHONY: all clean

all: $(PREFIX) $(BUILD) $(PREFIX)/test

$(BUILD)/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(PREFIX) $(BUILD):
	mkdir -p $@

$(PREFIX)/test: $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(PREFIX)/test $(BUILD)/*.o
