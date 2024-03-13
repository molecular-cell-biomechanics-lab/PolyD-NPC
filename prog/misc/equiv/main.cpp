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
// #include "cargo.h"
#include "bcs.h"
#include "vec.h"
#include "grid.h"

// #include "param.h"
// #include "sys.h"

// rule: always small to large

#define SEED_EQUIV	1
// #define PATH_BCS    "../../bcs/fgnup"
#define PATH_BCS    "input"
#define PATH_OUT 	"output"

#define OUT_STEP 	5000
// #define OUT_STEP 	500
#define LIST_STEP 	100
// #define TOTAL_STEP 	500000		// 10 us
// #define TOTAL_STEP 	1000000 	// 20 us
#define TOTAL_STEP 	2000000 	// 40 us


std::map<int,int> N_sol = {
//	{R_car, sol_num}
	{0, 33600},
	{1, 33598},
	{2, 33591},
	{3, 33571},
	{4, 33532},
	{5, 33468},
	{6, 33373},
	{7, 33239},
	{8, 33062},
	{9, 32834},
	{10, 32550},
	{11, 32202},
	{12, 31785},
	{13, 31293},
	{14, 30718},
	{15, 30056},
	{16, 29299},
	{17, 28441},
	{18, 27476},
	{19, 26398}
};
// calculated by ../misc/sol_size/sol_size.cpp



void bead_init
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_fgnup 	&fgnup,
	// _beads 	&soltn,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	fgnup.init(grid);
	// soltn.init(grid,fgnup,cargo);

	return;
}

void read_config
// --------------------------------------------------------------------
//
// Function: read configuration of beads
// 
(
	_fgnup 	&fgnup,
	// _beads 	&soltn,
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
	// _beads 	&soltn,
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
			sprintf(file, "%s/fg_s%03d", PATH_OUT, count); 
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
	// _beads  soltn(seed);
	_fgnup  fgnup(seed);

	bead_init(fgnup,grid);
	read_config(fgnup,grid,bcs);
	equiv_sim(fgnup,grid,bcs,seed);

	char file[128];
	sprintf(file, "%s/fgnup_equiv", PATH_OUT); 
	fgnup.output(file);
	
	return 0;
}


