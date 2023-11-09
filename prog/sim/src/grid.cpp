#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "vec.h"
#include "env.h"
#include "bcs.h"
#include "grid.h"
#include "beads.h"


double 	x_edge;
double 	y_edge;
double 	z_edge;


void _grid::read_vars
// --------------------------------------------------------------------
//
// Function: read parameters from bcs.txt
// 
(
)
// --------------------------------------------------------------------
{

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, "grid.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }


	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "grid_dim = %lf", &(this->grid_dim) );

	return;
}


void _grid::calc_vars
// --------------------------------------------------------------------
//
// Function: calculate class variables
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	// this->num_x = (int)((bcs.R_pore * 2) 		  / this->grid_dim);
	// this->num_y = (int)((bcs.R_pore * 2) 		  / this->grid_dim);
	// this->num_z = (int)((bcs.cy_top - bcs.cy_btm) / this->grid_dim);

	num_x = (int)((bcs.L_x * 2) 		  / grid_dim);
	num_y = (int)((bcs.L_y * 2) 		  / grid_dim);
	num_z = (int)((bcs.cy_top - bcs.cy_btm + bcs.L_z * 2) / grid_dim);

	N_grid = num_x * num_y * num_z;

	// x_edge = - bcs.R_pore;
	// y_edge = - bcs.R_pore;
	// z_edge =   bcs.cy_btm;

	x_edge = - bcs.L_x;
	y_edge = - bcs.L_y;
	z_edge = - bcs.L_z + bcs.cy_btm;

	return;
}


void _grid::display_vars
// --------------------------------------------------------------------
//
// Purpose: display input variables
// 
(
)
// --------------------------------------------------------------------
{

    std::cout << "\x1b[34m"	 << std::endl;

	std::cout << "grid_dim = " << this->grid_dim << std::endl;
	std::cout << "num_x    = " << this->num_x    << std::endl;
	std::cout << "num_y    = " << this->num_y    << std::endl;
	std::cout << "num_z    = " << this->num_z    << std::endl;
	
    std::cout << "\x1b[39m"	<< std::endl;
    return;

}



// void _grid::init
// // --------------------------------------------------------------------
// //
// // Function: calculate class variables
// // 
// (
// 	_bcs 	*bcs
// )
// // --------------------------------------------------------------------
// {
// 	calc_vars( bcs );
// 	display_vars();
// 	make_grid();
// 	list_grid_nbr();
// 	return;
// }


void _grid::make_grid
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	int total_num = num_x * num_y * num_z;
	for( int i=0; i<total_num; i++ ){
		// binned_beads.push_back(std::vector<int>());
		grid_nbr.push_back(std::vector<int>());
	}
	return;
}


void _grid::list_grid_nbr
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	for( int id=0; id<grid_nbr.size(); id++ ){

		int i = (int) (id / (num_y * num_z));
		int j = (int) ((id - num_y * num_z* i) / num_z);
		int k = id - num_y * num_z * i - num_z * j;

		for( int a=-1; a<=1; a++ ){
		for( int b=-1; b<=1; b++ ){
		for( int c=-1; c<=1; c++ ){
			
			int ii = i+a;
			int jj = j+b;
			int kk = k+c;

			if( ii < 0 || ii >= num_x ) continue;
			if( jj < 0 || jj >= num_y ) continue;
			if( kk < 0 ){
				kk += num_z;
			}else if( kk >= num_z ){
				kk -= num_z;
			}

			int nbr_id = ii*num_y*num_z + jj*num_z + kk;

			// if( nbr_id == id ) continue; // exclude itself

			grid_nbr[id].push_back( nbr_id );
		}
		}
		}

	}
	return;
}


int _grid::get_bin
// --------------------------------------------------------------------
//
// Function: calculate class variables
// 
(
	_vec<double> 	pos
)
// --------------------------------------------------------------------
{
	int i = (int)( (pos.x - x_edge) / grid_dim );
	int j = (int)( (pos.y - y_edge) / grid_dim );
	int k = (int)( (pos.z - z_edge) / grid_dim );

	// if( i < 0 || i >= num_x || 
	// 	j < 0 || j >= num_y || 
	// 	k < 0 || k >= num_z ){
	// 	std::cout << "ERROR: beads out side of the box" << std::endl;
	// 	exit(1);
	// }


	if( i >= num_x ) i = num_x-1; 
	else if( i < 0 ) i = 0;

	if( j >= num_y ) j = num_y-1; 
	else if( j < 0 ) j = 0;

	if( k >= num_z ) k = num_z-1; 
	else if( k < 0 ) k = 0;
	
	int id = i*num_y*num_z + j*num_z + k;

	return id;
}

void _grid::sort_beads
// --------------------------------------------------------------------
//
// Function: calculate class variables
// 
(
	_beads 	*beads
)
// --------------------------------------------------------------------
{
	for( int i=0; i<N_grid; i++ ){
		beads->binned_beads[i].clear();
	}

	for( int i=0; i<beads->pos.size(); i++ ){
		int grid_id = get_bin( beads->pos[i] );
		beads->binned_beads[ grid_id ].push_back(i);
	}

	return;
}


_grid::_grid
// --------------------------------------------------------------------
//
// Constructor: 
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	read_vars();
	calc_vars( bcs );
	// display_vars();
	make_grid();
	list_grid_nbr();
}


_grid::~_grid
// --------------------------------------------------------------------
//
// Deconstructor:
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "grid destructor" << std::endl;
}
