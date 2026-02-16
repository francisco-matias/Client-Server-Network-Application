CC := gcc
CFLAGS := -Wall

FILES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h)
OBJECTS := $(addprefix bin/,$(notdir $(FILES:.c=.o)))

VPATH := src

all: bin cot

ring: $(OBJECTS)
	$(CC) -o cot $(OBJECTS)

bin/%.o: %.c | $(HEADERS)
	$(CC) $(CFLAGS) -Isrc -c $< -o $@

bin:
	mkdir -p bin

clean:
	rm -rf bin/* cot

