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
#include "env.h"
#include "vec.h"
#include "grid.h"

#define OUT_STEP 5000
#define TOTAL_STEP 50000
#define PATH_INPUT "./input"


int main(){
	_bcs 	bcs;
	_grid 	grid(bcs);
	_fgnup 	fg;
	_cargo 	cg;

	cg.init(grid);
	fg.init(grid,cg);

	fg.set_beads( PATH_INPUT "/fgnup.vtk" );
	cg.set_beads( PATH_INPUT "/cargo.vtk", 0,0,80 );

	fg.output( "test/fg_0" );
	cg.output( "test/cg_0" );

	cg.make_list(bcs,grid);
	fg.make_list(bcs,grid,cg);

	for( int i=1; i<TOTAL_STEP; i++ ){

		if( i%LIST_STEP == 0 ){
			cg.make_list(bcs,grid);
			fg.make_list(bcs,grid,cg);
		}

		cg.calc_force(bcs);
		fg.calc_force(bcs,cg);
		
		cg.frc[0].y -= 1*KBT_0;

		cg.move(bcs);
		fg.move(bcs);

		if( i%OUT_STEP == 0 ){
			std::cout << i << std::endl;
			char fg_name[68], cg_name[68], sl_name[68];
			sprintf( fg_name, "test/fg_%d", int(i/OUT_STEP) );
			sprintf( cg_name, "test/cg_%d", int(i/OUT_STEP) );
			fg.output( fg_name );
			cg.output( cg_name );
		}
	}

	return 0;
}
