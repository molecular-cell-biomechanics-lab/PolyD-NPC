#ifndef SYS_H
#define SYS_H

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


namespace sys_2{
	void init	   ( _cargo &cargo, _fgnup &fgnup, _grid &grid, char *dir_i );
	void make_list ( _cargo &cargo, _fgnup &fgnup, _grid &grid, _bcs &bcs );
	void calc_force( _cargo &cargo, _fgnup &fgnup, _grid &grid, _bcs &bcs );
	void move      ( _cargo &cargo, _fgnup &fgnup, _grid &grid, _bcs &bcs );
	void record_adj( _cargo &cargo, _fgnup &fgnup, _grid &grid, _bcs &bcs );
}

// namespace sys_3{
// 	void init	   ( _fgnup &fgnup, _cargo &cargo, _beads &soltn, _grid &grid, char *dir_i );
// 	void make_list ( _fgnup &fgnup, _cargo &cargo, _beads &soltn, _grid &grid, _bcs &bcs );
// 	void calc_force( _fgnup &fgnup, _cargo &cargo, _beads &soltn, _grid &grid, _bcs &bcs );
// 	void move      ( _fgnup &fgnup, _cargo &cargo, _beads &soltn, _grid &grid, _bcs &bcs );
// }


#endif