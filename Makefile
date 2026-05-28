CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = 

SOURCES = main.c utils.c archive.c crypto.c ui.c log.c cfg.c has.c cmp.c prg.c man.c bat.c cli.c
HEADERS = platform.h odium.h log.h cfg.h has.h cmp.h prg.h man.h bat.h cli.h
OBJECTS = $(SOURCES:.c=.o)
TARGET = odium

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) *.o

.PHONY: help
help:
	@echo "Odium File Sharding Tool"
	@echo "========================"
	@echo "make          - Build the project"
	@echo "make clean    - Remove build artifacts"
	@echo ""
	@echo "Usage:"
	@echo "  ./odium                        - Interactive mode"
	@echo "  ./odium --shard <file> <key>   - Shard a file with encryption key"
	@echo "  ./odium --restore <dir> <key>  - Restore a sharded file"
