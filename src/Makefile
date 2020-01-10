# Makefile for get_fasta_info.c
# Last modified: fre jan 10, 2020  04:01
# Johan.Nylander@{nrm|nbis}.se

CC     = gcc
GDB    = gdb
CFLAGS = -Wall -O3
LDLIBS = -lm -lz
SRC    = get_fasta_info.c
OBJ    = $(SRC:%.c=%.o)
PROG   = get_fasta_info
RM     = rm -f
INFILE1 = ../dat/fasta.fasta
INFILE2 = ../dat/fasta.fasta.gz

.PHONY: all debug test clean

all: $(PROG)

$(OBJ): $(SRC)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDLIBS)

debug: CFLAGS += -g

debug: $(PROG)
	$(GDB) ./$(PROG)

test: $(PROG) $(INFILE1) $(INFILE2)
	./$(PROG) $(INFILE1) $(INFILE2)

clean:
	@ $(RM) $(OBJ) $(PROG)


