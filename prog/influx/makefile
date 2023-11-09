# CC    	= g++-11
CC    	= g++
# CC    	= icc

CFLAGS 	= -std=c++11 -O2 -DDEBUG
LDFLAGS = ${CFLAGS}

PROG  	= a.out
VPATH 	= src ../sim/src ../sim/inc
INCDIR 	= src ../sim/src ../sim/inc
IPATH   = $(INCDIR:%=-I%)
OBJS  	= main.o beads.o fgnup.o bcs.o grid.o cargo.o sys.o param.o sim.o

all 	: ${PROG} cxn.out

${PROG}	: ${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}

cxn.out : cxn.cpp
	${CC} $< -o $@ ${LDFLAGS}

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) -c $< ${IPATH} ${CFLAGS}


clean 	: 
	rm *.o

vclean 	: 
	rm *.vtk