#ifndef FGNUP_H
#define FGNUP_H

#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "env.h"
#include "bcs.h"
#include "vec.h"
#include "beads.h"


#define NUP_SIZE 80
#define SHIFT_SEED_F 2000000

class _fgnup : public _beads
{
private:
	std::vector<std::pair <int,int>     > bond ;
	std::vector<std::tuple<int,int,int> > angle;
	std::vector<int> teth;  // 0: non-tethered, 1: tethered

public:
	/* tension		*/
	double 	k_tens	;		// spring constant for tensil force 				{kBT_0/segm^2}
	double 	r_tens	;		// natural length between neighboring beads			(nm)

	/* bending 		*/
	double 	l_kuhn 	;		// Kuhn length of FG-Nups 							{segm}
	double 	k_bend	;		// spring constant for bending 						(g nm2 /ns2)

	int 		seed;

	 _fgnup();
	 // _fgnup( int seed ):_beads( seed );
	 _fgnup( int seed_i );
	~_fgnup();
	void set_beads( const char *ifs_name );

	void init_rand();
	void init_rand( int seed_i );	

	void calc_force();
	void calc_force( _bcs &bcs );
	void calc_force( _bcs &bcs, _beads &beads_0 );
	// void calc_force( _bcs &bcs, _beads &beads_0, _beads &beads_1 );

	void move();
	void move( _bcs &bcs );

	void output    ( const char *ofs_name );
	void output_asc( const char *ofs_name );
	void output_bin( const char *ofs_name );

private:
	void read_vars();
	void calc_vars();
	void display_vars();
	void calc_force_bond ();
	void calc_force_bond ( _bcs &bcs );
	void calc_force_angle();
	void calc_force_angle( _bcs &bcs );
	void set_beads_asc( const char *ifs_name );
	void set_beads_bin( const char *ifs_name );

};


#endif