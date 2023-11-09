#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "sim.h"
#include <iomanip>
#include <time.h>

#define PATH_INP "./input"
#define PATH_OUT "./output"
#define PATH_REC "./rec"


std::mt19937 							fft_mt;			// random number generator
std::uniform_real_distribution<double> 	rnd(-1.0, 1.0);	// uniform distribution


int heaviside
// --------------------------------------------------------------------
//
// Purpose: Heaviside step function
// 			H(x-a) = 0 	(if x < a)
// 			H(x-a) = 1 	(if a < x)
// 
(
	double x, 
	double a 
)
// --------------------------------------------------------------------
{
	int val = 0;
	if( a < x ) val = 1;
	return val;
}


int heaviside_zero
// --------------------------------------------------------------------
//
// Purpose: Heaviside step function (very first interface)
// 			H(x-a) = 0 	(if |r-r0| > a) outside
// 			H(x-a) = 1 	(if |r-r0| < a) inside
// 
(
	_vec<double> pos,
	double 		 R_pore 	// radius
)
// --------------------------------------------------------------------
{
	int val = 0;
	double dist_2 = 0.0;
	dist_2 += (pos.x-INTF_CT_X) * (pos.x-INTF_CT_X);
	dist_2 += (pos.y-INTF_CT_Y) * (pos.y-INTF_CT_Y);
	dist_2 += (pos.z-INTF_CT_Z) * (pos.z-INTF_CT_Z);
	if( dist_2 < R_pore*R_pore || pos.z > INTF_CT_Z )	val = 1;
	return val;
}

void write_cpos_asc
// --------------------------------------------------------------------
//
// Function: output cpos (to temporaly save in SSD)
// 
(
	int 	seed,
	int 	inter,	// (i+1) th interface (shifted by 1 to record starting pos)
	int 	k_bra,
	int 	count,
	_vec<double> *cpos,
	int 		 *bond
)
// --------------------------------------------------------------------
{
	FILE *f_out;
	char fname[256];
	sprintf(fname, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt", PATH_OUT, seed, inter, k_bra, count);
	f_out = fopen( fname, "w" );

	fprintf(f_out, "%d\n", REC_STEP );

	for( int i=0; i<REC_STEP; i++ ){
		fprintf(f_out, "%lf %lf %lf %d\n", cpos[i].x, cpos[i].y, cpos[i].z, bond[i] );
	}
	fclose(f_out);
	return;
}

void write_cpos_bin
// --------------------------------------------------------------------
//
// Function: output cpos (to temporaly save in SSD)
// 
(
	int 	seed,
	int 	inter,	// (i+1) th interface (shifted by 1 to record starting pos)
	int 	k_bra,
	int 	count,
	_vec<double> *cpos,
	int 		 *bond
)
// --------------------------------------------------------------------
{
	char ofs_name[256];
	sprintf(ofs_name, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt", PATH_OUT, seed, inter, k_bra, count);
	std::ofstream f_out;
	f_out.open(ofs_name, std::ios::binary);
	
	if(!f_out){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	f_out << REC_STEP <<std::endl;

	for( int i=0; i<REC_STEP; i++ ){
		f_out.write((char*)&cpos[i].x, sizeof(double));
		f_out.write((char*)&cpos[i].y, sizeof(double));
		f_out.write((char*)&cpos[i].z, sizeof(double));
		f_out.write((char*)&bond[i]  , sizeof(int 	));
		// f_out << cpos[i].x << " " << cpos[i].y << " " << cpos[i].z <<std::endl;
	}

	return;
}


void output_config
// --------------------------------------------------------------------
//
// Function: output beads configuration (to temporaly save in SSD)
// 
(
	int 	seed,
	int 	inter,	// (i+1) th interface (shifted by 1 to record starting pos)
	int 	k_bra,
	int 	count,
	_cargo 	&cargo,
	_fgnup 	&fgnup
)
// --------------------------------------------------------------------
{
	char fn_cg[256];
	char fn_fg[256];

	sprintf(fn_cg, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d", PATH_OUT, seed, inter, k_bra, count);
	sprintf(fn_fg, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d", PATH_OUT, seed, inter, k_bra, count);
	cargo.output(fn_cg);
	fgnup.output(fn_fg);

	return;
}


void convert_sys_to_pos
// --------------------------------------------------------------------
//
// Function: _beads -> _pos conversion
// 
(
	_pos	&pos  ,		// empty
	_cargo 	&cargo,
	_fgnup 	&fgnup
)
// --------------------------------------------------------------------
{
	// int N_fgnup = fgnup.pos.size();
	// int N_cargo = cargo.pos.size();

	// pos.fg_pos.reserve(N_fgnup);
	// pos.fg_frc.reserve(N_fgnup);
	// pos.cg_pos.reserve(N_cargo);
	// pos.cg_frc.reserve(N_cargo);

	// for(int i=0; i<N_fgnup; i++){
	// 	pos.fg_pos.push_back(fgnup.pos[i]);
	// 	// pos.fg_frc.push_back(fgnup.frc[i]);
	// 	float tmp_x = fgnup.frc[i].x; // double -> float conversion
	// 	float tmp_y = fgnup.frc[i].y; // double -> float conversion
	// 	float tmp_z = fgnup.frc[i].z; // double -> float conversion
	// 	pos.fg_frc.push_back( _vec<float>(tmp_x, tmp_y, tmp_z) );
	// }
	// for(int i=0; i<N_cargo; i++){
	// 	pos.cg_pos.push_back(cargo.pos[i]);
	// 	// pos.cg_frc.push_back(cargo.frc[i]);
	// 	float tmp_x = cargo.frc[i].x; // double -> float conversion
	// 	float tmp_y = cargo.frc[i].y; // double -> float conversion
	// 	float tmp_z = cargo.frc[i].z; // double -> float conversion
	// 	pos.cg_frc.push_back( _vec<float>(tmp_x, tmp_y, tmp_z) );
	// }

	// pos.cg_cen = cargo.center;
	// for(int i=0; i<4; i++) pos.cg_qat[i] = cargo.quat[i];

	// pos.cg_frc_sum = cargo.frc_sum;
	// pos.cg_trq_sum = cargo.trq_sum;

	pos.cargo = cargo;
	pos.fgnup = fgnup;

	return;
}


void convert_pos_to_sys
// --------------------------------------------------------------------
//
// Function: _pos -> _beads conversion
//			 frc of fgnup and cargo here is used only for VTK output
// 			 so float -> double converstion does not affect the calculation
// 
(
	_pos	&pos  ,		// not empty
	_cargo 	&cargo,
	_fgnup 	&fgnup
)
// --------------------------------------------------------------------
{
	// int N_fgnup = fgnup.pos.size();
	// int N_cargo = cargo.pos.size();

	// for(int i=0; i<N_fgnup; i++){
	// 	fgnup.pos[i] = pos.fg_pos[i];
	// 	// fgnup.frc[i] = pos.fg_frc[i];
	// 	double tmp_x = pos.fg_frc[i].x; // float -> double conversion
	// 	double tmp_y = pos.fg_frc[i].y; // float -> double conversion
	// 	double tmp_z = pos.fg_frc[i].z; // float -> double conversion
	// 	fgnup.frc[i] = _vec<double>(tmp_x, tmp_y, tmp_z);
	// }

	// for(int i=0; i<N_cargo; i++){
	// 	cargo.pos[i] = pos.cg_pos[i];
	// 	// cargo.frc[i] = pos.cg_frc[i];
	// 	double tmp_x = pos.cg_frc[i].x; // float -> double conversion
	// 	double tmp_y = pos.cg_frc[i].y; // float -> double conversion
	// 	double tmp_z = pos.cg_frc[i].z; // float -> double conversion
	// 	cargo.frc[i] = _vec<double>(tmp_x, tmp_y, tmp_z);
	// }

	// cargo.center = pos.cg_cen;
	// for(int i=0; i<4; i++) cargo.quat[i] = pos.cg_qat[i];

	// cargo.frc_sum = pos.cg_frc_sum;
	// cargo.trq_sum = pos.cg_trq_sum;

	cargo = pos.cargo;
	fgnup = pos.fgnup;

	return;
}



void dummy_force
// --------------------------------------------------------------------
//
// Function: dummy calculation of the force for force record in _pos
// 			 (we want to record force after moving beads.
// 			  the force calculated in the main loop is before moving beads.
//			  just the calculation of the force does not harm the system.)
// 
(
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  
)
// --------------------------------------------------------------------
{
	sys_2::calc_force( cargo, fgnup, grid, bcs ); 		
	cargo.calc_cargo_force( bcs ); // usually in move() function

	return;
}


int pruning
// --------------------------------------------------------------------
//
// Function: pruning (0:continue, 1:terminate)
// 
(
	_param 	&param,
	_cargo 	&cargo,
	_traj 	&temp_traj_k,
	int 	&prev_pos,
	int 	&prun_id
)
{
	if( heaviside( cargo.center.z, param.x_div[prev_pos] ) == 0 ){

		double p_prob = param.p_end[prun_id];  // pruning probability
		prev_pos -= 1;
		prun_id  += 1;
		double 	rand = (rnd(fft_mt) + 1.0)/ 2; 	// rand = 0..1

		if( rand<p_prob ){
			temp_traj_k.s = 0.0;
			return 1; // terminate
		}else{
			temp_traj_k.s *= 1.0/(1.0-p_prob);
			return 0; // continue
		}

	}
	return 0;
}

void log_output
// --------------------------------------------------------------------
//
// Function: terminal output of the simulation log
// 
(
	int 	interface,
	int 	kth_launch,
	int 	traj_num,
	int 	step_count,
	int 	prun_id,
	int 	prev_pos_id,
	// double 	cargo_cen_x,
	// double 	cargo_cen_y,
	// double 	cargo_cen_z,
	_cargo 	cargo,
	double 	traj_weight
)
// --------------------------------------------------------------------
{
	double x = cargo.center.x;
	double y = cargo.center.y;
	double z = cargo.center.z;

	std::cout << std::fixed;
	std::cout 
	<< std::setw(2) << interface 	<< " " 
	<< std::setw(2) << kth_launch 	<< " " 
	<< std::setw(3) << traj_num 	<< " " 
	<< std::setw(7) << step_count 	<< " " 
	<< std::setw(2) << prun_id 		<< " " 
	<< std::setw(2) << prev_pos_id 	<< " " 
	// << std::setw(8) << std::setprecision(4) << cargo_cen_x << " " 
	// << std::setw(8) << std::setprecision(4) << cargo_cen_y << " " 
	// << std::setw(8) << std::setprecision(4) << cargo_cen_z << " " 
	<< std::setw(8) << std::setprecision(4) << cargo.center.x << " " 
	<< std::setw(8) << std::setprecision(4) << cargo.center.y << " " 
	<< std::setw(8) << std::setprecision(4) << cargo.center.z << " " 
	<< std::setw(8) << std::setprecision(4) << sqrt(x*x + y*y + z*z) << " " 
	<< std::setw(5) << std::setprecision(2) << traj_weight    << " "
	<< std::setw(3) << cargo.count_state()  << 
	std::endl;

	return;
}


void launch_ksim
// --------------------------------------------------------------------
//
// Function: with pruning
// 
(
	int 	i, 						// i-th interface (i=0..n-1)
	_param 	&param,
	_path 	&path , 				// path
	std::vector<_traj> &temp_traj,	// trajectory k=0..k_bra[i]
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
)
// --------------------------------------------------------------------
{
	_pos start_i = path.start[i];
	// std::vector<_vec<double> > cpos(REC_STEP);
	_vec<double> cpos[REC_STEP];
	int 		 bond[REC_STEP];

	for( int k = 0; k<param.k_bra[i]; k++ ){

		int count    = 0;
		int traj_num = 0;
		int prev_pos = i;
		int prun_id  = 0;


		// cargo.set_seed(seed+k);
		// fgnup.set_seed(seed+k);
		// int tmp_1 = cargo.get_seed();
		// int tmp_2 = fgnup.get_seed();
		// std::cout << tmp_1 << " " << tmp_2 << std::endl;

		std::cout << "k= " << k << std::endl;
		convert_pos_to_sys( start_i, cargo, fgnup );

		_traj temp_traj_k;
		temp_traj_k.s = 1.0;
		temp_traj_k.k = k;

		log_output(i,k,traj_num,count,prun_id,prev_pos,cargo,temp_traj_k.s);

		sys_2::make_list( cargo, fgnup, grid, bcs );

		while(1){	// main simulation loop

			sys_2::calc_force( cargo, fgnup, grid, bcs );
			sys_2::move	     ( cargo, fgnup, grid, bcs );
			cpos[count%REC_STEP] = cargo.center;
			bond[count%REC_STEP] = cargo.count_state();
			count ++;

			if( count%LIST_STEP == 0 ){
				sys_2::make_list( cargo, fgnup, grid, bcs );
			}
			if( count%REC_STEP  == 0 ){

				dummy_force( cargo, fgnup, grid, bcs );
				// _pos tmp_pos;
				// convert_sys_to_pos( tmp_pos, fgnup, cargo );
				// temp_traj_k.pos.push_back( tmp_pos );

				output_config (seed,i+1,k,traj_num,cargo,fgnup);
				write_cpos_bin(seed,i+1,k,traj_num,cpos,bond);
				// write_cpos_asc(seed,i+1,k,traj_num,cpos,bond);

				traj_num ++;

				log_output(i,k,traj_num,count,prun_id,prev_pos,cargo,temp_traj_k.s);

				if( heaviside( cargo.center.z, param.x_div[i+1] ) == 1 ){
					temp_traj_k.n = traj_num;
					_pos end_pos;
					convert_sys_to_pos( end_pos, cargo, fgnup );
					temp_traj_k.end_pos = end_pos;
					temp_traj.push_back(temp_traj_k);
					break;
				// }else if( heaviside( cargo.center.z, param.x_div[0] ) == 0 ){
				}else if( heaviside_zero( cargo.center, bcs.R_pore ) == 0 ){
					temp_traj_k.n = traj_num;
					temp_traj_k.s = 0.0;
					temp_traj.push_back(temp_traj_k);
					break;
				}

				if( pruning(param, cargo, temp_traj_k, prev_pos, prun_id) == 1 ){
					temp_traj_k.n = traj_num;
					temp_traj.push_back(temp_traj_k);
					break;
				}

			}
		}
	}

	return;
}


void calc_prob_f
// --------------------------------------------------------------------
//
// Purpose: 
// 
(
	int 	i, 						// i-th interface (i=0..n-1)
	_param 	&param,
	_path 	&path,
	std::vector<_traj> &temp_traj	// trajectory k=0..k_bra[i]
)
// --------------------------------------------------------------------
{
	double	prob_i = 0.0;
	for( int k=0; k<param.k_bra[i]; k++ ){
		prob_i += temp_traj[k].s;
	}
	prob_i /= param.k_bra[i];
	path.prob.push_back(prob_i);
	return;
}


void calc_weight
// --------------------------------------------------------------------
//
// Function: calculation of the weight
// 			 w_ib[i] is the weight for w_i+1,b (weight for next interface)
//			 misalignment is modified in the output file (write_rate)
// 
(
	int 	i, 						// i-th interface (i=0..n-1)
	_path 	&path 
)
// --------------------------------------------------------------------
{
	double	weight = 1.0;
	for( int j=0; j<i+1; j++ ){
		weight *= path.prob[j];
	}
	// i-th weight w_i,b should be the prod of j=0..i-1
	// for the convenience, j=0..i is calualted here (i.e. w_i+1,b)
	// the misalignment is modified in the output file (write_rate)

	path.w_ib.push_back(weight);
	return;
}


void choose_next
// --------------------------------------------------------------------
//
// Function: choose next starting posistion depending on the pruning weight
// 
(
	int 	i, 						// i-th interface (i=0..n-1)
	_param 	&param,
	_path 	&path, 
	std::vector<_traj> &temp_traj	// trajectory k=0..k_bra[i]
)
// --------------------------------------------------------------------
{
	std::vector<int> 	k_success;
	std::vector<double> k_weight;
	double 				sum_wt = 0.0;

	for( int k=0; k<param.k_bra[i]; k++ ){
		double wt = temp_traj[k].s;
		if( wt == 0 ) continue;
		k_success.push_back(k);
		k_weight .push_back(wt);
		sum_wt += wt;
	}

	// double 	rand = (rnd(fft_mt) + 1.0)/ 2;			// rand = 0..1
	// int 	r_id = (int)(rand * k_success.size());	// r_id = 0..k_s.size()-1
	// int 	k_id = k_success[r_id];					// k_id  	of next start point
	// _pos 	next_pos = temp_traj[k_id].pos.back(); 	// position of next start point

	double rand;
	rand = (rnd(fft_mt) + 1.0)/ 2;					// rand = 0..1
	rand *= sum_wt;									// rand = 0..sum_wt

	double 	tmp = 0.0;
	int 	k_id;

	for( int n=0; n<k_success.size(); n++ ){
		tmp += k_weight[n];
		if( rand <= tmp ){
			k_id = k_success[n];
			break;
		}
	}

	// _pos 	next_pos = temp_traj[k_id].pos.back(); 	// position of next start point
	_pos 	next_pos = temp_traj[k_id].end_pos; 	// position of next start point

	path.start.push_back( next_pos 	   	  );
	path.traj .push_back( temp_traj[k_id] );

	return;
}



void end_trial
// --------------------------------------------------------------------
//
// Purpose: procedure when there is no path overcoming [i,i+1] interface
// 			path_b.prob = path_b.w_ib = 0 after i-th interface
// 
(
	int 	i, 						// i-th interface (i=0..n-1)
	_param 	&param,
	_path 	&path 
)
// --------------------------------------------------------------------
{
	std::cout << "there is no trial overcoming [" << param.x_div[i] << ", " << param.x_div[i+1]
			  << "] interface" << std::endl;

	// for i-th interface
	_traj temp_traj;
	temp_traj.k = -1;
	temp_traj.n =  0;
	path.traj.push_back(temp_traj);

	for( int j=i+1; j<param.n_divs; j++ ){
		path.prob.push_back(0.0);
		path.w_ib.push_back(0.0);
		_traj temp_traj;
		temp_traj.k = -1;
		temp_traj.n =  0;
		path.traj.push_back(temp_traj);
	}
	return;
}


void write_prog
// --------------------------------------------------------------------
//
// Function: output progress file
// 
(
	int 	seed,
	_param 	&param,
	_path	&path 
)
// --------------------------------------------------------------------
{
	FILE *f_out;
	char fname[256];
	sprintf(fname, "%s/prog_s%03d.txt", PATH_OUT, seed); // put this in PATH_OUT for writing
	f_out = fopen( fname, "w" );
	fprintf(f_out, "strt_div = %d\n", param.strt_div );
	fprintf(f_out, "prob w_ib k_id step\n");

	for( int i=0; i<path.prob.size(); i++ ){
		fprintf(f_out, "%lf %lf %d %d\n", path.prob[i], path.w_ib[i], path.traj[i].k, path.traj[i].n );
	}
	fclose(f_out);

	return;
}



void read_config_strt
// --------------------------------------------------------------------
//
// Function: read configuration of beads from the initial file
// 
(
	int 	seed  ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid 
)
// --------------------------------------------------------------------
{
	cargo.init(grid);
	fgnup.init(grid,cargo);

	char ifs_cg[128];
	char ifs_fg[128];
	sprintf(ifs_cg, "%s/s%03d_cargo.vtk", PATH_INP, seed ); 
	sprintf(ifs_fg, "%s/s%03d_fgnup.vtk", PATH_INP, seed ); 
	cargo.set_beads(ifs_cg);
	fgnup.set_beads(ifs_fg);

	return;
}


void read_config_cont
// --------------------------------------------------------------------
//
// Function: read configuration of beads from the progress file
// 
(
	int 	seed  ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_path 	&path
)
// --------------------------------------------------------------------
{
	cargo.init(grid);
	fgnup.init(grid,cargo);

	_traj 	cont = path.traj.back();
	int 	n_it = path.traj.size();

	char ifs_cg[128];
	char ifs_fg[128];
	sprintf(ifs_cg, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_vtx.vtk", PATH_REC, seed, n_it, cont.k, cont.n-1 );
	sprintf(ifs_fg, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d.vtk"	  , PATH_REC, seed, n_it, cont.k, cont.n-1 );

	std::cout << ifs_cg << std::endl;

	cargo.set_beads(ifs_cg);
	fgnup.set_beads(ifs_fg);

	return;
}

void read_prog
// --------------------------------------------------------------------
//
// Function: read progress file if existed
// 
(
	_param 	&param,
	_path 	&path ,
	int 	seed
)
// --------------------------------------------------------------------
{
	char fname[128];
	sprintf(fname, "%s/prog_s%03d.txt", PATH_REC, seed); // put this in PATH_REC for reading
	std::ifstream ifs(fname);
	std::string str;

	if(ifs.fail()){ // progress file does not exist
		if( param.strt_div != 0 ){
			std::cout << "\x1b[31m" << 
			"ERROR: " << fname << " does not exist, strt_div = " << param.strt_div << 
			"\x1b[349" << std::endl;
			exit(1);
		}
		return;
	}

	int strt_div_num;
	std::getline(ifs, str); std::sscanf(str.data(), "strt_div = %d", &strt_div_num);
	std::getline(ifs, str); // skip

	if( strt_div_num != param.strt_div ){
		std::cout << "\x1b[31m" << 
		"ERROR: strt_div does not match: " << strt_div_num << " " << param.strt_div << 
		"\x1b[349" << std::endl;
		exit(1);
	}

	while(std::getline(ifs, str)){
		double  prob, w_ib;
		int 	k_id, step;
		std::sscanf(str.data(), "%lf %lf %d %d", &prob, &w_ib, &k_id, &step);
		_pos  tmp_pos ;
		_traj tmp_traj;
		tmp_traj.k = k_id;
		tmp_traj.n = step;
		path.start.push_back(tmp_pos);	//empty
		path.traj.push_back(tmp_traj);
		path.prob.push_back(prob);
		path.w_ib.push_back(w_ib);
	}


	return;
}

void ffs_init
// --------------------------------------------------------------------
//
// Function:
// 
(
	_param 	&param,
	_path 	&path ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
)
// --------------------------------------------------------------------
{
	read_prog(param, path, seed);

	if(param.strt_div == 0){
		/* start simulation from the begining */
		read_config_strt(seed,cargo,fgnup,grid);
		_pos tmp_pos;
		convert_sys_to_pos(tmp_pos, cargo, fgnup);
		path.start.push_back(tmp_pos);
		output_config(seed,0,0,0,cargo,fgnup);
	}else{
		/* continuation from the previous run */
		read_config_cont(seed,cargo,fgnup,grid,path);
		_pos tmp_pos;
		convert_sys_to_pos(tmp_pos, cargo, fgnup);
		path.start.push_back(tmp_pos);
	}

	fft_mt.seed(seed+12345);
	return;
}


void check_time
// --------------------------------------------------------------------
//
// Function: check if duration time does not exceed TIME_MAX
// 
(
	int 	i 	  ,		// i-th interface
	_param 	&param,
	_path 	&path ,
	int 	seed  ,
	clock_t clock_str
)
// --------------------------------------------------------------------
{
	clock_t clock_end = clock();
	double 	duration_hour = (double)(clock_end - clock_str) / CLOCKS_PER_SEC / 3600;

	if( duration_hour > TIME_MAX ){
		param.strt_div = i;
		write_prog( seed, param, path );
		std::cout << "\n" << "ffs_sim terminated before i = " << i << " (time out)" << std::endl;
		exit(2);
	}
	return;
}

void ffs_main
// --------------------------------------------------------------------
//
// Function: main FFS simulation
// 
(
	_param 	&param,
	_path 	&path ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
)
// --------------------------------------------------------------------
{
	clock_t clock_str = clock();

	for( int i=param.strt_div; i<param.n_divs; i++ ){

		check_time( i, param, path, seed, clock_str );

		std::vector<_traj> temp_traj;
		launch_ksim( i, param, path, temp_traj, cargo, fgnup, grid, bcs, seed );
		calc_prob_f( i, param, path, temp_traj );
		calc_weight( i, path );

		// std::cout << i << " " << path.prob[i] << std::endl;

		if( path.prob[i] == 0 ){
			end_trial( i, param, path );
			break;
		}

		choose_next( i, param, path, temp_traj );
	}
}

void ffs_sim
// --------------------------------------------------------------------
//
// Function: main FFS simulation
// 
(
	_param 	&param,
	_path 	&path ,
	_cargo 	&cargo,
	_fgnup 	&fgnup,
	_grid 	&grid ,
	_bcs 	&bcs  ,
	int 	seed
)
// --------------------------------------------------------------------
{

	ffs_init( param, path, cargo, fgnup, grid, bcs, seed );
	ffs_main( param, path, cargo, fgnup, grid, bcs, seed );

	std::cout << "\n" << "ffs_sim finished" << std::endl;
	return;
}



// void write_path
// // --------------------------------------------------------------------
// //
// // Function: output path
// // 
// (
// 	int 	seed,
// 	_param 	&param,
// 	_path	&path , 
// 	_fgnup 	&fgnup,
// 	_cargo 	&cargo
// )
// // --------------------------------------------------------------------
// {
// 	char fn_fg[256];
// 	char fn_cg[256];
// 	int  count = 0;

// 	for( int i=0; i<param.n_divs; i++ ){

// 		if( path.prob[i]==0 ) break;

// 		for( int j=0; j<path.traj[i].pos.size()-1; j++ ){
// 			// if( j%D_OUTINTVL == 0 ){
// 			convert_pos_to_sys(path.traj[i].pos[j],fgnup,cargo);
// 			sprintf(fn_fg, "%s/fg_s%03d_%05d", PATH_OUT, seed, count);
// 			sprintf(fn_cg, "%s/cg_s%03d_%05d", PATH_OUT, seed, count);
// 			fgnup.output(fn_fg);
// 			cargo.output(fn_cg);
// 			// }
// 			count ++;
// 		}
// 	}

// 	int last_i = param.n_divs - 1;
// 	int last_j = path.traj[last_i].pos.size() - 1;
// 	convert_pos_to_sys(path.traj[last_i].pos[last_j],fgnup,cargo);
// 	sprintf(fn_fg, "%s/fg_s%03d_%05d", PATH_OUT, seed, count);
// 	sprintf(fn_cg, "%s/cg_s%03d_%05d", PATH_OUT, seed, count);
// 	fgnup.output(fn_fg);
// 	cargo.output(fn_cg);

// 	return;
// }


void write_rate
// --------------------------------------------------------------------
//
// Function: output rate
// 
(
	int 	seed,
	_param 	&param,
	_path	&path 
)
// --------------------------------------------------------------------
{
	FILE *f_out;
	char fname[256];
	sprintf(fname, "%s/rate_s%03d.txt", PATH_OUT, seed);
	f_out = fopen( fname, "w" );
	fprintf(f_out, "prob w_ib k_id step\n");

	int count = path.traj[0].n;
	fprintf(f_out, "%lf %lf %d %d\n", path.prob[0], 1.0, path.traj[0].k, path.traj[0].n );
	for( int i=1; i<param.n_divs; i++ ){
		fprintf(f_out, "%lf %lf %d %d\n", path.prob[i], path.w_ib[i-1], path.traj[i].k, path.traj[i].n );
		count += path.traj[i].n;
	}
	fprintf(f_out, "%lf %lf %d %d\n", 1.0, path.w_ib[param.n_divs-1], -1, count+1 );

	// count+1 for the last step
	// alignment of path.w_ib

	fclose(f_out);

	return;
}


void write_out
// --------------------------------------------------------------------
//
// Function: output path and rate 
// 
(
	int 	seed  ,
	_param 	&param,
	_path	&path , 
	_cargo 	&cargo,
	_fgnup 	&fgnup
)
// --------------------------------------------------------------------
{
	// write_path(seed, param, path, fgnup, cargo);
	write_rate(seed, param, path);
	return;
}





