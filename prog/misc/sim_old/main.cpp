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



#define OUT_STEP 500
// #define OUT_STEP 5000
// #define OUT_STEP 100
// #define LIST_STEP 100	 -> env.h
// #define TOTAL_STEP 5000000
#define TOTAL_STEP 500000
// #define TOTAL_STEP 1000

#define PATH_INPUT  "./input"
#define PATH_OUTPUT "./output"





// int main(){ 
/* Only FG-Nups */

// 	_bcs 	bcs;
// 	_grid 	grid(bcs);
// 	_fgnup 	fg;

// 	fg.init(grid);

// 	fg.set_beads( PATH_INPUT "/fg.vtk" );

// 	fg.output( PATH_OUTPUT "/fg_0" );

// 	fg.make_list(bcs,grid);

// 	for( int i=1; i<TOTAL_STEP; i++ ){

// 		if( i%LIST_STEP == 0 ){
// 			fg.make_list(bcs,grid);
// 		}

// 		fg.calc_force(bcs);
// 		fg.move(bcs);

// 		if( i%OUT_STEP == 0 ){
// 			std::cout << i << std::endl;
// 			char fg_name[68], cg_name[68], sl_name[68];
// 			sprintf( fg_name, "%s/fg_%d", PATH_OUTPUT, int(i/OUT_STEP) );
// 			fg.output( fg_name );
// 		}
// 	}

// 	return 0;
// }




int main(){
/* FG-Nups + Cargo */
	_bcs 	bcs;
	_grid 	grid(bcs);
	_fgnup 	fg;
	_cargo 	cg;

	cg.init(grid);
	fg.init(grid,cg);

	// fg.set_beads( PATH_INPUT "/s001_fgnup.vtk" );
	// cg.set_beads( PATH_INPUT "/s001_cargo.vtk", 0,0,-40 );

	fg.set_beads( PATH_INPUT "/fg.vtk" );
	cg.set_beads( PATH_INPUT "/cg.vtk" );

	fg.output( PATH_OUTPUT "/fg_0" );
	cg.output( PATH_OUTPUT "/cg_0" );

	cg.make_list(bcs,grid);
	fg.make_list(bcs,grid,cg);

	for( int i=1; i<TOTAL_STEP; i++ ){

		if( i%LIST_STEP == 0 ){
			cg.make_list(bcs,grid);
			fg.make_list(bcs,grid,cg);
		}

		cg.calc_force(bcs);
		fg.calc_force(bcs,cg);
		
		// cg.frc[0].y -= 1*KBT_0;

		cg.move(bcs);
		fg.move(bcs);

		if( i%OUT_STEP == 0 ){
			std::cout << i << std::endl;
			char fg_name[68], cg_name[68], sl_name[68];
			sprintf( fg_name, "%s/fg_%d", PATH_OUTPUT, int(i/OUT_STEP) );
			sprintf( cg_name, "%s/cg_%d", PATH_OUTPUT, int(i/OUT_STEP) );
			fg.output( fg_name );
			cg.output( cg_name );
		}
	}

	return 0;
}


