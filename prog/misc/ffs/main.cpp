#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "beads.h"
#include "fgnup.h"
#include "cargo.h"
#include "bcs.h"
#include "vec.h"
#include "grid.h"

#include "sim.h"
#include "param.h"

// execute: ./a.out {seed}
// rule: always small to large

#define PATH_INP "input"
#define INT_CAST 	1e-3 		// small number to add when casting from double to int


void read_input
(
	int 	*pore,
	double 	*rcar,
	int 	*phyd, 
	int 	*kuhn, 
	char 	*k_on,
	char 	*k_of,
	double 	*temp
)
{
	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INP, "init.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line
	std::getline(ifs, str); std::sscanf(str.data(), "pore = %d" , pore 	);
	std::getline(ifs, str); std::sscanf(str.data(), "rcar = %lf", rcar 	);
	std::getline(ifs, str); std::sscanf(str.data(), "phyd = %d" , phyd 	);
	std::getline(ifs, str); std::sscanf(str.data(), "kuhn = %d" , kuhn 	);
	std::getline(ifs, str); std::sscanf(str.data(), "k_on = %s" , k_on	);
	std::getline(ifs, str); std::sscanf(str.data(), "k_of = %s" , k_of 	);
	std::getline(ifs, str); std::sscanf(str.data(), "temp = %lf", temp 	);

	// std::cout << "pore = " << *pore << std::endl;
	// std::cout << "rcar = " << *rcar << std::endl;
	// std::cout << "phyd = " << *phyd << std::endl;
	// std::cout << "kuhn = " << *kuhn << std::endl;
	// std::cout << "k_on = " << k_on  << std::endl;
	// std::cout << "k_of = " << k_of  << std::endl;?


	return;
}

void check_cg_para(_cargo &cargo, double rcar)
{
	if(cargo.R_cargo != rcar){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"R_car does not match " << cargo.R_cargo << " " << (double)(rcar)
		<< "\x1b[39m" << std::endl;
        exit(1);
	}
	return;
}

void check_fg_para(_fgnup &fgnup, _bcs &bcs, int pore, int kuhn, char* k_on, char* k_of, double temp)
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

	if((int)(bcs_por+INT_CAST) != pore){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"kuhn does not match " << (int)(bcs_por+INT_CAST) << " " << pore
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

void check_input
// --------------------------------------------------------------------
//
// Purpose: read parameters from init.txt and check inputs are correct
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	int 	pore;
	double	rcar;
	int 	phyd;
	int 	kuhn;
	char 	k_on[16];
	char 	k_of[16];
	double 	temp;

	read_input(&pore,&rcar,&phyd,&kuhn,k_on,k_of,&temp);
	check_cg_para(cargo,rcar);
	check_fg_para(fgnup,bcs,pore,kuhn,k_on,k_of,temp);

	return;
}

void read_config
// --------------------------------------------------------------------
//
// Function: read configuration of beads
// 
(
	int 	seed  ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	char dire[64];
	sprintf(dire, "%s/s%03d_", PATH_INP, seed);
	sys_2::init(cargo, fgnup, grid, dire);

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

	check_cmd_line(argc);
    int seed = atoi(argv[1]);

	_param 	param;
	_path 	path ;

	_bcs 	bcs;
	_grid 	grid(bcs);
	_fgnup  fgnup(seed);
	_cargo 	cargo(seed);

	check_input(cargo,fgnup,bcs);

    // initialization -> moved intto ffs_sim
    // read_config(seed,cargo,fgnup,grid);

    // main ffs simulation
    ffs_sim(param,path,cargo,fgnup,grid,bcs,seed);

    // output recording
    write_out(seed,param,path,cargo,fgnup);

	return 0;
}

