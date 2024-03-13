#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <map>

#include "beads.h"
#include "fgnup.h"
#include "cargo.h"
#include "bcs.h"
#include "vec.h"
#include "grid.h"
#include "env.h"

#include "sys.h"

#define PATH_INP    "input"
#define PATH_OUT	"output"

#define CG_INIT_X 	0
#define CG_INIT_Y 	0
#define CG_INIT_Z 	-20

#define INIT_SHIFT 	40			// shift of init cargo position to adjust overlapping
#define N_INIT_DIV 	200			// # of steps to set the cargo to the CG_INIT_Z

#define INT_CAST 	1e-3 		// small number to add when casting from double to int

// preparation steps -----
// # of steps to push cargo inside the box: N_INIT_DIV*LIST_STEP = 10000 ( 400 ns)
// # of steps for the equilibriation 	  : EQ_STEP_BS 			 = 50000 (1000 ns)

// #define LIST_STEP 	100
// #define TOTAL_STEP 	10000000	// 100 us
// #define EQ_STEP_BS  50000		// 0.5 us
// #define CNTR_OUT 	1000 		// 10   ns
// #define OUT_STEP 	50000 		// 0.5 	us
// #define TRAJ_OUT 	50000 		// 0.5  us

// //DEBUG
#define LIST_STEP 	100
#define TOTAL_STEP 	25000 
#define EQ_STEP_BS  500
#define CNTR_OUT 	1000
#define OUT_STEP 	1000
#define TRAJ_OUT 	5000


void convertToIntegers(double input, int &intPart, int &decimalPart) {
	intPart = (int)input;
	decimalPart = (input - intPart) * 10;
}


void dummy_force
// --------------------------------------------------------------------
//
// Function: dummy calculation of the force for force record in _pos
// 			 (we want to record force after moving beads.
// 			  the force calculated in the main loop is before moving beads.
//			  just the calculation of the force does not harm the system.)
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  
)
// --------------------------------------------------------------------
{
	sys_2::calc_force( cargo, fgnup, grid, bcs ); 		
	cargo.calc_cargo_force( bcs ); // usually in move() function
}

void bead_init
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	cargo.init(grid);
	fgnup.init(grid,cargo);
}

void read_config
// --------------------------------------------------------------------
//
// Function: read configuration of beads
//
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  
)
// --------------------------------------------------------------------
{
	char file[128];
	
	sprintf(file, "%s/cargo.vtk", PATH_INP );
	cargo.set_beads(file, CG_INIT_X, CG_INIT_Y, CG_INIT_Z - INIT_SHIFT);

	sprintf(file, "%s/fgnup.vtk", PATH_INP );
	fgnup.set_beads(file);

	double dz = (double)INIT_SHIFT / N_INIT_DIV;

	// cargo.output("cg_00.vtk");
	// fgnup.output("fg_00.vtk");

	// slowly push cargo inside the box
	std::cout << "setting cargo..." << std::endl;
	for( int i=0; i<N_INIT_DIV; i++ ){
		
		cargo.set_center(CG_INIT_X, CG_INIT_Y, CG_INIT_Z - INIT_SHIFT + dz*(i+1));
		cargo.make_list(bcs); // no grid since cargo may be outside of the box
		fgnup.make_list(bcs, cargo);

		for( int j=0; j<LIST_STEP; j++ ){
			sys_2::calc_force(cargo, fgnup, grid, bcs);
			fgnup.move(bcs); // move only fgnup
		}
		// sprintf(file, "test/test_%03d.vtk", i);
		// fgnup.output(file);
		// std::cout << "test" << std::endl;
	}

	// cargo.output("cg_01.vtk");
	// fgnup.output("fg_01.vtk");

	// equilibriate
	std::cout << "equilibriating..." << std::endl;
	for( int i=0; i<EQ_STEP_BS; i++ ){
		if( i%LIST_STEP == 0 ){
			sys_2::make_list(cargo, fgnup, grid, bcs);
		}
		sys_2::calc_force(cargo, fgnup, grid, bcs);
		fgnup.move(bcs);
	}
}

void sample_sim
// --------------------------------------------------------------------
//
// Function: simulation
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
)
{
	int count = 0;
	int step  = 0;

	double  rpore_2 = bcs.R_pore * bcs.R_pore;
	_vec<double> prev_pos = cargo.center;

	std::cout << "main simulation..." << std::endl;

	while( step <= TOTAL_STEP ){
	// while(1){

		if( step%LIST_STEP == 0 ){
			sys_2::make_list(cargo,fgnup,grid,bcs);
		}
		sys_2::calc_force(cargo,fgnup,grid,bcs);
		// cargo.frc[0].z += 10.0*KBT_0; // DEBUG
		sys_2::move 	 (cargo,fgnup,grid,bcs);


		if( step%CNTR_OUT == 0 ){
			char file1[128];
			sys_2::record_adj(cargo,fgnup,grid,bcs);
			sprintf(file1, "%s/cg_s%03d_t%08d", PATH_OUT, seed, step); cargo.output_ctr(file1);
			sprintf(file1, "%s/fg_s%03d_t%08d", PATH_OUT, seed, step); fgnup.output_adj(file1);

			if( step%OUT_STEP == 0 ){
				std::cout << "step: " << step << std::endl;

				if( step%TRAJ_OUT == 0 ){
					sprintf(file1, "%s/cg_s%03d_t%08d", PATH_OUT, seed, step); cargo.output_asc_vtx(file1);
					sprintf(file1, "%s/fg_s%03d_t%08d", PATH_OUT, seed, step); fgnup.output_asc(file1);
				}
			}
		}
		step ++;
	}
}

void check_cmd_line(int argc)
{
    if( argc != 2 ){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"Specify a random number seed"
		<< "\x1b[39m" << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[]){

	check_cmd_line(argc);
    int seed = atoi(argv[1]);
	_bcs 	bcs;
	_grid 	grid(bcs);
	_cargo 	cargo(seed);
	_fgnup  fgnup(seed);

	// initialization
	bead_init  (cargo,fgnup,grid);
	read_config(cargo,fgnup,grid,bcs);

	// cargo.output("cg_test.vtk");
	// fgnup.output("fg_test.vtk");

	// simulation
	sample_sim(cargo,fgnup,grid,bcs,seed);
	
	return 0;
}


