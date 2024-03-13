#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "vec.h"
#include "bcs.h"
#include "env.h"
#include "beads.h"
#include "cargo.h"


static double 	eps_rp;
static double 	sgm_rp;
static double 	cut_rp;
static double 	cut_wall;
static double 	frc_w_const;

// variables for periodic condition
static double 	copy_width;	// width to copy for periodic adjustment
// static double 	length_z;		// z-length of the simulation box


void _bcs::bounce_on_wall
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_beads 	*beads
)
// --------------------------------------------------------------------
{
	//TODO: grid search

	for( int i=0; i<beads->pos.size(); i++ ){

		double 	x 	  = beads->pos[i].x;
		double 	y 	  = beads->pos[i].y;
		double 	z 	  = beads->pos[i].z;

		_vec<double>  r_pos( x, y, 0.0 );
		double r_norm = r_pos.norm();
		double r_dist = R_pore - r_norm;

		if( z > cy_btm && z < cy_top ){
			if( r_dist < cut_wall ){
				double force = -frc_w_const;
				_vec<double> df = (force / r_norm) * r_pos;
				beads->frc[i].x += df.x;
				beads->frc[i].y += df.y;
			}
		}else if( z < cy_btm ){
			if( cy_btm - z < cut_wall && r_dist < 0 ){
				double force = -frc_w_const;
				beads->frc[i].z += force;
			}
		}else if( z > cy_top ){
			if( z - cy_top < cut_wall && r_dist < 0 ){
				double force = frc_w_const;
				beads->frc[i].z += force;
			}
		}


		// reflecting boundary on the top and bottom z
		if( z < cy_btm - L_z ){
			double force = frc_w_const;
			beads->frc[i].z += force;
		}else if( z > cy_top + L_z ){
			double force = -frc_w_const;
			beads->frc[i].z += force;
		}


		// if( r_dist < cut_wall ){
		// 	if( z > cy_btm && z < cy_top ){
		// 		double force = -frc_w_const;
		// 		_vec<double> df = (force / r_norm) * r_pos;
		// 		beads->frc[i].x += df.x;
		// 		beads->frc[i].y += df.y;
		// 	}else if( z < cy_btm ){
		// 		double force = -frc_w_const;
		// 		beads->frc[i].z += force;
		// 	}else if( z > cy_top ){
		// 		double force = frc_w_const;
		// 		beads->frc[i].z += force;
		// 	}

		// }

	}
}



void _bcs::calc_force_wall
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_beads 	*beads
)
// --------------------------------------------------------------------
{
	bounce_on_wall( beads );
}


// void _bcs::adjust_periodic
// // --------------------------------------------------------------------
// //
// // Function:
// // 
// (
// 	_vec<double> 	*dr
// )
// // --------------------------------------------------------------------
// {
// 	double dz = dr->z;
// 	if( dz >= thre_dist_z ){
// 		dz -= length_z;
// 	}else if( dz <= thre_dist_z ){
// 		dz += length_z;
// 	}
// 	return;
// }


void _bcs::move_at_boundary
// --------------------------------------------------------------------
//
// Function:
// 
(
	_beads 	*beads
)
// --------------------------------------------------------------------
{
	for( int i=0; i<beads->pos.size(); i++ ){
		double x = beads->pos[i].x;
		double y = beads->pos[i].y;
		double z = beads->pos[i].z;
		// x --------------------------
		if( x < - L_x ){
			beads->pos[i].x += length_x;
		}else if( x > L_x ){
			beads->pos[i].x -= length_x;
		}
		// y --------------------------
		if( y < - L_y ){
			beads->pos[i].y += length_y;
		}else if( y > L_y ){
			beads->pos[i].y -= length_y;
		}
		// z --------------------------
		// if( z < cy_btm - L_z ){
		// 	beads->pos[i].z += length_z;
		// }else if( z > cy_top + L_z ){
		// 	beads->pos[i].z -= length_z;
		// }
	}
}



void _bcs::move_at_boundary
// --------------------------------------------------------------------
//
// Function: for cargo
// 
(
	_cargo 	*cargo
)
// --------------------------------------------------------------------
{
	double x = cargo->center.x;
	double y = cargo->center.y;

	if( x < - L_x ){
		cargo->center.x += length_x;
		cargo->update_pos();
	}else if( x > L_x ){
		cargo->center.x -= length_x;
		cargo->update_pos();
	}

	if( y < - L_y ){
		cargo->center.y += length_y;
		cargo->update_pos();
	}else if( y > L_y ){
		cargo->center.y -= length_y;
		cargo->update_pos();
	}
}

// void _bcs::init
// // --------------------------------------------------------------------
// //
// // Function: initialize the system
// // 
// (
// 	_beads 	*beads
// )
// // --------------------------------------------------------------------
// {
// 	calc_static( beads );
// 	return;
// }


void _bcs::calc_static
// --------------------------------------------------------------------
//
// Function: calculate static variables
// 
(
	// _beads 	*beads
)
// --------------------------------------------------------------------
{
	_beads tmp;
	// beads->get_vars_rp( &eps_rp, &sgm_rp, &cut_rp );
	tmp.get_vars_rp( &eps_rp, &sgm_rp, &cut_rp );
	cut_wall = cut_rp / 2.0;
	frc_w_const = eps_rp / sgm_rp;
	// copy_width_z = beads->get_cut_list();
	copy_width = tmp.get_cut_list();
	length_x = 2.0*L_x;
	length_y = 2.0*L_y;
	length_z = cy_top - cy_btm + 2.0*L_z;

	thre_dist_x = length_x - 2.0*copy_width;
	thre_dist_y = length_y - 2.0*copy_width;
	thre_dist_z = length_z - 2.0*copy_width;
}


void _bcs::read_vars
// --------------------------------------------------------------------
//
// Function: read parameters from bcs.txt
// 
(
)
// --------------------------------------------------------------------
{
	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, "bcs.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "R_pore = %lf", &(this->R_pore) );
	std::getline(ifs, str); std::sscanf( str.data(), "cy_top = %lf", &(this->cy_top) );
	std::getline(ifs, str); std::sscanf( str.data(), "cy_btm = %lf", &(this->cy_btm) );
	std::getline(ifs, str); std::sscanf( str.data(), "L_x    = %lf", &(this->L_x   ) );
	std::getline(ifs, str); std::sscanf( str.data(), "L_y    = %lf", &(this->L_y   ) );
	std::getline(ifs, str); std::sscanf( str.data(), "L_z    = %lf", &(this->L_z   ) );
}


// void _bcs::read_beads_vars
// // --------------------------------------------------------------------
// //
// // Purpose: read parameters from beads.txt
// // 
// (
//    	// char 	*ifs_name
// )
// // --------------------------------------------------------------------
// {

// 	char ifs_name[64];
// 	sprintf( ifs_name, "%s/%s", PATH_INPUT, "beads.txt" );
// 	std::ifstream ifs(ifs_name);
// 	std::string str;

// 	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }


// 	std::getline(ifs, str); // skip 1 line
// 	std::getline(ifs, str); std::sscanf( str.data(), "l_segm = %lf", &cut_rp );
// 	std::getline(ifs, str); // t_delt
// 	std::getline(ifs, str); // T_body
// 	std::getline(ifs, str); // h_visc
// 	std::getline(ifs, str); // r_stks
// 	std::getline(ifs, str); // D_diff
// 	std::getline(ifs, str); std::sscanf( str.data(), "eps_rp = %lf", &eps_rp );
// 	std::getline(ifs, str); std::sscanf( str.data(), "sgm_rp = %lf", &sgm_rp );

// 	return;
// }




void _bcs::display_vars
// --------------------------------------------------------------------
//
// Purpose: display input variables
// 
(
)
// --------------------------------------------------------------------
{
    std::cout << "\x1b[34m"	 << std::endl;

	std::cout << "R_pore = " << this->R_pore << std::endl;
	std::cout << "cy_top = " << this->cy_top << std::endl;
	std::cout << "cy_btm = " << this->cy_btm << std::endl;
	std::cout << "L_x    = " << this->L_x    << std::endl;
	std::cout << "L_y    = " << this->L_y    << std::endl;
	std::cout << "L_z    = " << this->L_z    << std::endl;
	
    std::cout << "\x1b[39m"	<< std::endl;
}


_bcs::_bcs
// --------------------------------------------------------------------
//
// Constructor: 
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "bcs Constructor" << std::endl;
	read_vars();
	calc_static();
	// display_vars();
}


_bcs::~_bcs
// --------------------------------------------------------------------
//
// Deconstructor:
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "bcs destructor" << std::endl;
}

