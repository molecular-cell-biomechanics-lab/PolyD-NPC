/* Basin simulation */

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

#include "param.h"
#include "sys.h"

#define PATH_CONFIG "input"
#define PATH_BCS    "input"
#define PATH_OUT	"output"
#define PATH_TRAJ	"traj"

#define CG_INIT_X 	0
#define CG_INIT_Y 	0
#define CG_INIT_Z 	-50

#define INTF_CT_X 	0			// center of the first interface determination sphere
#define INTF_CT_Y 	0			// center of the first interface determination sphere
#define INTF_CT_Z 	0			// center of the first interface determination sphere


#define INIT_SHIFT 	40			// shift of init cargo position to adjust overlapping
#define N_INIT_DIV 	200			// # of steps to set the cargo to the CG_INIT_Z

#define INT_CAST 	1e-3 		// small number to add when casting from double to int

// preparation steps -----
// # of steps to push cargo inside the box: N_INIT_DIV*LIST_STEP = 10000 ( 400 ns)
// # of steps for the equilibriation 	  : EQ_STEP_BS 			 = 50000 (1000 ns)

#define OUT_STEP 	5000 		// 0.1 us
#define LIST_STEP 	100
#define TOTAL_STEP 	5000000		// 100 us
#define EQ_STEP_BS  50000		// 1  us
#define TRAJ_OUT 	50000 		// 1  us


void convertToIntegers(double input, int &intPart, int &decimalPart) {
	intPart = (int)input;
	decimalPart = (input - intPart) * 10;
}

int heaviside
// --------------------------------------------------------------------
//
// Purpose: Heaviside step function
// 			H(x-a) = 0 	(if x < a)
// 			H(x-a) = 1 	(if a < x)
// 
(
	double x, 
	double a 
)
// --------------------------------------------------------------------
{
	int val = 0;
	if( a < x ) val = 1;
	return val;
}

int heaviside_ad
// --------------------------------------------------------------------
//
// Purpose: Heaviside step function (modified specifically for basin)
// 			H(x-a) = 0 	(if |r-r0| > a) outside
// 			H(x-a) = 1 	(if |r-r0| < a) inside
// 
(
	_vec<double> pos,
	double 		 a_2	// radius^2
)
// --------------------------------------------------------------------
{
	int val = 0;
	double dist_2 = 0.0;
	dist_2 += (pos.x-INTF_CT_X) * (pos.x-INTF_CT_X);
	dist_2 += (pos.y-INTF_CT_Y) * (pos.y-INTF_CT_Y);
	dist_2 += (pos.z-INTF_CT_Z) * (pos.z-INTF_CT_Z);
	if( dist_2 < a_2 || pos.z > INTF_CT_Z )	val = 1;
	return val;
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

	return;
}


void read_input
// --------------------------------------------------------------------
//
// Purpose: read parameters from init.txt
// 
(
	int 	*D_por,
	int 	*leng ,
	double 	*R_car,
	int 	*p_hyd, 
	int 	*kuhn , 
	char 	*k_on ,
	char 	*k_of ,
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
	std::getline(ifs, str); std::sscanf(str.data(), "R_car = %lf", R_car 	  );
	std::getline(ifs, str); std::sscanf(str.data(), "p_hyd = %d" , p_hyd 	  );
	std::getline(ifs, str); std::sscanf(str.data(), "kuhn  = %d" , kuhn  	  );
	std::getline(ifs, str); std::sscanf(str.data(), "k_on  = %s" , k_on		  );
	std::getline(ifs, str); std::sscanf(str.data(), "k_of  = %s" , k_of 	  );
	std::getline(ifs, str); std::sscanf(str.data(), "temp  = %lf", temp 	  );

	// std::cout << "D_por = " << *D_por << std::endl;
	// std::cout << "R_car = " << *R_car << std::endl;
	// std::cout << "p_hyd = " << *p_hyd << std::endl;
	// std::cout << "kuhn  = " << *kuhn  << std::endl;
	// std::cout << "k_on  = " << k_on  << std::endl;
	// std::cout << "k_of  = " << k_of  << std::endl;

	return;
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

	return;
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
	int 	r_int ,
	int 	r_dec ,
	int 	p_hyd , 
	int 	kuhn  , 
	char 	*k_on , 
	char 	*k_of ,
	int 	t_int
)
// --------------------------------------------------------------------
{
	char file[128];
	
	sprintf(file, "%s/cargo_r%02d_%dp%02d.vtk", PATH_BCS, r_int, r_dec, p_hyd );
	cargo.set_beads(file, CG_INIT_X, CG_INIT_Y, CG_INIT_Z - INIT_SHIFT);

	// if( leng == 0 ){
	// 	sprintf(file, "%s/no_nup_p%02d.vtk", PATH_BCS, D_por );
	// }else{
	// 	sprintf(file, "%s/fgnup_equiv_p%02d_l%03d_b%d_k%s_k%s.vtk", PATH_BCS, D_por, leng, kuhn, k_on, k_of );		
	// }
	sprintf(file, "%s/fgnup_equiv_p%02d_l%03d_b%d_k%s_k%s_t%04d.vtk", PATH_BCS, D_por, leng, kuhn, k_on, k_of, t_int );
	fgnup.set_beads(file);

	double dz = (double)INIT_SHIFT / N_INIT_DIV;

	// slowly push cargo inside the box
	for( int i=0; i<N_INIT_DIV; i++ ){
		
		cargo.set_center(CG_INIT_X, CG_INIT_Y, CG_INIT_Z - INIT_SHIFT + dz*(i+1));
		cargo.make_list( bcs ); // no grid since cargo may be outside of the box
		// fgnup.make_list( bcs, grid, cargo );
		fgnup.make_list( bcs, cargo );

		for( int j=0; j<LIST_STEP; j++ ){
			sys_2::calc_force(cargo, fgnup, grid, bcs);
			fgnup.move(bcs); // move only fgnup
		}
		// sprintf(file, "%s/cg_init_%d.vtk", PATH_OUT, i ); cargo.output(file);
		// sprintf(file, "%s/fg_init_%d.vtk", PATH_OUT, i ); fgnup.output(file);
		// std::cout << i << std::endl;
	}

	// equilibriate
	for( int i=0; i<EQ_STEP_BS; i++ ){
		if( i%LIST_STEP == 0 ){
			sys_2::make_list(cargo, fgnup, grid, bcs);
		}
		sys_2::calc_force(cargo, fgnup, grid, bcs);
		fgnup.move(bcs);
	}

	return;
}

void basin_sim
// --------------------------------------------------------------------
//
// Function: simulation in the stable basin
// 
(
	_param 	&param,
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

			if(seed==1) {
				// std::cout << step << " " << prev_z << " " << cargo.center.z << ", count: " << count << std::endl;
				std::cout << step << "\t" << cargo.center.z << "\t count: " << count << std::endl;
			}

			if( heaviside_ad( prev_pos    , rpore_2 ) == 0 &&
				heaviside_ad( cargo.center, rpore_2 ) == 1 ){
				dummy_force( cargo, fgnup, grid, bcs );
				char file[128];
				sprintf(file, "%s/cg_s%03d_%03d", PATH_OUT, seed, count); cargo.output(file);
				sprintf(file, "%s/fg_s%03d_%03d", PATH_OUT, seed, count); fgnup.output(file);
				count ++;
			}
			prev_pos = cargo.center;
		}

		step ++;
	}

	double time = step * fgnup.t_delt;
	double flux = count / time;

	std::cout << "seed: " << seed << ", count = " << count << ", flux = " << flux << " (/ns)" << std::endl;

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

void check_cg_radius(_cargo cargo, double R_car)
{
	// if(cargo.R_cargo != (double)(R_car)){
	if(cargo.R_cargo != R_car){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"R_car does not match " << cargo.R_cargo << " " << (double)(R_car)
		<< "\x1b[39m" << std::endl;
        exit(1);
	}
	return;
}

void check_fg_values(_fgnup fgnup, _bcs bcs, int D_por, int kuhn, char* k_on, char* k_of, double temp)
{
	double bcs_por;
	double fg_kuhn;
	double fg_k_on;
	double fg_k_of;
	double fg_temp;

	bcs_por = bcs.R_pore * 2.0;
	fg_kuhn = fgnup.l_kuhn / fgnup.l_segm;
	fg_k_on = fgnup.kon_fg;
	fg_k_of = fgnup.kof_fg;

	fg_temp = fgnup.T_body;


	if((int)(bcs_por+INT_CAST) != D_por){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"pore does not match " << (int)(bcs_por+INT_CAST) << " " << D_por
		<< "\x1b[39m" << std::endl;
		exit(1);
	}
	if((int)(fg_kuhn+INT_CAST) != kuhn){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"kuhn does not match " << (int)(fg_kuhn+INT_CAST) << " " << kuhn
		<< "\x1b[39m" << std::endl;
		exit(1);
	}
	if(fg_k_on != atof(k_on)){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"k_on does not match " << fg_k_on << " " << k_on
		<< "\x1b[39m" << std::endl;
		exit(1);
	}
	if(fg_k_of != atof(k_of)){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"k_of does not match " << fg_k_of << " " << k_of
		<< "\x1b[39m" << std::endl;
		exit(1);
	}
	if(fg_temp != temp){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"temp does not match " << fg_temp << " " << temp
		<< "\x1b[39m" << std::endl;
		exit(1);
	}

	return;
}

int main(int argc, char *argv[]){

	check_cmd_line(argc);
    int seed = atoi(argv[1]);

	_param 	param;

	_bcs 	bcs;
	_grid 	grid(bcs);
	_cargo 	cargo(seed);
	_fgnup  fgnup(seed);

	int 	D_por;
	int 	leng ;
	double 	R_car;
	int 	r_int;
	int 	r_dec;
	int 	p_hyd;
	int 	kuhn ;
	char 	k_on[16];
	char 	k_of[16];
	double 	temp;
	int 	t_int;
	int 	t_dec;

	// initialization
	read_input (&D_por, &leng, &R_car, &p_hyd, &kuhn, k_on, k_of, &temp);
	convertToIntegers(R_car, r_int, r_dec);
	convertToIntegers(temp , t_int, t_dec);
	bead_init  (cargo,fgnup,grid);
	read_config(cargo,fgnup,grid,bcs,D_por,leng,r_int,r_dec,p_hyd,kuhn,k_on,k_of,t_int);
	check_cg_radius(cargo,R_car);
	check_fg_values(fgnup,bcs,D_por,kuhn,k_on,k_of,temp);

	// cargo.output("cg_test.vtk");
	// fgnup.output("fg_test.vtk");

	// main basin simulation
	basin_sim(param,cargo,fgnup,grid,bcs,seed);
	
	return 0;
}


