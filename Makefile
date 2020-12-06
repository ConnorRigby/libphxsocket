BUILD=_build
PREFIX=bin

SRC=$(wildcard src/*.c)
OBJ = $(SRC:src/%.c=$(BUILD)/%.o)

OBJ+=_build/cJSON.o

CFLAGS=-g\
       -Iinclude \
			 -Ithirdparty/cJSON

LDFLAGS=-lwebsockets -lcap

.PHONY: all clean format

all: $(PREFIX) $(BUILD) $(PREFIX)/test

format: all
	astyle -n -r 'src/*.c' 'include/*.h'

$(BUILD)/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(PREFIX) $(BUILD):
	mkdir -p $@

$(PREFIX)/test: $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(PREFIX)/test $(BUILD)/*.o
