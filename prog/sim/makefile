# CC    	= g++-11
CC    	= g++
# CFLAGS 	= -std=c++11 -O2 -g -fsanitize=address
CFLAGS 	= -std=c++11 -O2
LDFLAGS = ${CFLAGS}

PROG  	= a.out
# PROG  	= cont.out
VPATH 	= src
INCDIR 	= src inc
IPATH   = $(INCDIR:%=-I%)
OBJS  	= main.o beads.o fgnup.o bcs.o grid.o cargo.o sys.o
# OBJS  	= cont.o beads.o fgnup.o bcs.o grid.o cargo.o sys.o 

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