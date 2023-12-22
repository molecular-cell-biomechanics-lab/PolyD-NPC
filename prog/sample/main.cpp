/* sample simulation */

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

#define PATH_CONFIG "input"
#define PATH_BCS    "input"
#define PATH_OUT	"output"
#define PATH_TRAJ	"traj"

#define CG_INIT_X 	0
#define CG_INIT_Y 	0
#define CG_INIT_Z 	-50

#define INIT_SHIFT 	40			// shift of init cargo position to adjust overlapping
#define N_INIT_DIV 	200			// # of steps to set the cargo to the CG_INIT_Z

#define INT_CAST 	1e-3 		// small number to add when casting from double to int

// preparation steps -----
// # of steps to push cargo inside the box: N_INIT_DIV*LIST_STEP = 10000 ( 400 ns)
// # of steps for the equilibriation 	  : EQ_STEP_BS 			 = 50000 (1000 ns)

// #define OUT_STEP 	5000 		// 0.1 us
// #define LIST_STEP 	100
// #define TOTAL_STEP 	5000000		// 100 us
// #define EQ_STEP_BS  50000		// 1  us
// #define TRAJ_OUT 	50000 		// 1  us

//DEBUG
#define OUT_STEP 	500 	
#define LIST_STEP 	100
#define TOTAL_STEP 	2000000		// 40 us
#define EQ_STEP_BS  500	
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


void read_input
// --------------------------------------------------------------------
//
// Purpose: read parameters from init.txt
// 
(
	int 	*D_por,
	int 	*leng ,
	int 	*N_lyr, 
	int 	*d_lyr,
	double 	*temp
)
// --------------------------------------------------------------------
{
	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_CONFIG, "init.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line
	std::getline(ifs, str); std::sscanf(str.data(), "D_por = %d" , D_por 	  );
	std::getline(ifs, str); std::sscanf(str.data(), "leng  = %d" , leng  	  );
	std::getline(ifs, str); std::sscanf(str.data(), "N_lyr = %d" , N_lyr 	  );
	std::getline(ifs, str); std::sscanf(str.data(), "d_lyr = %d" , d_lyr 	  );
	std::getline(ifs, str); std::sscanf(str.data(), "temp  = %lf", temp 	  );

	std::cout << "D_por = " << *D_por << std::endl;
	std::cout << "leng  = " << *leng  << std::endl;
	std::cout << "N_lyr = " << *N_lyr << std::endl;
	std::cout << "d_lyr = " << *d_lyr << std::endl;
	std::cout << "temp  = " << *temp  << std::endl;
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
	_bcs 	&bcs  ,
	int 	D_por ,
	int 	leng  ,
	int 	N_lyr , 
	int 	d_lyr , 
	int 	t_int
)
// --------------------------------------------------------------------
{
	char file[128];
	
	sprintf(file, "%s/cargo.vtk", PATH_BCS );
	cargo.set_beads(file, CG_INIT_X, CG_INIT_Y, CG_INIT_Z - INIT_SHIFT);

	sprintf(file, "%s/fgnup_p%02dl%03dn%02dd%02d.vtk", PATH_BCS, D_por, leng, N_lyr, d_lyr );
	fgnup.set_beads(file);

	double dz = (double)INIT_SHIFT / N_INIT_DIV;

	// cargo.output("cg_00.vtk");
	// fgnup.output("fg_00.vtk");

	// slowly push cargo inside the box
	std::cout << "setting cargo..." << std::endl;
	for( int i=0; i<N_INIT_DIV; i++ ){
		
		cargo.set_center(CG_INIT_X, CG_INIT_Y, CG_INIT_Z - INIT_SHIFT + dz*(i+1));
		cargo.make_list( bcs ); // no grid since cargo may be outside of the box
		fgnup.make_list( bcs, cargo );

		for( int j=0; j<LIST_STEP; j++ ){
			sys_2::calc_force(cargo, fgnup, grid, bcs);
			fgnup.move(bcs); // move only fgnup
		}
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
	int 	count = 0	;
	int 	step  = 0	;

	double  rpore_2 = bcs.R_pore * bcs.R_pore;
	_vec<double> prev_pos = cargo.center;

	while( step < TOTAL_STEP ){
	// while(1){

		if( step%LIST_STEP == 0 ){
			sys_2::make_list(cargo,fgnup,grid,bcs);
		}
		sys_2::calc_force(cargo,fgnup,grid,bcs);
		// cargo.frc[0].z += 10*KBT_0; // DEBUG
		sys_2::move 	 (cargo,fgnup,grid,bcs);

		if( step%OUT_STEP == 0 ){
			if( step%TRAJ_OUT == 0 ){
				char file1[128];
				sprintf(file1, "%s/cg_s%03d_t%08d", PATH_TRAJ, seed, step); cargo.output(file1);
				sprintf(file1, "%s/fg_s%03d_t%08d", PATH_TRAJ, seed, step); fgnup.output(file1);
			}
			std::cout << "step: " << step << std::endl;
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

void check_fg_values(_fgnup fgnup, _bcs bcs, int D_por, double temp)
{
	double bcs_por;
	double fg_temp;

	bcs_por = bcs.R_pore * 2.0;
	fg_temp = fgnup.T_body;


	if((int)(bcs_por+INT_CAST) != D_por){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"pore does not match " << (int)(bcs_por+INT_CAST) << " " << D_por
		<< "\x1b[39m" << std::endl;
		exit(1);
	}
	if(fg_temp != temp){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"temp does not match " << fg_temp << " " << temp
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

	int 	D_por;
	int 	leng ;
	int 	N_lyr;
	int 	d_lyr;
	double 	temp;
	int 	t_int;
	int 	t_dec;

	// initialization
	read_input (&D_por, &leng, &N_lyr, &d_lyr, &temp);
	convertToIntegers(temp , t_int, t_dec);
	bead_init  (cargo,fgnup,grid);
	read_config(cargo,fgnup,grid,bcs,D_por,leng,N_lyr,d_lyr,t_int);
	check_fg_values(fgnup,bcs,D_por,temp);

	// cargo.output("cg_test.vtk");
	// fgnup.output("fg_test.vtk");

	// simulation
	sample_sim(cargo,fgnup,grid,bcs,seed);
	
	return 0;
}


