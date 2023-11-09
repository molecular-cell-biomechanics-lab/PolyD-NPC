#ifndef CARGO_H
#define CARGO_H

#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "env.h"
#include "bcs.h"
#include "vec.h"
#include "beads.h"


#define QT_INIT_0 1.0			// initial value of the quaternion
#define QT_INIT_1 0.0			// initial value of the quaternion
#define QT_INIT_2 0.0			// initial value of the quaternion
#define QT_INIT_3 0.0			// initial value of the quaternion

#define ADJUST_Z_THRE 10 		// cutoff from the domain top to exert adjustment force (nm)
#define POTE_ADJUST   1 		// height of the adjustment potential (kBT_0)

#define SHIFT_SEED 1000000


class _cargo : public _beads
{
public:
	_vec<double>  	center ; 		// center position of the cargo
	double 			quat[4];		// quaternion

	_vec<double> 	frc_sum;		// sumation of force
	_vec<double> 	trq_sum;		// sumation of torque

	double 			R_cargo;		// Radius of the cargo 				     (nm)
	double 			D_trans;		// Diffusion coefficient for translation (nm2/ns)
	double 			D_rotat;		// Diffusion coefficient for rotation    (nm2/ns)

	int 			seed;

public:
	 _cargo();
	 _cargo( int seed_i );
	~_cargo();
	void set_beads( const char *ifs_name );
	void set_beads( const char *ifs_name, double x, double y, double z );

	// without grid search
	// override to remove making auto pair list
	void make_list();
	void make_list( _bcs &bcs );
	// void make_list( _bcs &bcs, _beads &o_beads_0 );
	// void make_list( _bcs &bcs, _beads &o_beads_0, _beads &o_beads_1 );

	// with grid search
	void make_list( _bcs &bcs, _grid &grid );
	// void make_list( _bcs &bcs, _grid &grid, _beads &o_beads_0 );
	// void make_list( _bcs &bcs, _grid &grid, _beads &o_beads_0, _beads &o_beads_1 );

	void calc_force();
	void calc_force( _bcs &bcs );
	void calc_force( _bcs &bcs, _beads &o_beads_0 );
	// void calc_force( _bcs &bcs, _beads &o_beads_0, _beads &o_beads_1 );

	void move();
	void move( _bcs &bcs );

	void output( const char *ofs_name );

	void set_center( double x, double y, double z );
	
	void calc_cargo_force();
	void calc_cargo_force(_bcs &bcs);

	int  count_state();

	int  get_seed();
	void set_seed(int seed_i);

// private:
	std::vector<_vec<double> > ori;					// position vec of bead from center
	std::vector<_vec<double> > ori_0;				// position vec of bead from center at t=0

	_vec<double> 	trq_sum_body;					// sumation of torque 

	_vec<double> 	d_theta; 						// for calculation
	double 			d_qt[4];						// for calculation
	double 			t_qt[4];						// for calculation

	double 	mat_A[3][3];							// A matrix (left:row, right:columnn)
	double 	mat_B[4][3];							// B matrix (left:row, right:columnn)

	std::vector<std::tuple<int,int,int> > face;		// triagonal set element (only for output)


	void init_rand();
	void init_rand( int seed_i );

	void set_beads_asc( const char *ifs_name );
	void set_beads_bin( const char *ifs_name );

	// void make_list_cross_fg( _bcs &bcs, _beads &o_beads, int cross_id );
	// void make_list_cross_fg( _bcs &bcs, _grid &grid, _beads &o_beads, int cross_id );

	void read_vars();
	void calc_vars();
	void calc_static();
	void set_init();
	void display_vars();

	void sum_force();
	void sum_torque();
	void adjust_force(_bcs &bcs);

	void update_center();
	void update_quat();
	void update_orient();
	void update_pos();

	void calc_torque_body();
	void calc_theta();
	void calc_d_quat();
	void calc_t_quat();
	void norm_quat();

	void calc_A();
	void calc_B();

	void output_asc(const char *ofs_name);
	void output_bin(const char *ofs_name);
	void output_ctr(const char *ofs_name);

	// bool find_elem_cargo( int cross_id, int elm );

};


#endif