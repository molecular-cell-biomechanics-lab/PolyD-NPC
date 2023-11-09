#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "vec.h"
#include "env.h"


/* ____ unit ______

length		: nm
time		: ns
mass		: g
temperature	: K
___________________ */


class _bcs;
class _beads;


class _grid
{
public:
	double 	grid_dim; 			// dimension (size) of a gird	(nm)
	int 	num_x	;			// # of grids in x-direction	(-)
	int 	num_y	;			// # of grids in y-direction	(-)
	int 	num_z	;			// # of grids in z-direction	(-)
	int 	N_grid 	; 			// # of total grids 			(-)

	// std::vector<std::vector<int> > binned_beads; 	// list of beads in a grid
	std::vector<std::vector<int> > grid_nbr; 		// list of neighboring grids (including itself)

public:
	 _grid( _bcs &bcs );
	~_grid();
	// void init( _bcs *bcs );
	void sort_beads( _beads *beads );

private:
	void read_vars();
	void calc_vars( _bcs &bcs );
	void display_vars();
	void make_grid();
	void list_grid_nbr();
	int  get_bin( _vec<double> pos );

};


#endif