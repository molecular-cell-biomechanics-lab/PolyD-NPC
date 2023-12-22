# CC    	= g++-11
CC    	= g++

CFLAGS 	= -std=c++11 -O2 
LDFLAGS = ${CFLAGS}

PROG  	= a.out
VPATH 	= ../sim/src ../sim/inc ../ffs/src
INCDIR 	= ../sim/src ../sim/inc ../ffs/src
IPATH   = $(INCDIR:%=-I%)
OBJS  	= main.o beads.o fgnup.o bcs.o grid.o cargo.o sys.o param.o

all 	: ${PROG}

${PROG}	: ${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) -c $< ${IPATH} ${CFLAGS}


clean 	: 
	rm *.o

vclean 	: 
	rm *.vtk