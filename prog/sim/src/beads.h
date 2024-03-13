#ifndef BEADS_H
#define BEADS_H

#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "vec.h"
#include "env.h"

/* ____ unit ______

length		: nm
time		: ns
mass		: g
temperature	: K
___________________ */


class _grid;


// Macro definition ===========================================================================

#define MARGIN				2.0		// list margin (nm)
#define EQUIV_STEP			100
#define PAIR_RESERVE 		200		// size of pair_list capacity
#define ADJ_AUTO_RESERVE 	345696	// size of hyd_adj capacity
#define ADJ_CROS_RESERVE 	17472	// size of hyd_adj_cross capacity


class _bcs;
class _grid;
class _cargo;
class _fgnup;


class _beads
{
public:
	std::vector<_vec<double> > pos;			// position of the beads
	std::vector<_vec<double> > frc;			// force on the beads
	std::vector<double>		   hyd;			// hydrophobic factor
	std::vector<int>		   hyd_id;		// index of hydrophobic beads 	   (hyd id -> bead id)
	std::vector<int>		   hyd_inv_id;	// inverse id of hydrophobic beads (bead id -> hyd id)

	std::vector<int>		   			hyd_state;		// number of interacting beads
	std::vector<bool> 		   			hyd_adj; 		// adjacent matrix for fg-fg (0: free, 1: interacting)
	std::vector< std::vector<bool> > 	hyd_adj_cross; 	// adjacent matrix for fg-fg (0: free, 1: interacting)
	
	std::vector< std::vector<int> > 				auto_pair_rp;	// pair list
	std::vector< std::vector<int> > 				auto_pair_fg;	// pair list (hyd id -> bead id)
	std::vector< std::vector< std::vector<int> > >  cross_pair_rp; 	// pair list
	std::vector< std::vector< std::vector<int> > >  cross_pair_fg; 	// pair list

	std::vector<std::vector<int> > binned_beads; 	// list of beads in a grid

// protected:
	/* general */
	double 	l_segm 	;		// segment length of bead-spring					(nm)
	double 	t_delt 	; 		// simulation time step 							(ns)

	/* diffusion 	*/
	double 	T_body	; 		// temperature 										(K)
	double  h_visc 	; 		// viscosity of water in room temperature 			{cP}={mPa s}

	/* diffusion 	*/
	double  r_stks 	; 		// Stokes radius of each bead 			  			(nm)
	double  D_diff 	; 		// Diffusion coefficient 	 			  			(nm2 /ns)
	
	/* repulsion 	*/
	double 	eps_rp	;		// height of repulsion potential 					{kBT_0}
	double 	sgm_rp	;		// characteristic length of repulsion potential 	{segm}
	double 	cut_rp	;		// cutoff distance of repulsion potential 			(nm)
	double 	sft_rp	;		// shift parameter making the soft-core 			(-)

	/* hydrophobic  */
	double 	eps_fg	;		// interfacial energy of hydrophobic attraction 	{kBT_0}
	double 	sgm_fg	;		// characteristic length of hydrophobic attraction 	{segm}
	double 	cut_fg	;		// cutoff distance of hydrophobic attraction 		{segm}
	double 	sft_fg	;		// shift parameter making the soft-core 			(-)

	double 	cut_rp_2;		// squre of the cutoff distnace (replsion)
	double 	cut_fg_2;		// squre of the cutoff distnace (hydrophobic)
	double 	cut_rp_list;	// cutoff distancce to make list for repulsion frc
	double 	cut_fg_list;	// cutoff distancce to make list for hydrophobic frc
	double 	cut_rp_list_2;	// squre of the cutoff distnace for listing (replsion)
	double 	cut_fg_list_2;	// squre of the cutoff distnace for listing (hydrophobic)

	int 	seed;

	double 	coeff_ex; 		// coefficient of the external force
	double 	rand_std; 		// deviation of the random force (normalized for uniform dist)	

public:
	_beads();
	_beads( int seed_i );
	~_beads();

	void display_vars();

	void set_beads( const char *ifs_name );
	void set_beads( double x, double y, double z, double i_hyd );
	// void set_beads( int N_bead, _bcs *bcs, _vec<double> cg_cen, double cg_rad );
	void set_beads( int N_bead, _bcs &bcs, _grid &grid, _cargo &cargo, _fgnup &fgnup );

	// without grid search
	void init();
	void init( _beads &o_beads_0 );
	void init( _beads &o_beads_0, _beads &o_beads_1 );

	// with grid search
	void init( _grid &grid );
	void init( _grid &grid, _beads &o_beads_0 );
	void init( _grid &grid, _beads &o_beads_0, _beads &o_beads_1 );

	// without grid search
	void make_list();
	void make_list( _bcs &bcs );
	void make_list( _bcs &bcs, _beads &o_beads_0 );
	void make_list( _bcs &bcs, _beads &o_beads_0, _beads &o_beads_1 );

	// with grid search
	void make_list( _bcs &bcs, _grid &grid );
	void make_list( _bcs &bcs, _grid &grid, _beads &o_beads_0 );
	void make_list( _bcs &bcs, _grid &grid, _beads &o_beads_0, _beads &o_beads_1 );

	void init_force();

	void calc_force();
	void calc_force( _bcs &bcs );
	void calc_force( _bcs &bcs, _beads &o_beads_0 );
	// void calc_force( _bcs &bcs, _beads &o_beads_0, _beads &o_beads_1 );

	void move();
	void move( _bcs &bcs );

	void make_hyd_adj();
	void make_hyd_adj( _bcs	&bcs );
	void make_hyd_adj( _bcs	&bcs, _beads &o_beads_0 );

	void calc_hyd_state();
	void calc_hyd_state( _beads &o_beads_0 );	

	void output			 ( const char *ofs_name );
	void output_asc		 ( const char *ofs_name );
	void output_bin		 ( const char *ofs_name );
	void output_adj 	 ( const char *ofs_name );
	void print_auto_list ( const char *ofs_name );
	void print_cross_list( const char *ofs_name );

	void get_vars_rp( double *eps, double *sgm, double *cut );
	double get_cut_list(); 

	int  get_seed();
	void set_seed(int seed_i);

protected:
	void init_cross_list( int n_obj ); 
	void init_pair();

	// void make_list_auto ();
	void make_list_auto_rp();
	void make_list_auto_fg();
	// void make_list_auto ( _bcs &bcs );
	void make_list_auto_rp ( _bcs &bcs );
	void make_list_auto_fg ( _bcs &bcs );
	// void make_list_cross( _bcs &bcs, _beads &o_beads, int cross_id );
	void make_list_cross_rp( _bcs &bcs, _beads &o_beads, int cross_id );
	void make_list_cross_fg( _bcs &bcs, _beads &o_beads, int cross_id );

	// void make_list_auto ( _bcs &bcs, _grid &grid );
	void make_list_auto_rp ( _bcs &bcs, _grid &grid );
	void make_list_auto_fg ( _bcs &bcs, _grid &grid );
	// void make_list_cross( _bcs &bcs, _grid &grid, _beads &o_beads, int cross_id );
	void make_list_cross_rp( _bcs &bcs, _grid &grid, _beads &o_beads, int cross_id );
	void make_list_cross_fg( _bcs &bcs, _grid &grid, _beads &o_beads, int cross_id );


	void calc_force_rp  ();
	void calc_force_fg  ();
	void calc_force_rp  ( _bcs &bcs );
	void calc_force_fg  ( _bcs &bcs );
	void calc_force_rpfg( _bcs &bcs );
	void calc_force_rp  ( _bcs &bcs, _beads &o_beads, int cross_id );
	void calc_force_fg  ( _bcs &bcs, _beads &o_beads, int cross_id );
	void calc_force_rpfg( _bcs &bcs, _beads &o_beads, int cross_id );

	_vec<double> rp_force( double dist_2, _vec<double> dr );
	_vec<double> fg_force( double dist_2, _vec<double> dr, double prod_hyd );

	void set_hyd_adj();
	void set_hyd_adj( int o_beads_size, int cross_id );

	void init_hyd_adj_auto();
	void init_hyd_adj_cross( int o_beads_size, int cross_id );

	void make_hyd_adj_auto ();
	void make_hyd_adj_auto ( _bcs &bcs );
	void make_hyd_adj_cross( _bcs &bcs, _beads &o_beads, int cross_id );

	void init_hyd_state();
	void calc_hyd_state_auto();
	void calc_hyd_state_cross( _beads &o_beads, int cross_id );


	// double 	k_roll();
	// bool 	find_elem( std::vector<int> arr, int elm );


private:
	void init_rand();
	void init_rand( int seed_i );
	void read_vars();
	void calc_vars();
	void calc_static();
	void sync_grid( _grid &grid );

	void set_beads_asc( const char *ifs_name );
	void set_beads_bin( const char *ifs_name );

	_vec<double> test_dr( int i, int j );

};



#endif