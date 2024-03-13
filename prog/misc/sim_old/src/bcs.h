#ifndef BCS_H
#define BCS_H

#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "vec.h"
#include "env.h"
#include "grid.h"
#include "beads.h"
#include "cargo.h"


/* ____ unit ______

length		: nm
time		: ns
mass		: g
temperature	: K
___________________ */


class _beads;


class _bcs
{
public:
	double 	R_pore 	; 		// pore radius  					(nm)
	double 	cy_top	; 		// top    boundary of the cylinder  (nm)
	double 	cy_btm	; 		// bottom boundary of the cylinder  (nm)
	double 	L_x 	; 		// box size						    (nm)
	double 	L_y 	; 		// box size						    (nm)
	double 	L_z 	; 		// box size						    (nm)

public:
	 _bcs();
	~_bcs();
	// void init( _beads *beads );
	void calc_force_wall( _beads *beads );
	void adjust_periodic( _vec<double> *dr );
	void move_at_boundary( _beads *beads );
	void move_at_boundary( _cargo *cargo );

private:
	void read_vars();
	void display_vars();
	void calc_static();
	void bounce_on_wall( _beads *beads );

	double 	length_x;		// x-length of the simulation box
	double 	length_y;		// y-length of the simulation box
	double 	length_z;		// z-length of the simulation box
	double 	thre_dist_x; 	// threshold distance over which periodic adjustment needed
	double 	thre_dist_y; 	// threshold distance over which periodic adjustment needed
	double 	thre_dist_z; 	// threshold distance over which periodic adjustment needed

};


inline void _bcs::adjust_periodic
// --------------------------------------------------------------------
//
// Function:
// 
(
	_vec<double> 	*dr
)
// --------------------------------------------------------------------
{
	double dx = dr->x;
	double dy = dr->y;
	double dz = dr->z;
	// x
	if( dx >= thre_dist_x ){
		dx -= length_x;
	}else if( dx <= thre_dist_x ){
		dx += length_x;
	}
	// y
	if( dy >= thre_dist_y ){
		dy -= length_y;
	}else if( dy <= thre_dist_y ){
		dy += length_y;
	}
	// z
	if( dz >= thre_dist_z ){
		dz -= length_z;
	}else if( dz <= thre_dist_z ){
		dz += length_z;
	}
	return;
}



#endif