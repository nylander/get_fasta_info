# Makefile for get_fasta_info.c and get_fastq_info.c
# Last modified: tor sep 03, 2020  06:58
# Johan.Nylander@{nrm|nbis}.se

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

.PHONY: all clean install uninstall

SRC = get_fasta_info.c get_fastq_info.c

BIN = $(patsubst %.c,%,$(SRC))

OBJ = $(patsubst %.c,%.o,$(SRC))

LDLIBS = -lm -lz

CFLAGS = -Wall -O3 -funroll-loops

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ)

install: $(BIN)
	install $(BIN) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm $(foreach b,$(BIN), $(DESTDIR)$(PREFIX)/bin/$(b) )

