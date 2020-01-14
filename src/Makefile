# Makefile for get_fasta_info.c
# Last modified: Tue Jan 14, 2020  07:07PM
# Johan.Nylander@{nrm|nbis}.se

SRC = get_fasta_info.c get_fastq_info.c

BIN = $(patsubst %.c,%,$(SRC))

OBJ = $(patsubst %.c,%.o,$(SRC))

LDLIBS = -lm -lz

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean

