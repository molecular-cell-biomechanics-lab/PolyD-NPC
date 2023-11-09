/* Equiv simulation */

#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <map>

#include "beads.h"
#include "fgnup.h"
#include "bcs.h"
#include "vec.h"
#include "grid.h"

#define SEED_EQUIV	1
#define PATH_BCS    "input"
#define PATH_OUT 	"output"

#define OUT_STEP 	5000
// #define OUT_STEP 	500
#define LIST_STEP 	100
#define TOTAL_STEP 	500000		// 10 us
// #define TOTAL_STEP 	1000000 	// 20 us
// #define TOTAL_STEP 	2000000 	// 40 us


void bead_init
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_fgnup 	&fgnup,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	fgnup.init(grid);

	return;
}

void read_config
// --------------------------------------------------------------------
//
// Function: read configuration of beads
// 
(
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  
)
// --------------------------------------------------------------------
{
	char file[128];	
	sprintf(file, "%s/%s", PATH_BCS, "fgnup_init.vtk");
	fgnup.set_beads(file);

	return;
}

void equiv_sim
// --------------------------------------------------------------------
//
// Function: simulation to equiribliate FG-Nups
// 
(
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
)
// --------------------------------------------------------------------
{
	int 	count = 0	;
	int 	step  = 0	;

	while( step <= TOTAL_STEP ){

		if( step%LIST_STEP == 0 ){
			fgnup.make_list( bcs, grid );
		}
		fgnup.calc_force( bcs );
		fgnup.move( bcs );

		if( step%OUT_STEP == 0 ){
			std::cout << step << std::endl;
			char file[128];
			sprintf(file, "%s/fg_t%03d", PATH_OUT, count); 
			fgnup.output(file);
			count ++;
		}

		step ++;
	}

	return;
}

void check_cmd_line(int argc)
{
    if( argc != 2 ){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"Specify a random number seed"
		<< "\x1b[39m" << std::endl;
        exit(1);
    }
    return;
}



int main(int argc, char *argv[]){

	// check_cmd_line(argc);
	// int seed = atoi(argv[1]);
	int seed = SEED_EQUIV;

	_bcs 	bcs;
	_grid 	grid(bcs);
	_fgnup  fgnup(seed);

	bead_init(fgnup,grid);
	read_config(fgnup,grid,bcs);
	equiv_sim(fgnup,grid,bcs,seed);

	// char file[128];
	// sprintf(file, "%s/fgnup_equiv", PATH_OUT); 
	// fgnup.output(file);
	
	return 0;
}


