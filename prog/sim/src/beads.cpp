#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "beads.h"
#include "cargo.h"
#include "fgnup.h"
#include "bcs.h"
#include "env.h"
#include "vec.h"
#include "grid.h"

static double 	coef_rp_12;		// coefficient of repulsive force
static double 	coef_rp_6 ;		// coefficient of repulsive force
static double 	coef_fg_12;		// coefficient of hydrophbc force
static double 	coef_fg_6 ;		// coefficient of hydrophbc force
static double 	a_shift_rp;		// coefficient of repulsive force
static double 	a_shift_fg;		// coefficient of hydrophbc force

// std::mt19937 							mt   (RANDSEED) ; 	// random number generator
std::mt19937 							mt; 				// random number generator
std::uniform_real_distribution<double> 	unif (-1.0, 1.0);	// uniform distribution


void _beads::read_vars
// --------------------------------------------------------------------
//
// Purpose: read parameters from beads.txt
// 
(
   	// char 	*ifs_name
)
// --------------------------------------------------------------------
{
	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, "beads.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "l_segm = %lf", &l_segm );
	std::getline(ifs, str); std::sscanf( str.data(), "t_delt = %lf", &t_delt );
	std::getline(ifs, str); std::sscanf( str.data(), "T_body = %lf", &T_body );
	std::getline(ifs, str); std::sscanf( str.data(), "h_visc = %lf", &h_visc );
	std::getline(ifs, str); std::sscanf( str.data(), "r_stks = %lf", &r_stks );
	std::getline(ifs, str); // D_diff
	std::getline(ifs, str); std::sscanf( str.data(), "eps_rp = %lf", &eps_rp );
	std::getline(ifs, str); std::sscanf( str.data(), "sgm_rp = %lf", &sgm_rp );
	std::getline(ifs, str); std::sscanf( str.data(), "cut_rp = %lf", &cut_rp );
	std::getline(ifs, str); std::sscanf( str.data(), "sft_rp = %lf", &sft_rp );
	std::getline(ifs, str); std::sscanf( str.data(), "eps_fg = %lf", &eps_fg );
	std::getline(ifs, str); std::sscanf( str.data(), "sgm_fg = %lf", &sgm_fg );
	std::getline(ifs, str); std::sscanf( str.data(), "cut_fg = %lf", &cut_fg );
	std::getline(ifs, str); std::sscanf( str.data(), "sft_fg = %lf", &sft_fg );

	if( abs(cut_rp - pow(2-sft_rp, 1.0/6.0)) > 0.001 ){
		std::cout << "\x1b[31m" << "ERROR: steric cutoff is not at the bottom" << std::endl;
		std::cout << "cut_rp = " << cut_rp << ", (2-sft_rp)^(1/6) = " << pow(2-sft_rp, 1.0/6.0)
		<<"\x1b[349" << std::endl;
		exit(1);
	}
}



void _beads::calc_vars
// --------------------------------------------------------------------
//
// Purpose: unit conversion and calcultion of parameters in _param
// 
(
)
// --------------------------------------------------------------------
{
	// unit conversion
	h_visc = h_visc * 1e-18			;	// {mPa s} = {g /m /s} -> (g /nm /ns)
	r_stks = r_stks * l_segm 		; 	// {segm}  -> (nm)
	eps_rp = eps_rp * KBT_0			;	// {kBT_0} -> (nm2 g /ns2)
	sgm_rp = sgm_rp * l_segm		;	// {segm}  -> (nm)
	cut_rp = cut_rp * l_segm		;	// {segm}  -> (nm)
	eps_fg = eps_fg * KBT_0 		;	// {kBT_0} -> (nm2 g /ns2)
	sgm_fg = sgm_fg * l_segm		;	// {segm}  -> (nm)
	cut_fg = cut_fg * l_segm		;	// {segm}  -> (nm)

	// calculation of class variables
	D_diff = K_BOLTZ * T_body / 6.0 / PI / h_visc / r_stks; 	// (nm2 / ns)

	// this->cut_list = std::max( this->cut_rp, this->cut_fg );
	// this->cut_list += MARGIN;

	// std::cout << D_diff << std::endl;
	// exit(1);

	cut_rp_list = cut_rp + MARGIN;
	cut_fg_list = cut_fg + MARGIN;
}


void _beads::calc_static
// --------------------------------------------------------------------
//
// Function: calcultion of the static variables
// 
(
)
// --------------------------------------------------------------------
{
	cut_rp_2 = cut_rp * cut_rp;
	cut_fg_2 = cut_fg * cut_fg;

	cut_rp_list_2 = cut_rp_list * cut_rp_list;
	cut_fg_list_2 = cut_fg_list * cut_fg_list;

	coef_rp_12 =  48.0 * eps_rp * pow(sgm_rp, 12.0);
	coef_rp_6  = -24.0 * eps_rp * pow(sgm_rp,  6.0);

	coef_fg_12 =  48.0 * eps_fg * pow(sgm_fg, 12.0);
	coef_fg_6  = -24.0 * eps_fg * pow(sgm_fg,  6.0);

	a_shift_rp = sft_rp * pow(sgm_rp, 6.0);
	a_shift_fg = sft_fg * pow(sgm_fg, 6.0);

	coeff_ex = D_diff / K_BOLTZ / T_body * t_delt;
	rand_std = sqrt(3) * sqrt( 2 * D_diff * t_delt);

	// std::cout << "rand_std = " << rand_std << std::endl;
	// std::cout << "coeff_ex = " << coeff_ex << std::endl;
	// exit(1);
}


void _beads::display_vars
// --------------------------------------------------------------------
//
// Purpose: display input variables
// 
(
)
// --------------------------------------------------------------------
{
    std::cout << "\x1b[34m"	 << std::endl;

	std::cout << "l_segm = " << l_segm << std::endl;
	std::cout << "t_delt = " << t_delt << std::endl;

	std::cout << "T_body = " << T_body << std::endl;
	std::cout << "h_visc = " << h_visc << std::endl;
	std::cout << "r_stks = " << r_stks << std::endl;
	std::cout << "D_diff = " << D_diff << std::endl;
	
	std::cout << "eps_rp = " << eps_rp << std::endl;
	std::cout << "sgm_rp = " << sgm_rp << std::endl;
	std::cout << "cut_rp = " << cut_rp << std::endl;
	std::cout << "sft_rp = " << sft_rp << std::endl;
	
	std::cout << "eps_fg = " << eps_fg << std::endl;
	std::cout << "sgm_fg = " << sgm_fg << std::endl;
	std::cout << "cut_fg = " << cut_fg << std::endl;
	std::cout << "sft_fg = " << sft_fg << std::endl;
	
    std::cout << "\x1b[39m"	<< std::endl;
}

void _beads::init_rand
// --------------------------------------------------------------------
//
// Function: initialize random number generator (seed default)
// 
(
)
// --------------------------------------------------------------------
{
	mt.seed(RANDSEED);
	seed = RANDSEED;
	return;
}

void _beads::init_rand
// --------------------------------------------------------------------
//
// Function: initialize random number generator (seed from input)
// 
(
	int 	seed_i
)
// --------------------------------------------------------------------
{
	mt.seed(seed_i);
	seed = seed_i;
	// std::cout << "beads constructor seed: " << seed << std::endl;
	return;
}



_beads::~_beads
// --------------------------------------------------------------------
//
// Deconstructor: set member variables from input file
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "beads descructor" << std::endl;
}


_beads::_beads
// --------------------------------------------------------------------
//
// Constructor: set member variables from input file
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "beads Constructor" << std::endl;
	read_vars	();
	calc_vars	();
	calc_static	();
	// display_vars();
	// init_rand();
}

_beads::_beads
// --------------------------------------------------------------------
//
// Constructor: set member variables from input file
// 
(
	int 	seed_i
)
// --------------------------------------------------------------------
{
	// std::cout << "beads Constructor" << std::endl;
	read_vars	();
	calc_vars	();
	calc_static	();
	// display_vars();
	init_rand(seed_i);
}


void _beads::set_beads
// --------------------------------------------------------------------
//
// Function: sample
// 
(
	double	x,
	double	y,
	double	z,
	double 	i_hyd
)
// --------------------------------------------------------------------
{
	pos.push_back( _vec<double>(x, y, z) );
	frc.push_back( _vec<double>(0, 0, 0) );
	hyd.push_back( i_hyd );
	return;
}


// void _beads::set_beads
// // --------------------------------------------------------------------
// //
// // Function: randomply place beads in the system (bcs) where a cargo with 
// // 			 radius cg_rad is located at cg_cen
// // 
// (
// 	int 			N_bead,		// total number of beads
// 	_bcs 			&bcs  ,		// boundary condition
// 	_grid 			&grid  ,	// grid
// 	_cargo 			&cargo,		// another beads object (cargo)
// 	_fgnup 			&fgnup		// another beads object (cargo)
// )
// // --------------------------------------------------------------------
// {
// 	double x_min = -bcs.R_pore;
// 	double x_max =  bcs.R_pore;
// 	double y_min = -bcs.R_pore;
// 	double y_max =  bcs.R_pore;
// 	double z_min =  bcs.cy_btm;
// 	double z_max =  bcs.cy_top;

// 	double pore_rad_2 = bcs.R_pore * bcs.R_pore;
// 	double cg_rad_2   = cargo.R_cargo * cargo.R_cargo;

// 	// randomly place beads
// 	for( int i=0; i<N_bead; i++ ){
// 		int flag = 0;
// 		while( flag == 0 ){
// 			double x = x_min + (x_max - x_min) * (unif(mt)+1)/2;
// 			double y = y_min + (y_max - y_min) * (unif(mt)+1)/2;
// 			double z = z_min + (z_max - z_min) * (unif(mt)+1)/2;
// 			double dist_ax_2 = x*x + y*y;
// 			_vec<double> r_cg = _vec<double>(x,y,z) - cargo.center;
// 			if( dist_ax_2 < pore_rad_2 && r_cg.sqr() > cg_rad_2 ){
// 				flag = 1;
// 				pos.push_back(_vec<double>(x,y,z));
// 				frc.push_back(_vec<double>(0,0,0));
// 				// hyd.push_back(0.0);
// 				hyd_inv_id.push_back(-1);
// 			}
// 		}
// 	}

// 	init_pair();

// 	// equilibriate
// 	fgnup.make_list(bcs,grid);
// 	cargo.make_list(bcs,grid,fgnup);
// 	this->make_list(bcs,grid,fgnup,cargo);

// 	for( int t=0; t<EQUIV_STEP; t++ ){
// 		if( t%10==0 ){
// 			fgnup.make_list(bcs,grid);
// 			cargo.make_list(bcs,grid,fgnup);
// 			this->make_list(bcs,grid,fgnup,cargo);
// 		}

// 		init_force();
// 		calc_force(bcs,fgnup,cargo);
// 		move(bcs);
// 		// char fname[64];
// 		// sprintf( fname, "test_%d.vtk", int(t) );
// 		// output(fname);
// 	}

// 	// std::cout << "finished setting " << pos.size() << " beads by random placement" << std::endl;

// 	return;
// }


void _beads::set_beads_asc
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	int N_bead;
	double x, y, z;
	double val;
	int tmp_i;

	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<4; i++ ) std::getline(ifs, str); // skip 4 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		pos.push_back( _vec<double>(x, y, z) );
		frc.push_back( _vec<double>(0, 0, 0) );
	}

	std::getline(ifs, str); // skip 1 line
	for( int i=0; i<N_bead; i++ ) std::getline(ifs, str); // skip N_bead lines
	std::getline(ifs, str); // skip 1 line
	for( int i=0; i<N_bead; i++ ) std::getline(ifs, str); // skip N_bead lines

	int count = 0;
	std::getline(ifs, str); // skip 1 line
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf", &val );
		if( val != 0.0 ){
			hyd_id.push_back(i);
			hyd.push_back(val);
			hyd_inv_id.push_back(count);
			count ++;
		}else{
			hyd_inv_id.push_back(-1);
		}
	}

	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%d", &tmp_i );
		if( hyd_inv_id[i] != -1 ){
			hyd_state.push_back(tmp_i);
		}
	}

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		_vec<double> tmp(x, y, z);
		frc.push_back(tmp);
	}

	init_pair();
	// std::cout << "finished setting " << pos.size() << " beads from " << ifs_name << std::endl;
}



void _beads::set_beads_bin
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*fname
)
// --------------------------------------------------------------------
{
	int 	N_bead;
	double 	x, y, z;
	double 	val;
	int 	dum;
	float 	tmp_x, tmp_y, tmp_z, tmp;
	int 	tmp_i;

	char ifs_name[64];
	sprintf( ifs_name, "%s", fname );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<4; i++ ) std::getline(ifs, str); // skip 4 line	
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );

	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_x, sizeof(float)); SwapEnd(tmp_x);
		ifs.read((char *) &tmp_y, sizeof(float)); SwapEnd(tmp_y);
		ifs.read((char *) &tmp_z, sizeof(float)); SwapEnd(tmp_z);
		pos.push_back(_vec<double>(tmp_x, tmp_y, tmp_z));
	}

	int count = 0;
	for( int i=0; i<3; i++ ) std::getline(ifs, str); // skip 3 lines
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp, sizeof(float)); SwapEnd(tmp);
		if( tmp != 0.0 ){
			hyd_id.push_back(i);
			hyd.push_back(tmp);
			hyd_inv_id.push_back(count);
			count ++;
		}else{
			hyd_inv_id.push_back(-1);
		}
	}

	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 lines
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_i, sizeof(int)); SwapEnd(tmp_i);
		if( hyd_inv_id[i] != -1 ){
			hyd_state.push_back(tmp_i);
		}
	}

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_x, sizeof(float)); SwapEnd(tmp_x);
		ifs.read((char *) &tmp_y, sizeof(float)); SwapEnd(tmp_y);
		ifs.read((char *) &tmp_z, sizeof(float)); SwapEnd(tmp_z);
		_vec<double> tmp_f(tmp_x, tmp_y, tmp_z);
		tmp_f /= FRC_PN;
		frc.push_back(tmp_f);
	}

	init_pair();
	// std::cout << "finished setting " << pos.size() << " beads from " << ifs_name << std::endl;
}


void _beads::set_beads
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	_beads::set_beads_bin(ifs_name);
	return;
}

void _beads::init_pair
// --------------------------------------------------------------------
//
// Function: initialize the pair list
// 
(
)
// --------------------------------------------------------------------
{
	// auto_pair
	for( int i=0; i<pos.size(); i++ ){
		auto_pair_rp.push_back(std::vector<int>());
		auto_pair_rp[i].reserve(PAIR_RESERVE);
	}
	for( int i=0; i<hyd.size(); i++ ){
		auto_pair_fg.push_back(std::vector<int>());
		auto_pair_fg[i].reserve(PAIR_RESERVE);
	}

	// cross_pair
	for( int n=0; n<cross_pair_rp.size(); n++ ){
		for( int i=0; i<pos.size(); i++ ){
			cross_pair_rp[n].push_back(std::vector<int>());
			cross_pair_rp[n][i].reserve(PAIR_RESERVE);
		}
		for( int i=0; i<hyd.size(); i++ ){
			cross_pair_fg[n].push_back(std::vector<int>());
			cross_pair_fg[n][i].reserve(PAIR_RESERVE);
		}
	}
}


void _beads::init
// --------------------------------------------------------------------
//
// Function: initialize the system
// 
(
)
// --------------------------------------------------------------------
{
	hyd_adj.reserve(ADJ_AUTO_RESERVE);
}

void _beads::init
// --------------------------------------------------------------------
//
// Function: initialize the system
// 
(
	_beads &o_beads_0
)
// --------------------------------------------------------------------
{
	init();
	init_cross_list(1);
}

void _beads::init
// --------------------------------------------------------------------
//
// Function: initialize the system
// 
(
	_beads &o_beads_0,
	_beads &o_beads_1
)
// --------------------------------------------------------------------
{
	init();
	init_cross_list(2);
}

void _beads::init
// --------------------------------------------------------------------
//
// Function: initialize the system
// 
(
	_grid	&grid
)
// --------------------------------------------------------------------
{
	sync_grid(grid);
	init();
}

void _beads::init
// --------------------------------------------------------------------
//
// Function: initialize the system
// 
(
	_grid  &grid,
	_beads &o_beads_0
)
// --------------------------------------------------------------------
{
	sync_grid(grid);
	init(o_beads_0);
}


void _beads::init
// --------------------------------------------------------------------
//
// Function: initialize the system
// 
(
	_grid  &grid,
	_beads &o_beads_0,
	_beads &o_beads_1
)
// --------------------------------------------------------------------
{
	sync_grid(grid);
	init(o_beads_0,o_beads_1);
}



void _beads::init_cross_list
// --------------------------------------------------------------------
//
// Function: initialize cross pair list
(
	int 	n_obj 		// number of the _beads objects making cross-list with
)
// --------------------------------------------------------------------
{
	for( int i=0; i<n_obj; i++ ){
		std::vector< std::vector<int> > tmp;
		cross_pair_rp.push_back(tmp);
		cross_pair_fg.push_back(tmp);
		hyd_adj_cross.push_back(std::vector<bool>());
		hyd_adj_cross[i].reserve(ADJ_CROS_RESERVE);
	}
}

void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list
// 
(
)
// --------------------------------------------------------------------
{
	make_list_auto_rp();
	make_list_auto_fg();
}

void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	make_list_auto_rp(bcs);
	make_list_auto_fg(bcs);
}

void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list
// 
(
	_bcs 	&bcs,
	_beads	&o_beads_0
)
// --------------------------------------------------------------------
{
	make_list_auto_rp(bcs);
	make_list_cross_rp(bcs, o_beads_0, 0);
	make_list_cross_fg(bcs, o_beads_0, 0);
}

void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list
// 
(
	_bcs 	&bcs,
	_beads	&o_beads_0,
	_beads	&o_beads_1
)
// --------------------------------------------------------------------
{
	make_list_auto_rp(bcs);
	make_list_cross_rp(bcs, o_beads_0, 0);
	make_list_cross_fg(bcs, o_beads_0, 0);
	make_list_cross_rp(bcs, o_beads_1, 1);
	make_list_cross_fg(bcs, o_beads_1, 1);
}


void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list using grid list
// 
(
	_bcs 	&bcs,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	grid.sort_beads(this);
	make_list_auto_rp(bcs, grid);
	make_list_auto_fg(bcs, grid);
}

void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list using grid list
// 
(
	_bcs 	&bcs,
	_grid 	&grid,
	_beads	&o_beads_0
)
// --------------------------------------------------------------------
{
	grid.sort_beads(this);
	make_list_auto_rp (bcs, grid);
	make_list_cross_rp(bcs, grid, o_beads_0, 0);
	make_list_cross_fg(bcs, grid, o_beads_0, 0);
}

void _beads::make_list
// --------------------------------------------------------------------
//
// Function: make pair list using grid list
// 
(
	_bcs 	&bcs,
	_grid 	&grid,
	_beads	&o_beads_0,
	_beads	&o_beads_1
)
// --------------------------------------------------------------------
{
	grid.sort_beads(this);
	make_list_auto_rp (bcs, grid);
	make_list_cross_rp(bcs, grid, o_beads_0, 0);
	make_list_cross_fg(bcs, grid, o_beads_0, 0);
	make_list_cross_rp(bcs, grid, o_beads_1, 1);
	make_list_cross_fg(bcs, grid, o_beads_1, 1);
}

// double _beads::k_roll()
// // --------------------------------------------------------------------
// //
// // Function: return random number in [0,1]
// // 
// // --------------------------------------------------------------------
// {
// 	return (unif(mt)+1.0)/2.0;
// }

// bool _beads::find_elem
// // --------------------------------------------------------------------
// //
// // Function: check exisitence of the element in an array
// // 			 TRUE  if the element exists
// // 			 FLASE if the element does NOT exist
// // 
// (
// 	std::vector<int>	arr,
// 	int 				elm
// )
// // --------------------------------------------------------------------
// {
// 	return std::find(arr.begin(), arr.end(), elm) != arr.end();
// }


void _beads::make_list_auto_rp
// --------------------------------------------------------------------
//
// Function: make auto pair list
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		auto_pair_rp[i].clear();
		auto_pair_rp[i].reserve(PAIR_RESERVE);
	}

	for( int i=0; i<pos.size(); i++ ){
		_vec<double> pos_key = pos[i];
		int hyd_key = hyd_inv_id[i];
		for( int j=i+1; j<pos.size(); j++ ){
			_vec<double> dr = pos[j] - pos_key;
			if( hyd_key!=-1 && hyd_inv_id[j]!=-1 ) continue; //remove if both are fg
			if( dr.sqr() < cut_rp_list_2 ){
				auto_pair_rp[i].push_back(j);
			}
		}
	}
}



void _beads::make_list_auto_fg
// --------------------------------------------------------------------
//
// Function: make auto pair list
//
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		auto_pair_fg[i].clear();
		auto_pair_fg[i].reserve(PAIR_RESERVE);
	}

	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];
		for( int j=i+1; j<hyd.size(); j++ ){
			int jj = hyd_id[j];
			_vec<double> dr = pos[jj] - pos_key;
			if( dr.sqr() < cut_fg_list_2 ){
				auto_pair_fg[i].push_back(jj); // hyd id -> bead id
			}
		}
	}
}

void _beads::make_list_auto_rp
// --------------------------------------------------------------------
//
// Function: make auto pair list
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		auto_pair_rp[i].clear();
		auto_pair_rp[i].reserve(PAIR_RESERVE);
	}

	for( int i=0; i<pos.size(); i++ ){
		_vec<double> pos_key = pos[i];
		int hyd_key = hyd_inv_id[i];
		for( int j=i+1; j<pos.size(); j++ ){
			_vec<double> dr = pos[j] - pos_key;
			bcs.adjust_periodic(&dr);
			if( hyd_key!=-1 && hyd_inv_id[j]!=-1 ) continue; //remove if both are fg
			if( dr.sqr() < cut_rp_list_2 ){
				auto_pair_rp[i].push_back(j);
			}
		}
	}
}

void _beads::make_list_auto_fg
// --------------------------------------------------------------------
//
// Function: make auto pair list
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		auto_pair_fg[i].clear();
		auto_pair_fg[i].reserve(PAIR_RESERVE);
	}

	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];
		for( int j=i+1; j<hyd.size(); j++ ){
			int jj = hyd_id[j];
			_vec<double> dr = pos[jj] - pos_key;
			bcs.adjust_periodic(&dr);
			if( dr.sqr() < cut_fg_list_2 ){
				auto_pair_fg[i].push_back(jj); // hyd id -> bead id
			}
		}
	}
}

void _beads::make_list_auto_rp
// --------------------------------------------------------------------
//
// Function: make auto pair list using grid list
// 
(
	_bcs 	&bcs,
	_grid 	&grid
)
// --------------------------------------------------------------------
{

	for( int i=0; i<pos.size(); i++ ){
		auto_pair_rp[i].clear();
		auto_pair_rp[i].reserve(PAIR_RESERVE);
	}

	// among neighboring grids (including itself)
	for( int bi=0; bi<grid.N_grid; bi++ ){
	for( int bj : grid.grid_nbr[bi] ){ 	//grid_nbr include itself

		int N_bead_i = binned_beads[bi].size();
		int N_bead_j = binned_beads[bj].size();

		for( int i=0; i<N_bead_i; i++ ){
			int ii = binned_beads[bi][i];
			_vec<double> pos_key = pos[ii];
			int hyd_key = hyd_inv_id[ii];

		for( int j=0; j<N_bead_j; j++ ){
			int jj = binned_beads[bj][j];

			if( ii >= jj ) continue;
			if( hyd_key!=-1 && hyd_inv_id[jj]!=-1 ) continue; //remove if both are fg

			_vec<double> dr = pos[jj] - pos_key;
			bcs.adjust_periodic(&dr);

			double dr_sqr = dr.sqr();

			if( dr_sqr < cut_rp_list_2 ){
				auto_pair_rp[ii].push_back(jj);
			}
		}
		}
	}
	}

	for( int i=0; i<pos.size(); i++ ){
		std::sort(auto_pair_rp[i].begin(),auto_pair_rp[i].end());
	}
}


void _beads::make_list_auto_fg
// --------------------------------------------------------------------
//
// Function: make auto pair list using grid list
// 
(
	_bcs 	&bcs,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		auto_pair_fg[i].clear();
		auto_pair_fg[i].reserve(PAIR_RESERVE);
	}

	// among neighboring grids (including itself)
	for( int bi=0; bi<grid.N_grid; bi++ ){
	for( int bj : grid.grid_nbr[bi] ){ 	//grid_nbr include itself

		int N_bead_i = binned_beads[bi].size();
		int N_bead_j = binned_beads[bj].size();

		for( int i=0; i<N_bead_i; i++ ){
			int ii = binned_beads[bi][i];
			_vec<double> pos_key = pos[ii];

		for( int j=0; j<N_bead_j; j++ ){
			int jj = binned_beads[bj][j];

			if( ii >= jj ) continue;

			_vec<double> dr = pos[jj] - pos_key;
			bcs.adjust_periodic(&dr);

			double dr_sqr = dr.sqr();

			if( dr_sqr < cut_fg_list_2 ){
				int fg_i = hyd_inv_id[ii];
				int fg_j = hyd_inv_id[jj];
				if( fg_i != -1 && fg_j != -1 ){
					auto_pair_fg[fg_i].push_back(jj); // hyd id -> bead id
				}
			}
		}
		}
	}
	}

	for( int i=0; i<hyd.size(); i++ ){
		std::sort(auto_pair_fg[i].begin(),auto_pair_fg[i].end());
	}
}

void _beads::make_list_cross_rp
// --------------------------------------------------------------------
//
// Function: make cross pair list (for repulsion)
// 
(
	_bcs 	&bcs, 
	_beads 	&o_beads,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		cross_pair_rp[cross_id][i].clear();
		cross_pair_rp[cross_id][i].reserve(PAIR_RESERVE);
	}
	
	// std::vector<_vec<double> > o_pos = o_beads.pos;
	// std::vector<int> o_hyd_inv_id = o_beads.hyd_inv_id;

	for( int i=0; i<pos.size(); i++ ){
		_vec<double> pos_key = pos[i];
		int hyd_key = hyd_inv_id[i];
		for( int j=0; j<o_beads.pos.size(); j++ ){
			_vec<double> dr = o_beads.pos[j] - pos_key;
			if( hyd_key!=-1 && o_beads.hyd_inv_id[j]!=-1 ) continue; //remove if both are fg
			bcs.adjust_periodic(&dr);
			if( dr.sqr() < cut_rp_list_2 ){
				cross_pair_rp[cross_id][i].push_back(j);
			}
		}
	}
}


void _beads::make_list_cross_fg
// --------------------------------------------------------------------
//
// Function: make cross pair list (for hydrophobic)
// 
(
	_bcs 	&bcs, 
	_beads 	&o_beads,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		cross_pair_fg[cross_id][i].clear();
		cross_pair_fg[cross_id][i].reserve(PAIR_RESERVE);
	}

	// std::vector<_vec<double> > o_pos = o_beads.pos;

	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];
		for( int j=0; j<o_beads.hyd.size(); j++ ){
			int jj = o_beads.hyd_id[j];
			_vec<double> dr = o_beads.pos[jj] - pos_key;
			bcs.adjust_periodic(&dr);
			if( dr.sqr() < cut_fg_list_2 ){
				cross_pair_fg[cross_id][i].push_back(jj); // hyd id -> bead id
			}
		}
	}
}



void _beads::make_list_cross_rp
// --------------------------------------------------------------------
//
// Function: make cross pair list (for repulsion) using grid list
// 
(
	_bcs 	&bcs, 
	_grid 	&grid,
	_beads 	&o_beads,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	// std::vector<_vec<double> > o_pos = o_beads.pos;
	// std::vector<int> o_hyd_inv_id = o_beads.hyd_inv_id;

	for( int i=0; i<pos.size(); i++ ){
		cross_pair_rp[cross_id][i].clear();
		cross_pair_rp[cross_id][i].reserve(PAIR_RESERVE);
	}

	// among neighboring grids (including itself)
	for( int bi=0; bi<grid.N_grid; bi++ ){
	for( int bj : grid.grid_nbr[bi] ){ 	// grid_nbr include itself

		int   N_bead_i =   		 binned_beads[bi].size();
		int o_N_bead_j = o_beads.binned_beads[bj].size();

		for( int i=0; i<N_bead_i; i++ ){
			int ii = binned_beads[bi][i];
			_vec<double> pos_key = pos[ii];
			int hyd_key = hyd_inv_id[ii];

		for( int j=0; j<o_N_bead_j; j++ ){
			int jj = o_beads.binned_beads[bj][j];

			if( hyd_key!=-1 && o_beads.hyd_inv_id[jj]!=-1 ) continue; //remove if both are fg

			_vec<double> dr = o_beads.pos[jj] - pos_key;
			bcs.adjust_periodic(&dr);

			double dr_sqr = dr.sqr();
			if( dr_sqr < cut_rp_list_2 ){
				cross_pair_rp[cross_id][ii].push_back(jj);
			}

		}
		}
	}
	}

	for( int i=0; i<pos.size(); i++ ){
		std::sort(cross_pair_rp[cross_id][i].begin(),cross_pair_rp[cross_id][i].end());
	}
}


void _beads::make_list_cross_fg
// --------------------------------------------------------------------
//
// Function: make cross pair list (for hydrophobic) using grid list
// 
(
	_bcs 	&bcs, 
	_grid 	&grid,
	_beads 	&o_beads,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		cross_pair_fg[cross_id][i].clear();
		cross_pair_fg[cross_id][i].reserve(PAIR_RESERVE);
	}

	// std::vector<_vec<double> > o_pos = o_beads.pos;

	// among neighboring grids (including itself)
	for( int bi=0; bi<grid.N_grid; bi++ ){
	for( int bj : grid.grid_nbr[bi] ){ 	// grid_nbr include itself

		int   N_bead_i =   		 binned_beads[bi].size();
		int o_N_bead_j = o_beads.binned_beads[bj].size();

		for( int i=0; i<N_bead_i; i++ ){
			int ii = binned_beads[bi][i];
			_vec<double> pos_key = pos[ii];

		for( int j=0; j<o_N_bead_j; j++ ){
			int jj = o_beads.binned_beads[bj][j];

			_vec<double> dr = o_beads.pos[jj] - pos_key;
			bcs.adjust_periodic(&dr);

			double dr_sqr = dr.sqr();
			if( dr_sqr < cut_fg_list_2 ){
				int fg_i = hyd_inv_id[ii];
				int fg_j = o_beads.hyd_inv_id[jj];
				if( fg_i != -1 && fg_j != -1 ){
					cross_pair_fg[cross_id][fg_i].push_back(jj); // hyd id -> bead id
				}
			}
		}
		}
	}
	}

	for( int i=0; i<hyd.size(); i++ ){
		std::sort(cross_pair_fg[cross_id][i].begin(),cross_pair_fg[cross_id][i].end());
	}
}

_vec<double> _beads::rp_force
// --------------------------------------------------------------------
//
// Function: formula for the steric repulsion
// 
(
	double 			dist_2,
	_vec<double> 	dr
)
// --------------------------------------------------------------------
{
	double dist_4 = dist_2 * dist_2;
	double dist_6 = dist_2 * dist_2 * dist_2;
	double core_1 = a_shift_rp + dist_6;
	double core_2 = core_1 * core_1;
	double core_3 = core_1 * core_1 * core_1;
	double force  = coef_rp_12 * dist_4 / core_3 + coef_rp_6 * dist_4 / core_2;
	return force * dr;
}

_vec<double> _beads::fg_force
// --------------------------------------------------------------------
//
// Function: formula for the hydrophobic force
// 
(
	double 			dist_2,
	_vec<double> 	dr 	  ,
	double 			prod_hyd
)
// --------------------------------------------------------------------
{
	double dist_4 = dist_2 * dist_2;
	double dist_6 = dist_2 * dist_2 * dist_2;
	double core_1 = a_shift_fg + dist_6;
	double core_2 = core_1 * core_1;
	double core_3 = core_1 * core_1 * core_1;
	double force  = coef_fg_12 * dist_4 / core_3 + coef_fg_6 * dist_4 / core_2;
	return prod_hyd * force * dr;
}


void _beads::calc_force_rp
// --------------------------------------------------------------------
//
// Function: calculate repulsive force (auto)
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		_vec<double> pos_key = pos[i];
		_vec<double> frc_key(0.,0.,0.);

		for( int j=0; j<auto_pair_rp[i].size(); j++ ){
			int jj = auto_pair_rp[i][j];
			_vec<double> dr = pos_key - pos[jj];

			double dist_2 = dr.sqr();

			if( dist_2 < cut_rp_2 ){
				_vec<double> df = rp_force(dist_2, dr);
				frc_key	+= df;
				frc[jj] -= df;
			}
		}
		frc[i] += frc_key;
	}
}

void _beads::calc_force_rp
// --------------------------------------------------------------------
//
// Function: calculate repulsive force (auto)
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		_vec<double> pos_key = pos[i];
		_vec<double> frc_key(0,0,0);

		for( int j=0; j<auto_pair_rp[i].size(); j++ ){
			int jj = auto_pair_rp[i][j];
			_vec<double> dr = pos_key - pos[jj];

			bcs.adjust_periodic(&dr);
			double dist_2 = dr.sqr();

			if( dist_2 < cut_rp_2 ){
				_vec<double> df = rp_force(dist_2, dr);
				frc_key += df;
				frc[jj] -= df;
			}
		}
		frc[i] += frc_key;
	}
}

void _beads::calc_force_fg
// --------------------------------------------------------------------
//
// Function: calculate hydrophobic force (auto)
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];
		_vec<double> frc_key(0,0,0);

		for( int j=0; j<auto_pair_fg[i].size(); j++ ){
			int jj = auto_pair_fg[i][j]; // jj is already a bead id
			_vec<double> dr = pos_key - pos[jj];

			double dist_2 = dr.sqr();

			if( dist_2 < cut_fg_2 ){
				int    fg_j     = hyd_inv_id[jj]; // bead id -> hyd id
				double prod_hyd = hyd[i] * hyd[fg_j];
				_vec<double> df = fg_force(dist_2, dr, prod_hyd);
				frc_key += df;
				frc[jj] -= df;
			}
		}
		frc[ii] += frc_key;
	}
}

void _beads::calc_force_fg
// --------------------------------------------------------------------
//
// Function: calculate hydrophobic force (auto)
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];
		_vec<double> frc_key(0,0,0);

		for( int j=0; j<auto_pair_fg[i].size(); j++ ){
			int jj = auto_pair_fg[i][j]; // jj is already a bead id
			_vec<double> dr = pos_key - pos[jj];

			bcs.adjust_periodic(&dr);
			double dist_2 = dr.sqr();

			if( dist_2 < cut_fg_2 ){
				int    fg_j     = hyd_inv_id[jj]; // bead id -> hyd id
				double prod_hyd = hyd[i] * hyd[fg_j];
				_vec<double> df = fg_force(dist_2, dr, prod_hyd);
				frc_key += df;
				frc[jj] -= df;
			}
		}
		frc[ii] += frc_key;
	}
}


void _beads::calc_force_rp
// --------------------------------------------------------------------
//
// Function: calculate repulsive force (cross)
// 
(
	_bcs 	&bcs,
	_beads 	&o_beads,	// the other _bead object
	int 	cross_id	// ID of the cross-object
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		_vec<double> pos_key = pos[i];
		_vec<double> frc_key(0,0,0);

		for( int j=0; j<cross_pair_rp[cross_id][i].size(); j++ ){

			int jj = cross_pair_rp[cross_id][i][j];
			_vec<double> dr = pos_key - o_beads.pos[jj];
			bcs.adjust_periodic(&dr);
			double dist_2 = dr.sqr();

			if( dist_2 < cut_rp_2 ){
				_vec<double> df = rp_force(dist_2, dr);
				frc_key 	   += df;
				o_beads.frc[jj]-= df;
			}
		}
		frc[i] += frc_key;
	}
}

void _beads::calc_force_fg
// --------------------------------------------------------------------
//
// Function: calculate hydrophobic force (cross)
// 
(
	_bcs 	&bcs,
	_beads 	&o_beads,	// the other _bead object
	int 	cross_id	// ID of the cross-object
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];
		_vec<double> frc_key(0,0,0);

		for( int j=0; j<cross_pair_fg[cross_id][i].size(); j++ ){

			int jj = cross_pair_fg[cross_id][i][j]; // this is already bead id
			_vec<double> dr = pos_key - o_beads.pos[jj];
			bcs.adjust_periodic(&dr);
			double dist_2 = dr.sqr();

			if( dist_2 < cut_fg_2 ){
				int    fg_j     = o_beads.hyd_inv_id[jj]; // bead id -> hyd id
				double prod_hyd = hyd[i] * o_beads.hyd[fg_j];
				_vec<double> df = fg_force(dist_2, dr, prod_hyd);
				frc_key 	   += df;
				o_beads.frc[jj]-= df;
			}
		}
		frc[ii] += frc_key;
	}
}


void _beads::init_force
// --------------------------------------------------------------------
//
// Function: initialize force
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<frc.size(); i++ ){
		frc[i] = _vec<double> (0.,0.,0.);
	}
}


void _beads::calc_force
// --------------------------------------------------------------------
//
// Function: calculate force
// 
(
)
// --------------------------------------------------------------------
{
	init_force();
	calc_force_rp();
	calc_force_fg();
}

void _beads::calc_force
// --------------------------------------------------------------------
//
// Function: calculate force
// 
(
	_bcs 	&bcs 		// boundary 
)
// --------------------------------------------------------------------
{
	init_force();
	calc_force_rp(bcs);
	calc_force_fg(bcs);
	bcs.calc_force_wall(this);
}

void _beads::calc_force
// --------------------------------------------------------------------
//
// Function: calculate force between this and the other _bead object
//
(
	_bcs 	&bcs, 		// boundary 
	_beads 	&o_beads_0	// the other _bead object
)
// --------------------------------------------------------------------
{
	init_force();
	calc_force_rp(bcs);
	calc_force_fg(bcs);
	calc_force_rp(bcs, o_beads_0, 0);
	calc_force_fg(bcs, o_beads_0, 0);
	bcs.calc_force_wall(this);
}

// void _beads::calc_force
// // --------------------------------------------------------------------
// //
// // Function: calculate force between this and the other _bead object
// // 
// (
// 	_bcs 	&bcs 	  , 	// boundary 
// 	_beads 	&o_beads_0,		// the other _bead object
// 	_beads 	&o_beads_1		// the other _bead object
// )
// // --------------------------------------------------------------------
// {
// 	init_force();
// 	calc_force_rp( bcs );
// 	calc_force_fg( bcs );
// 	// calc_force_rpfg( bcs );
// 	calc_force_rp( bcs, o_beads_0, 0 );
// 	calc_force_fg( bcs, o_beads_0, 0 );
// 	// calc_force_rpfg( bcs, o_beads_0, 0 );
// 	calc_force_rp( bcs, o_beads_1, 1 );
// 	calc_force_fg( bcs, o_beads_1, 1 );
// 	// calc_force_rpfg( bcs, o_beads_1, 1 );
// 	bcs.calc_force_wall(this);
// 	return;
// }

void _beads::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		pos[i].x = pos[i].x + coeff_ex * frc[i].x + rand_std * unif(mt); 
		pos[i].y = pos[i].y + coeff_ex * frc[i].y + rand_std * unif(mt); 
		pos[i].z = pos[i].z + coeff_ex * frc[i].z + rand_std * unif(mt); 
	}
}


void _beads::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		pos[i].x = pos[i].x + coeff_ex * frc[i].x + rand_std * unif(mt); 
		pos[i].y = pos[i].y + coeff_ex * frc[i].y + rand_std * unif(mt); 
		pos[i].z = pos[i].z + coeff_ex * frc[i].z + rand_std * unif(mt); 
		// std::cout << frc[i].x << " " << unif(mt) << std::endl;
	}

// std::cout << unif(mt) << std::endl; exit(1);
	// std::cout << coeff_ex << " " << rand_std << std::endl;
	// std::cout << coeff_ex/FRC_PN << " " << rand_std << std::endl;
	// exit(1);

	bcs.move_at_boundary(this);
}


void _beads::set_hyd_adj
// --------------------------------------------------------------------
//
// Function: initially set 1d array for the adjacent matrix
//
(
)
// --------------------------------------------------------------------
{
	// std::cout << "create1" << std::endl;
	for( int i=0; i<hyd.size(); i++ ){
	for( int j=i+1; j<hyd.size(); j++ ){
		// int id = i*(hyd.size()-1) - (i*(i-1))/2 + (j-i-1);
		hyd_adj.push_back(0);
	}}
}

void _beads::set_hyd_adj
// --------------------------------------------------------------------
//
// Function: initially set 1d array for the adjacent matrix
//
(
	int 	o_beads_size, 
	int 	cross_id
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
	for( int j=0; j<o_beads_size; j++ ){
		// int id = i*o_beads.hyd.size() + j;
		hyd_adj_cross[cross_id].push_back(0);
	}}
}


void _beads::init_hyd_adj_auto
// --------------------------------------------------------------------
//
// Function: initiate adjcent matrix (set all elements zero)
// 
(
)
// --------------------------------------------------------------------
{
	if( hyd_adj.size() == 0 ) set_hyd_adj(); // initialize if the array is empty
	for( int i=0; i<hyd.size(); i++ ){
	for( int j=i+1; j<hyd.size(); j++ ){
		int id = i*(hyd.size()-1) - (i*(i-1))/2 + (j-i-1);
		hyd_adj[id] = 0;
	}}
}

void _beads::init_hyd_adj_cross
// --------------------------------------------------------------------
//
// Function: initiate adjcent matrix (set all elements zero)
// 
(
	int 	o_beads_size,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	if( hyd_adj_cross[cross_id].size() == 0 ) set_hyd_adj(o_beads_size, cross_id); // initialize if the array is empty
	for( int i=0; i<hyd.size(); i++ ){
	for( int j=0; j<o_beads_size; j++ ){
		// int id = i*hyd.size() + j;
		int id = i*o_beads_size + j;
		hyd_adj_cross[cross_id][id] = 0;
	}}
}

void _beads::make_hyd_adj
// --------------------------------------------------------------------
//
// Function: make adjcent matrix before output
// 
(
)
// --------------------------------------------------------------------
{
	init_hyd_adj_auto();
	make_hyd_adj_auto();
}

void _beads::make_hyd_adj
// --------------------------------------------------------------------
//
// Function: make adjcent matrix before output
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	init_hyd_adj_auto();
	make_hyd_adj_auto(bcs);
}

void _beads::make_hyd_adj
// --------------------------------------------------------------------
//
// Function: make adjcent matrix before output
// 
(
	_bcs 	&bcs,
	_beads 	&o_beads_0
)
// --------------------------------------------------------------------
{
	init_hyd_adj_auto();
	init_hyd_adj_cross(o_beads_0.hyd.size(), 0);
	make_hyd_adj_auto(bcs);
	make_hyd_adj_cross(bcs, o_beads_0, 0);
}

void _beads::make_hyd_adj_auto
// --------------------------------------------------------------------
//
// Function:
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];

		for( int j=i+1; j<hyd.size(); j++ ){
			int jj = hyd_id[j];

			_vec<double> dr = pos_key - pos[jj];
			double dist_2 = dr.sqr();

			if( dist_2 < cut_fg_2 ){
				int id = i*(hyd.size()-1) - (i*(i-1))/2 + (j-i-1);
				hyd_adj[id] = 1;
			}
		}
	}
}

void _beads::make_hyd_adj_auto
// --------------------------------------------------------------------
//
// Function:
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];

		for( int j=i+1; j<hyd.size(); j++ ){
			int jj = hyd_id[j];

			_vec<double> dr = pos_key - pos[jj];
			bcs.adjust_periodic(&dr);
			double dist_2 = dr.sqr();

			if( dist_2 < cut_fg_2 ){
				int id = i*(hyd.size()-1) - (i*(i-1))/2 + (j-i-1);
				hyd_adj[id] = 1;
			}
		}
	}
}


void _beads::make_hyd_adj_cross
// --------------------------------------------------------------------
//
// Function:
// 
(
	_bcs 	&bcs,
	_beads 	&o_beads,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int ii = hyd_id[i];
		_vec<double> pos_key = pos[ii];

		for( int j=0; j<o_beads.hyd.size(); j++ ){
			int jj = o_beads.hyd_id[j];

			_vec<double> dr = pos_key - o_beads.pos[jj];
			bcs.adjust_periodic(&dr);
			double dist_2 = dr.sqr();

			if( dist_2 < cut_fg_2 ){
				// int id = i*hyd.size() + j;
				int id = i*o_beads.hyd.size() + j;
				hyd_adj_cross[cross_id][id] = 1;
			}
		}
	}
}


void _beads::init_hyd_state
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd_state.size(); i++ ){
		hyd_state[i] = 0;
	}
}

void _beads::calc_hyd_state_auto
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int count = 0;
		for( int j=i+1; j<hyd.size(); j++ ){
			int id = i*(hyd.size()-1) - (i*(i-1))/2 + (j-i-1);
			if( hyd_adj[id] == 1 ){
				count ++;
				hyd_state[j] ++;
			}
		}
		hyd_state[i] += count;
	}
}

void _beads::calc_hyd_state_cross
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_beads 	&o_beads,
	int 	cross_id
)
// --------------------------------------------------------------------
{
	for( int i=0; i<hyd.size(); i++ ){
		int count = 0;
		for( int j=0; j<o_beads.hyd.size(); j++ ){
			// int id = i*hyd.size() + j;
			int id = i*o_beads.hyd.size() + j;
			count += hyd_adj_cross[cross_id][id];
		}
		hyd_state[i] += count;
	}

	for( int j=0; j<o_beads.hyd.size(); j++ ){
		int count = 0;
		for( int i=0; i<hyd.size(); i++ ){
			// int id = i*hyd.size() + j;
			int id = i*o_beads.hyd.size() + j;
			count += hyd_adj_cross[cross_id][id];
		}
		o_beads.hyd_state[j] += count;
	}
}

void _beads::calc_hyd_state
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	init_hyd_state();
	calc_hyd_state_auto();
}


void _beads::calc_hyd_state
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_beads 	&o_beads_0
)
// --------------------------------------------------------------------
{
	init_hyd_state();
	calc_hyd_state_auto();
	calc_hyd_state_cross(o_beads_0, 0);
}

void _beads::output_asc
// --------------------------------------------------------------------
//
// Function: VTK output by ascii mode
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s.vtk", ofs_name_in );

	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "beads\n" );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET UNSTRUCTURED_GRID\n" );
	fprintf( f_out, "POINTS %lu float\n", this->pos.size() );
	for( int i=0; i<this->pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", this->pos[i].x, this->pos[i].y, this->pos[i].z );
	}
	fprintf( f_out, "CELLS %lu %lu\n", this->pos.size(), 2*(this->pos.size()) );
	for( int i=0; i<this->pos.size(); i++ ){
		fprintf( f_out, "1 %d\n", i );
	}
	fprintf( f_out, "CELL_TYPES %lu\n", this->pos.size() );
	for( int i=0; i<this->pos.size(); i++ ){
		fprintf( f_out, "1\n" );
	}
	fprintf( f_out, "POINT_DATA %lu\n", this->pos.size() );
	fprintf( f_out, "SCALARS hyd float\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	for( int i=0; i<this->pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			fprintf( f_out, "%lf\n", 0.0 );
		}else{
			fprintf( f_out, "%lf\n", hyd[hyd_inv_id[i]] );
		}
	}

	fprintf( f_out, "SCALARS state int\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	for( int i=0; i<this->pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			fprintf( f_out, "%d\n", -1 );
		}else{
			fprintf( f_out, "%d\n", hyd_state[hyd_inv_id[i]] );
		}
	}

	/* force output is in the unit of pN */
	fprintf( f_out, "VECTORS frc float\n" );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", frc[i].x * FRC_PN, frc[i].y *FRC_PN, frc[i].z * FRC_PN );
	}

	fclose(f_out);
}



void _beads::output_adj
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s_adj.txt", ofs_name_in );

	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	// auto matrix
	fprintf( f_out, "auto: (%lu, %lu)\n", hyd.size(), hyd.size() );

	for( int i=0; i<hyd.size(); i++ ){
		for( int j=i+1; j<hyd.size(); j++ ){
			int id = i*(hyd.size()-1) - (i*(i-1))/2 + (j-i-1);
			// fprintf( f_out, "%d ", static_cast<int>(hyd_adj[id]) );
			if( hyd_adj[id] ) fprintf( f_out, "%d %d\n", i, j );
		}
		// fprintf( f_out, "\n" );
	}

	// cross matrix
	for( int k=0; k<hyd_adj_cross.size(); k++ ){
		int o_beads_size = (int)((hyd_adj_cross[k].size() + 0.001) / hyd.size());
		fprintf( f_out, "\n" );
		fprintf( f_out, "cross %d: (%lu, %d)\n", k, hyd.size(), o_beads_size );

		for( int i=0; i<hyd.size(); i++ ){
			for( int j=0; j<o_beads_size; j++ ){
				int id = i*o_beads_size + j;
				// fprintf( f_out, "%d ", static_cast<int>(hyd_adj_cross[k][id]) );
				if( hyd_adj_cross[k][id] ) fprintf( f_out, "%d %d\n", i, j );
			}
			// fprintf( f_out, "\n" );
		}
	}

	// // free memory since it is too large
	// hyd_adj.shrink_to_fit();
	// for( int i=0; i<hyd_adj_cross.size(); i++ ){
	// 	hyd_adj_cross[i].shrink_to_fit();
	// }

	fclose(f_out);
}

void _beads::output_bin
// --------------------------------------------------------------------
//
// Function: VTK output by binary mode
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s.vtk", ofs_name_in );

	std::ofstream f_out;
	f_out.open(ofs_name, std::ios::binary);
	
	if(!f_out){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	f_out << "# vtk DataFile Version 2.0" 			<<std::endl;
	f_out << "beads"								<<std::endl;
	f_out << "BINARY" 								<<std::endl;
	f_out << "DATASET UNSTRUCTURED_GRID"		 	<<std::endl;
	f_out << "POINTS " << pos.size() << " float" 	<<std::endl;

	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = pos[i].x; SwapEnd(tmp_x);
		float tmp_y = pos[i].y; SwapEnd(tmp_y);
		float tmp_z = pos[i].z; SwapEnd(tmp_z);
		f_out.write((char*)&tmp_x, sizeof(float));
		f_out.write((char*)&tmp_y, sizeof(float));
		f_out.write((char*)&tmp_z, sizeof(float));
	}

	f_out << "CELLS " << pos.size() << " " << 2*pos.size()	<<std::endl;

	for( int i=0; i<pos.size(); i++ ){
		int tmp_n = 1; SwapEnd(tmp_n);
		int tmp_i = i; SwapEnd(tmp_i);
		f_out.write((char*)&tmp_n, sizeof(int));
		f_out.write((char*)&tmp_i, sizeof(int));
	}

	f_out << "CELL_TYPES " << pos.size() 			<<std::endl;

	for( int i=0; i<pos.size(); i++ ){
		int tmp_n = 1; SwapEnd(tmp_n);
		f_out.write((char*)&tmp_n, sizeof(int));
	}

	f_out << "POINT_DATA " << pos.size() 	<< std::endl;
	f_out << "SCALARS hyd float" 			<< std::endl;
	f_out << "LOOKUP_TABLE default"			<< std::endl;

	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			float tmp = 0.0;
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(float));
		}else{
			float tmp = hyd[hyd_inv_id[i]];
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(float));
		}
	}

	f_out << "SCALARS state int" 			<< std::endl;
	f_out << "LOOKUP_TABLE default"			<< std::endl;

	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			int tmp = -1;
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(int));
		}else{
			int tmp = hyd_state[hyd_inv_id[i]];
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(int));
		}
	}

	/* force output is in the unit of pN */
	f_out << "VECTORS frc float" 			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = frc[i].x *FRC_PN; SwapEnd(tmp_x);
		float tmp_y = frc[i].y *FRC_PN; SwapEnd(tmp_y);
		float tmp_z = frc[i].z *FRC_PN; SwapEnd(tmp_z);
		f_out.write((char*)&tmp_x, sizeof(float));
		f_out.write((char*)&tmp_y, sizeof(float));
		f_out.write((char*)&tmp_z, sizeof(float));
	}
}


void _beads::output
// --------------------------------------------------------------------
//
// Function: VTK output (default set with binary mode)
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	_beads::output_bin(ofs_name_in);
	_beads::output_adj(ofs_name_in);
}


void _beads::set_seed
// --------------------------------------------------------------------
//
// Function: 
// 
(
	int 	seed_i
)
// --------------------------------------------------------------------
{
	_beads::init_rand(seed_i);
}

int _beads::get_seed
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	return seed;
}

// void _beads::print_auto_list
// // --------------------------------------------------------------------
// //
// // Function: 
// // 
// (
// 	const char 	*ofs_name
// )
// // --------------------------------------------------------------------
// {
// 	FILE *f_out;
// 	f_out = fopen( ofs_name, "w" );
// 	if( f_out == NULL ){
// 		std::cout << "\x1b[31m" << 
// 		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
// 		exit(1);
// 	}

// 	for( int i=0; i<auto_pair.size(); i++ ){
// 		fprintf( f_out, "%d %d\n", auto_pair[i].first, auto_pair[i].second );
// 	}

// 	fclose(f_out);
// 	return;
// }


// void _beads::print_cross_list
// // --------------------------------------------------------------------
// //
// // Function: 
// // 
// (
// 	const char 	*ofs_name
// )
// // --------------------------------------------------------------------
// {
// 	FILE *f_out;
// 	f_out = fopen( ofs_name, "w" );
// 	if( f_out == NULL ){
// 		std::cout << "\x1b[31m" << 
// 		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
// 		exit(1);
// 	}

// 	for( int id=0; id<cross_pair.size(); id++ ){
// 		for( int i=0; i<cross_pair[id].size(); i++ ){
// 			fprintf( f_out, "%d %d %d\n", id, cross_pair[id][i].first, cross_pair[id][i].second );
// 		}
// 	}

// 	fclose(f_out);
// 	return;
// }

void _beads::get_vars_rp
// --------------------------------------------------------------------
//
// Function: getter for replsion variables
// 
(
	double 	*eps,
	double 	*sgm,
	double 	*cut
)
// --------------------------------------------------------------------
{
	*eps = eps_rp;
	*sgm = sgm_rp;
	*cut = cut_rp;
	return;
}

double _beads::get_cut_list
// --------------------------------------------------------------------
//
// Function: getter for cut_list
// 
(
)
// --------------------------------------------------------------------
{
	double val = std::max(cut_rp_list, cut_fg_list);
	return val;
}



void _beads::sync_grid
// --------------------------------------------------------------------
//
// Function: syncronize grid and make binned_beads array
// 
(
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	for( int i=0; i<grid.N_grid; i++ ){
		binned_beads.push_back(std::vector<int>());
	}
	return;
}


