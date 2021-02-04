CFLAGS = -g -Wall -Wstrict-prototypes
PROGS = make-graph print-graph floyd-serial floyd-parallel 
LDFLAGS = -lm
CC = gcc
MCC = mpicc

all: $(PROGS)

make-graph: make-graph.o graph.o

print-graph: print-graph.o graph.o

floyd-serial: floyd-serial.o graph.o

floyd-parallel: floyd-parallel.c graph.o graph_checkerboard_io.c
	$(MCC) $(LDFLAGS) -o $@ $+

clean: 
	rm -f $(PROGS) *.o core*