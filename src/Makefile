# Makefile for get_fasta_info.c and get_fastq_info.c
# Last modified: tor jan 30, 2020  11:27
# Johan.Nylander@{nrm|nbis}.se

SRC = get_fasta_info.c get_fastq_info.c

BIN = $(patsubst %.c,%,$(SRC))

OBJ = $(patsubst %.c,%.o,$(SRC))

LDLIBS = -lm -lz

CFLAGS = -Wall -O3 -funroll-loops

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean

