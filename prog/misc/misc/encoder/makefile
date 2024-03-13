# CC    	= g++-11
CC    	= g++
# CC    	= icc
# OPT 	= -std=c++11 -O2 -pg
OPT 	= -std=c++11 
# OPT 	= -std=c++11 -O2 -g
# OPT 	= -std=c++11 -O3 -flto -pg
# OPT 	= -std=c++11 -O3 -flto -openmp
# OPT 	= -std=c++11 -O3 -mtune=native -march=native
# OPT 	= -std=c++11 -Ofast -mtune=native -march=native
# OPT = -std=c++11 -fopenmp -O2 -lm
PROG  	= cg.out fg.out
VPATH 	= src 
# INCDIR 	= inc src
# IPATH   = $(INCDIR:%=-I%)
# OBJS  	= main.o vars.o
# OBJS  	= main.o beads.o fgnup.o bcs.o grid.o cargo.o

all 		: ${PROG}
cg.out 	: cg_encoder.cpp
	${CC} $^ -o $@ ${OPT}
fg.out 	: fg_encoder.cpp
	${CC} $^ -o $@ ${OPT}

# ${PROG}	: ${OBJS}
# 	${CC} -o $@ ${OBJS} ${OPT}

# # %.o 	: %.cpp
# # 	${CC} -c $< ${OPT}

# .SUFFIXES: .cpp .o

# .cpp.o:
# 	$(CC) -c $< ${OPT} ${IPATH}


# main.o 		: main.cpp
# # vars.o 	: vars.cpp vars.h
# beads.o 	: beads.cpp beads.h
# fgnup.o 	: fgnup.cpp fgnup.h
# cargo.o 	: cargo.cpp cargo.h
# bcs.o 		: bcs.cpp 	bcs.h
# grid.o 		: grid.cpp  grid.h


clean 	: 
	rm *.out
