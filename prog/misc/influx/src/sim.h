#ifndef DEF_H
#define DEF_H

#include "beads.h"
#include "fgnup.h"
#include "cargo.h"
#include "bcs.h"
#include "vec.h"
#include "grid.h"

#include "sim.h"
#include "param.h"

/* ____ unit ______

length		: nm
time		: ns
weight		: kg
temperature	: K
___________________ */


#define LIST_STEP 	100

#ifdef DEBUG
	#define REC_STEP 	500
	#define TIME_MAX 	0.001 	// 0.001 hour = 3.6 second
#else
	#define REC_STEP 	5000
	#define TIME_MAX 	30 		// 30 hour
#endif


struct _pos
{
// // 	memo:: frc is used only for VTK output, so float is good enough

// 	std::vector<_vec<double> > fg_pos; 		// position of fg-nups
// 	std::vector<_vec<double> > cg_pos;		// position of cargo beads

// 	std::vector<_vec<float > > fg_frc; 		// force of fg-nups
// 	std::vector<_vec<float > > cg_frc;		// force of cargo beads

// 	_vec<double> 			   cg_cen; 		// center of the cargo
// 	double 					   cg_qat[4];	// quaternion of the cargo
// 	_vec<double> 		   cg_frc_sum;		// sumation of the force on cargo
// 	_vec<double> 		   cg_trq_sum;		// sumation of the torque on cargo

	_cargo cargo;
	_fgnup fgnup;

};

struct _traj
{
	_pos  end_pos;		// termination position of the trajectory
	int 		n; 		// # of total steps in the trajectory
	double 		s; 		// unsuccessful (0) or successful (1) to move 
						// forward to the next interface
	int 		k; 		// k_launch id
};

struct _path
{
	std::vector<_pos>  start; 	// start position at interface i to i+1 (i=0..n-1)
	std::vector<_traj>  traj; 	// trajectory     at interface i to i+1 (i=0..n-1)
	std::vector<double> prob; 	// probability to go thru int. i to i+1 (i=0..n-1)
	std::vector<double> w_ib; 	// weight of path b from A to intfc i+1 (i=0..n-1)
};


void ffs_sim
// Purpose	: main FFS simulation from 0-th to n-th interface
// File		: sim.cpp
(
	_param 	&param,
	_path 	&path ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
);


void write_out
(
	int 	seed,
	_param 	&param,
	_path	&path,
	_cargo 	&cargo,
	_fgnup 	&fgnup
);

#endif