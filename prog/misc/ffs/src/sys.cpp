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
#include "sys.h"


void sys_2::init
// --------------------------------------------------------------------
//
// Function: read configuration and initialize system
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	char 	*dir_i 			// input directory
)
// --------------------------------------------------------------------
{
	cargo.init(grid);
	fgnup.init(grid,cargo);

	char ifs[128];
	sprintf(ifs, "%s%s", dir_i, "cargo.vtk"); cargo.set_beads(ifs);
	sprintf(ifs, "%s%s", dir_i, "fgnup.vtk"); fgnup.set_beads(ifs);

	return;
}

void sys_2::make_list
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	cargo.make_list( bcs, grid );
	fgnup.make_list( bcs, grid, cargo );
	return;
}


void sys_2::calc_force
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	cargo.calc_force( bcs );
	fgnup.calc_force( bcs, cargo );
	return;
}


void sys_2::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	cargo.move( bcs );
	fgnup.move( bcs );

	return;
}

