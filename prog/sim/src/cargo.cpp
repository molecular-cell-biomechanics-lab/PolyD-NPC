#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include "vec.h"
#include "env.h"
#include "bcs.h"
#include "beads.h"
#include "cargo.h"


static double 	coeff_ex_tra; 		// coefficient of the external force (translation)
static double 	coeff_ex_rot; 		// coefficient of the external force (rotation)
static double 	rand_std_tra; 		// deviation of rand force (normalized for unif dist, translation)
static double 	rand_std_rot; 		// deviation of rand force (normalized for unif dist, rotation)
static double 	k_adjust;			// coefficient for adjustment force

// std::mt19937 							c_mt   (RANDSEED+1); 	// random number generator
std::mt19937 							c_mt; 	// random number generator
std::uniform_real_distribution<double> 	c_unif (-1.0, 1.0);	// uniform distribution

void _cargo::set_beads_asc
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	int N_bead, N_face;
	double x, y, z;
	double val;
	int dum;
	int a, b, c;
	int tmp_i;

	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "%lf %lf %lf", &x, &y, &z );
	center = _vec<double>(x, y, z);
	
	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		_vec<double> tmp(x, y, z);
		pos.push_back( tmp );
		ori.push_back( tmp - center );
		ori_0 = ori;
		// frc.push_back( _vec<double>(0, 0, 0) );
	}

	// std::getline(ifs, str); 							  // skip 1 line
	// for( int i=0; i<N_bead; i++ ) std::getline(ifs, str); // skip N_bead line (VERTICES)

	std::getline(ifs, str); std::sscanf( str.data(), "POLYGONS %d %d", &N_face, &dum );
	for( int i=0; i<N_face; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "3 %d %d %d", &a, &b, &c );
		face.push_back( std::tuple<int,int,int>(a,b,c) );
	}

	int count = 0;
	for( int i=0; i<3; i++ ) std::getline(ifs, str); // skip 3 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf", &val );
		if( val != 0.0 ){
			hyd_id.push_back(i);
			hyd.push_back(val);
			hyd_inv_id.push_back(count);
			count ++;
		}else{
			hyd_inv_id.push_back(-1);
		}
	}

	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%d", &tmp_i );
		if( hyd_inv_id[i] != -1 ){
			hyd_state.push_back(tmp_i);
		}
	}

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		_vec<double> tmp(x, y, z);
		frc.push_back(tmp);
	}


	_beads::init_pair();
	// std::cout << "finished setting " << pos.size() << " beads from " << ifs_name << std::endl;

	return;
}



void _cargo::set_beads_asc_vtx
// --------------------------------------------------------------------
//
// Function: Input for vertex format file
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	int N_bead, N_vtx;
	double x, y, z;
	double val;
	int dum;
	int a, b, c;
	int tmp_i;

	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "%lf %lf %lf", &x, &y, &z );
	center = _vec<double>(x, y, z);
	
	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		_vec<double> tmp(x, y, z);
		pos.push_back( tmp );
		ori.push_back( tmp - center );
		ori_0 = ori;
	}

	std::getline(ifs, str); std::sscanf( str.data(), "VERTICES %d %d", &N_vtx, &dum );
	for( int i=0; i<N_vtx; i++ ) std::getline(ifs, str); // skip N_vtx lines

	int count = 0;
	for( int i=0; i<3; i++ ) std::getline(ifs, str); // skip 3 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf", &val );
		if( val != 0.0 ){
			hyd_id.push_back(i);
			hyd.push_back(val);
			hyd_inv_id.push_back(count);
			count ++;
		}else{
			hyd_inv_id.push_back(-1);
		}
	}

	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%d", &tmp_i );
		if( hyd_inv_id[i] != -1 ){
			hyd_state.push_back(tmp_i);
		}
	}

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 lines
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		_vec<double> tmp(x, y, z);
		frc.push_back(tmp);
	}


	_beads::init_pair();
	// std::cout << "finished setting " << pos.size() << " beads from " << ifs_name << std::endl;

	return;
}


void _cargo::set_beads_bin
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*fname
)
// --------------------------------------------------------------------
{

	int 	N_bead, N_face;
	double 	x, y, z;
	double 	val;
	int 	dum;
	float 	tmp_x, tmp_y, tmp_z, tmp;
	int 	face_0, face_1, face_2;
	int 	tmp_i;


	char ifs_name[64];
	sprintf( ifs_name, "%s", fname );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }


	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "%lf %lf %lf", &x, &y, &z );
	center = _vec<double>(x, y, z);

	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_x, sizeof(float)); SwapEnd(tmp_x);
		ifs.read((char *) &tmp_y, sizeof(float)); SwapEnd(tmp_y);
		ifs.read((char *) &tmp_z, sizeof(float)); SwapEnd(tmp_z);
		pos.push_back(_vec<double>(tmp_x, tmp_y, tmp_z));
	}

	std::getline(ifs, str); std::sscanf( str.data(), "POLYGONS %d %d", &N_face, &dum );
	for( int i=0; i<N_face; i++ ){
		ifs.read((char *) &dum, sizeof(int));
		ifs.read((char *) &face_0, sizeof(int)); SwapEnd(face_0);
		ifs.read((char *) &face_1, sizeof(int)); SwapEnd(face_1);
		ifs.read((char *) &face_2, sizeof(int)); SwapEnd(face_2);
		face.push_back( std::tuple<int,int,int>(face_0, face_1, face_2) );
	}

	int count = 0;
	for( int i=0; i<3; i++ ) std::getline(ifs, str); // skip 3 lines
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp, sizeof(float)); SwapEnd(tmp);
		if( tmp != 0.0 ){
			hyd_id.push_back(i);
			hyd.push_back(tmp);
			hyd_inv_id.push_back(count);
			count ++;
		}else{
			hyd_inv_id.push_back(-1);
		}
	}

	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 lines
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_i, sizeof(int)); SwapEnd(tmp_i);
		if( hyd_inv_id[i] != -1 ){
			hyd_state.push_back(tmp_i);
		}
	}

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_x, sizeof(float)); SwapEnd(tmp_x);
		ifs.read((char *) &tmp_y, sizeof(float)); SwapEnd(tmp_y);
		ifs.read((char *) &tmp_z, sizeof(float)); SwapEnd(tmp_z);
		_vec<double> tmp_f(tmp_x, tmp_y, tmp_z);
		tmp_f /= FRC_PN;
		frc.push_back(tmp_f);
	}

	for( int i=0; i<N_bead; i++ ){
		ori.  push_back( pos[i] - center );
		ori_0.push_back( pos[i] - center );
	}

	_beads::init_pair();

	return;
}


void _cargo::set_beads
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	// _cargo::set_beads_bin(ifs_name);
	// _cargo::set_beads_asc(ifs_name);
	_cargo::set_beads_asc_vtx(ifs_name);
	return;
}


void _cargo::set_beads
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name,
	double 		x,
	double 		y,
	double 		z
)
// --------------------------------------------------------------------
{
	// set_beads_asc(ifs_name);
	// set_beads_bin(ifs_name);
	set_beads_asc_vtx(ifs_name);
	set_center(x,y,z);
	return;
}


void _cargo::read_vars
// --------------------------------------------------------------------
//
// Purpose: read member variables from cargo.txt
// 
(
)
// --------------------------------------------------------------------
{

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, "cargo.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "R_cargo = %lf", &(this->R_cargo) );

	return;
}

void _cargo::calc_vars
// --------------------------------------------------------------------
//
// Purpose: unit conversion and calculate vairables
// 
(
)
// --------------------------------------------------------------------
{
	D_trans = K_BOLTZ * T_body / 6.0 / PI / h_visc / R_cargo;
	D_rotat = K_BOLTZ * T_body / 8.0 / PI / h_visc / R_cargo / R_cargo / R_cargo;
	return;
}


void _cargo::calc_static
// --------------------------------------------------------------------
//
// Purpose: calculate static variables
// 
(
)
// --------------------------------------------------------------------
{
	coeff_ex_tra = D_trans / K_BOLTZ / T_body * t_delt;
	coeff_ex_rot = D_rotat / K_BOLTZ / T_body * t_delt;
	rand_std_tra = sqrt(3) * sqrt( 2 * D_trans * t_delt);
	rand_std_rot = sqrt(3) * sqrt( 2 * D_rotat * t_delt);
	k_adjust 	 = KBT_0 * POTE_ADJUST;
	return;
}


void _cargo::set_init
// --------------------------------------------------------------------
//
// Purpose: set initial variables
// 
(
)
// --------------------------------------------------------------------
{
	quat[0] = QT_INIT_0;
	quat[1] = QT_INIT_1;
	quat[2] = QT_INIT_2;
	quat[3] = QT_INIT_3;

	d_qt[0] = 0.0;
	d_qt[1] = 0.0;
	d_qt[2] = 0.0;
	d_qt[3] = 0.0;

	d_theta.IN( 0.0, 0.0, 0.0 );

	calc_A();
	calc_B();

	return;
}

void _cargo::display_vars
// --------------------------------------------------------------------
//
// Purpose: display input variables
// 
(
)
// --------------------------------------------------------------------
{

    std::cout << "\x1b[34m"	 << std::endl;
	std::cout << "R_cargo = " << R_cargo << std::endl;
	std::cout << "D_trans = " << D_trans << std::endl;
	std::cout << "D_rotat = " << D_rotat << std::endl;
    std::cout << "\x1b[39m"	<< std::endl;
    return;

}


void _cargo::calc_A
// --------------------------------------------------------------------
//
// Function: calculate A-matrix
// 
(
)
// --------------------------------------------------------------------
{
	mat_A[0][0] = quat[0]*quat[0] + quat[1]*quat[1] - quat[2]*quat[2] - quat[3]*quat[3];
	mat_A[0][1] = 2*(quat[1]*quat[2] - quat[0]*quat[3]);
	mat_A[0][2] = 2*(quat[1]*quat[3] + quat[0]*quat[2]);
	mat_A[1][0] = 2*(quat[1]*quat[2] + quat[0]*quat[3]);
	mat_A[1][1] = quat[0]*quat[0] - quat[1]*quat[1] + quat[2]*quat[2] - quat[3]*quat[3];
	mat_A[1][2] = 2*(quat[2]*quat[3] - quat[0]*quat[1]);
	mat_A[2][0] = 2*(quat[1]*quat[3] - quat[0]*quat[2]);
	mat_A[2][1] = 2*(quat[2]*quat[3] + quat[0]*quat[1]);
	mat_A[2][2] = quat[0]*quat[0] - quat[1]*quat[1] - quat[2]*quat[2] + quat[3]*quat[3];
	return;
}

void _cargo::calc_B
// --------------------------------------------------------------------
//
// Function: calculate A-matrix
// 
(
)
// --------------------------------------------------------------------
{
	mat_B[0][0] = -quat[1] / 2.0;
	mat_B[0][1] = -quat[2] / 2.0;
	mat_B[0][2] = -quat[3] / 2.0;
	mat_B[1][0] = +quat[0] / 2.0;
	mat_B[1][1] = -quat[3] / 2.0;
	mat_B[1][2] = +quat[2] / 2.0;
	mat_B[2][0] = +quat[3] / 2.0;
	mat_B[2][1] = +quat[0] / 2.0;
	mat_B[2][2] = -quat[1] / 2.0;
	mat_B[3][0] = -quat[2] / 2.0;
	mat_B[3][1] = +quat[1] / 2.0;
	mat_B[3][2] = +quat[0] / 2.0;
	return;
}


void _cargo::sum_force
// --------------------------------------------------------------------
//
// Function: calculate summation of force
// 
(
)
// --------------------------------------------------------------------
{
	frc_sum.IN(0,0,0);
	for( int i=0; i<frc.size(); i++ ){
		frc_sum += frc[i];
	}
	return;
}

void _cargo::sum_torque
// --------------------------------------------------------------------
//
// Function: calculate summation of torque
// 
(
)
// --------------------------------------------------------------------
{
	trq_sum.IN(0,0,0);
	for( int i=0; i<frc.size(); i++ ){
		trq_sum += ori[i] % frc[i];
	}
	return;
}


void _cargo::adjust_force
// --------------------------------------------------------------------
//
// Function: adjust force so that cargo does not go far away from the pore
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	// double thre_z = bcs.cy_top - ADJUST_Z_THRE;
	// double thre_z = bcs.cy_btm + ADJUST_Z_THRE;
	double thre_z = bcs.cy_btm - bcs.L_z + ADJUST_Z_THRE;
	if( center.z < thre_z ){
		frc_sum.z -= k_adjust * (center.z - thre_z);
	}
	// frc_sum.z += 10*k_adjust; //for debug
	return;
}


void _cargo::calc_torque_body
// --------------------------------------------------------------------
//
// Function: calculate trq_sum_body (A^t T)
// 
(
)
// --------------------------------------------------------------------
{
	trq_sum_body.x = mat_A[0][0]*trq_sum.x + mat_A[1][0]*trq_sum.y + mat_A[2][0]*trq_sum.z;
	trq_sum_body.y = mat_A[0][1]*trq_sum.x + mat_A[1][1]*trq_sum.y + mat_A[2][1]*trq_sum.z;
	trq_sum_body.z = mat_A[0][2]*trq_sum.x + mat_A[1][2]*trq_sum.y + mat_A[2][2]*trq_sum.z;
	return;
}


void _cargo::calc_theta
// --------------------------------------------------------------------
//
// Function: calculate d_theta
// 
(
)
// --------------------------------------------------------------------
{
	d_theta.x = coeff_ex_rot * trq_sum_body.x + rand_std_rot * c_unif(c_mt);
	d_theta.y = coeff_ex_rot * trq_sum_body.y + rand_std_rot * c_unif(c_mt);
	d_theta.z = coeff_ex_rot * trq_sum_body.z + rand_std_rot * c_unif(c_mt);
	return;
}

void _cargo::calc_d_quat
// --------------------------------------------------------------------
//
// Function: calculate d_qt
// 
(
)
// --------------------------------------------------------------------
{
	d_qt[0] = mat_B[0][0]*d_theta.x + mat_B[0][1]*d_theta.y + mat_B[0][2]*d_theta.z;
	d_qt[1] = mat_B[1][0]*d_theta.x + mat_B[1][1]*d_theta.y + mat_B[1][2]*d_theta.z;
	d_qt[2] = mat_B[2][0]*d_theta.x + mat_B[2][1]*d_theta.y + mat_B[2][2]*d_theta.z;
	d_qt[3] = mat_B[3][0]*d_theta.x + mat_B[3][1]*d_theta.y + mat_B[3][2]*d_theta.z;
	return;
}

void _cargo::calc_t_quat
// --------------------------------------------------------------------
//
// Function: calculate t_qt
// 
(
)
// --------------------------------------------------------------------
{
	t_qt[0] = quat[0] + d_qt[0];
	t_qt[1] = quat[1] + d_qt[1];
	t_qt[2] = quat[2] + d_qt[2];
	t_qt[3] = quat[3] + d_qt[3];
	return;
}

void _cargo::norm_quat
// --------------------------------------------------------------------
//
// Function: normalize quaternion
// 
(
)
// --------------------------------------------------------------------
{
	double qnorm = t_qt[0]*t_qt[0] + t_qt[1]*t_qt[1] + t_qt[2]*t_qt[2] + t_qt[3]*t_qt[3];
	qnorm = sqrt(qnorm);
	quat[0] = t_qt[0] / qnorm;
	quat[1] = t_qt[1] / qnorm;
	quat[2] = t_qt[2] / qnorm;
	quat[3] = t_qt[3] / qnorm;
	return;
}

void _cargo::update_center
// --------------------------------------------------------------------
//
// Function: update center position
// 
(
)
// --------------------------------------------------------------------
{
	center.x = center.x + coeff_ex_tra * frc_sum.x + rand_std_tra * c_unif(c_mt);
	center.y = center.y + coeff_ex_tra * frc_sum.y + rand_std_tra * c_unif(c_mt);
	center.z = center.z + coeff_ex_tra * frc_sum.z + rand_std_tra * c_unif(c_mt);
// std::cout << seed << " " << c_unif(c_mt) << std::endl; exit(1);
	return;
}

void _cargo::update_quat
// --------------------------------------------------------------------
//
// Function: update quaternion and matrix A and B
//
(
)
// --------------------------------------------------------------------
{
	// update quaternion
	calc_torque_body();
	calc_theta();
	calc_d_quat();
	calc_t_quat();
	norm_quat();

	// update matrix A and B
	calc_A();
	calc_B();
	return;
}

void _cargo::update_orient
// --------------------------------------------------------------------
//
// Function: update orientation vector
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<ori.size(); i++ ){
		ori[i].x = mat_A[0][0]*ori_0[i].x + mat_A[0][1]*ori_0[i].y + mat_A[0][2]*ori_0[i].z;
		ori[i].y = mat_A[1][0]*ori_0[i].x + mat_A[1][1]*ori_0[i].y + mat_A[1][2]*ori_0[i].z;
		ori[i].z = mat_A[2][0]*ori_0[i].x + mat_A[2][1]*ori_0[i].y + mat_A[2][2]*ori_0[i].z;
	}
	return;
}

void _cargo::update_pos
// --------------------------------------------------------------------
//
// Function: update beads position 
// 
(
)
// --------------------------------------------------------------------
{
	for( int i=0; i<pos.size(); i++ ){
		pos[i].x = center.x + ori[i].x;
		pos[i].y = center.y + ori[i].y;
		pos[i].z = center.z + ori[i].z;
	}
	return;
}



void _cargo::make_list
// --------------------------------------------------------------------
//
// Function: make pair list
// 
(
)
// --------------------------------------------------------------------
{
	return;
}

void _cargo::make_list
// --------------------------------------------------------------------
//
// Function: make pair list
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	return;
}

// void _cargo::make_list
// // --------------------------------------------------------------------
// //
// // Function: make pair list
// // 
// (
// 	_bcs 	&bcs,
// 	_beads	&o_beads_0
// )
// // --------------------------------------------------------------------
// {
// 	_beads::make_list_cross_rp( bcs, o_beads_0, 0 );
// 	_beads::make_list_cross_fg( bcs, o_beads_0, 0 );
// 	return;
// }

// void _cargo::make_list
// // --------------------------------------------------------------------
// //
// // Function: make pair list
// // 
// (
// 	_bcs 	&bcs,
// 	_beads	&o_beads_0,
// 	_beads	&o_beads_1
// )
// // --------------------------------------------------------------------
// {
// 	_beads::make_list_cross_rp( bcs, o_beads_0, 0 );
// 	_cargo::make_list_cross_fg( bcs, o_beads_0, 0 );
// 	_beads::make_list_cross_rp( bcs, o_beads_1, 1 );
// 	_cargo::make_list_cross_fg( bcs, o_beads_1, 1 );
// 	return;
// }


void _cargo::make_list
// --------------------------------------------------------------------
//
// Function: make pair list using grid list
// 
(
	_bcs 	&bcs,
	_grid 	&grid
)
// --------------------------------------------------------------------
{
	grid.sort_beads(this);
	return;
}

// void _cargo::make_list
// // --------------------------------------------------------------------
// //
// // Function: make pair list using grid list
// // 
// (
// 	_bcs 	&bcs,
// 	_grid 	&grid,
// 	_beads	&o_beads_0
// )
// // --------------------------------------------------------------------
// {
// 	grid.sort_beads(this);
// 	_beads::make_list_cross_rp( bcs, grid, o_beads_0, 0 );
// 	_cargo::make_list_cross_fg( bcs, grid, o_beads_0, 0 );
// 	return;
// }

// void _cargo::make_list
// // --------------------------------------------------------------------
// //
// // Function: make pair list using grid list
// // 
// (
// 	_bcs 	&bcs,
// 	_grid 	&grid,
// 	_beads	&o_beads_0,
// 	_beads	&o_beads_1
// )
// // --------------------------------------------------------------------
// {

// 	grid.sort_beads(this);
// 	_beads::make_list_cross_rp( bcs, grid, o_beads_0, 0 );
// 	_cargo::make_list_cross_fg( bcs, grid, o_beads_0, 0 );
// 	_beads::make_list_cross_rp( bcs, grid, o_beads_1, 1 );
// 	_cargo::make_list_cross_fg( bcs, grid, o_beads_1, 1 );


// 	return;
// }


// bool _cargo::find_elem_cargo
// // --------------------------------------------------------------------
// //
// // Function: check exisitence of the element in an array
// // 			 TRUE  if the element exists
// // 			 FLASE if the element does NOT exist
// // 
// (
// 	int 	cross_id,
// 	int 	elm
// )
// // --------------------------------------------------------------------
// {
// 	for( int i=0; i<hyd.size(); i++ ){
// 		if( _beads::find_elem(cross_pair_fg[cross_id][i],elm) == true ){
// 			return true;
// 		}
// 	}
// 	return false;
// }


// void _cargo::make_list_cross_fg
// // --------------------------------------------------------------------
// //
// // Function: make cross pair list (for hydrophobic)
// //			 consider cargo's whole surface as one patch
// // 
// (
// 	_bcs 	&bcs, 
// 	_beads 	&o_beads,
// 	int 	cross_id
// )
// // --------------------------------------------------------------------
// {

// 	// for( int i=0; i<hyd.size(); i++ ){
// 	// 	cross_pair_fg[cross_id][i].clear();
// 	// }
	
// 	std::vector<_vec<double> > o_pos = o_beads.pos;

// 	// k_off
// 	for( int i=0; i<hyd.size(); i++ ){
// 		int n_erase = 0;
// 		for( int j=0; j<cross_pair_fg[cross_id][i].size(); j++ ){
// 			if( k_roll() < kof_fg_t ){
// 				cross_pair_fg[cross_id][i].erase(cross_pair_fg[cross_id][i].begin()+j-n_erase);
// 				n_erase ++;
// 			}
// 		}
// 	}
	
// 	// k_on
// 	for( int j=0; j<o_beads.hyd.size(); j++ ){

// 		int jj = o_beads.hyd_id[j];
// 		if( find_elem_cargo(cross_id, jj) == true ) continue; // already attached
// 		_vec<double> pos_key = o_pos[jj];
// 		int 	vtx_near = -1;
// 		double 	dist_vtx = 1e3;

// 		for( int i=0; i<hyd.size(); i++ ){
// 			int ii = hyd_id[i];
// 			_vec<double> dr = pos[ii] - pos_key;
// 			bcs.adjust_periodic(&dr);
// 			double dr_2 = dr.sqr();
// 			if( dr_2 < cut_fg_list_2 && dr_2 < dist_vtx ){
// 				vtx_near = i;
// 				dist_vtx = dr_2;
// 			}
// 		}

// 		if( vtx_near != -1 && k_roll() < kon_fg_t ){
// 			cross_pair_fg[cross_id][vtx_near].push_back(jj); // hyd id -> bead id
// 		}

// 	}

// 	return;
// }


// void _cargo::make_list_cross_fg
// // --------------------------------------------------------------------
// //
// // Function: make cross pair list (for hydrophobic) using grid list
// //			 consider cargo's whole surface as one patch
// // 
// (
// 	_bcs 	&bcs, 
// 	_grid 	&grid,
// 	_beads 	&o_beads,
// 	int 	cross_id
// )
// // --------------------------------------------------------------------
// {
// 	std::vector<_vec<double> > o_pos = o_beads.pos;

// 	// for( int i=0; i<hyd.size(); i++ ){
// 	// 	cross_pair_fg[cross_id][i].clear();
// 	// }

// 	// k_off
// 	for( int i=0; i<hyd.size(); i++ ){
// 		int n_erase = 0;
// 		for( int j=0; j<cross_pair_fg[cross_id][i].size(); j++ ){
// 			if( k_roll() < kof_fg_t ){
// 				cross_pair_fg[cross_id][i].erase(cross_pair_fg[cross_id][i].begin()+j-n_erase);
// 				n_erase ++;
// 			}
// 		}
// 	}

// 	// among neighboring grids (including itself)
// 	for( int bi=0; bi<grid.N_grid; bi++ ){
// 	for( int bj : grid.grid_nbr[bi] ){ 	// grid_nbr include itself

// 		int   N_bead_i =   		 binned_beads[bi].size();
// 		int o_N_bead_j = o_beads.binned_beads[bj].size();

// 		for( int j=0; j<o_N_bead_j; j++ ){
// 			int jj = o_beads.binned_beads[bj][j];
// 			int fg_j = o_beads.hyd_inv_id[jj];
// 			if( fg_j == -1 ) continue;

// 			if( find_elem_cargo(cross_id, jj) == true ) continue; // already attached
// 			_vec<double> pos_key = o_pos[jj];
// 			int 	vtx_near = -1;
// 			double 	dist_vtx = 1e3;

// 			for( int i=0; i<N_bead_i; i++ ){
// 				int ii = binned_beads[bi][i];
// 				int fg_i = hyd_inv_id[ii];
// 				if( fg_i == -1 ) continue;

// 				_vec<double> dr = pos[ii] - pos_key;
// 				bcs.adjust_periodic(&dr);
// 				double dr_2 = dr.sqr();

// 				if( dr_2 < cut_fg_list_2 && dr_2 < dist_vtx ){
// 					vtx_near = fg_i;
// 					dist_vtx = dr_2;
// 				}
// 			}

// 			// k_on
// 			if( vtx_near != -1 && k_roll() < kon_fg_t ){
// 				cross_pair_fg[cross_id][vtx_near].push_back(jj); // hyd id -> bead id
// 			}

// 		}
// 	}
// 	}

// 	return;
// }



void _cargo::calc_cargo_force
// --------------------------------------------------------------------
//
// Function: calculate force for cargo
// 
(
)
// --------------------------------------------------------------------
{
	sum_force	();
	sum_torque	();
	return;
}

void _cargo::calc_cargo_force
// --------------------------------------------------------------------
//
// Function: calculate force for cargo
// 
(
	_bcs 	&bcs 	   	// boundary 
)
// --------------------------------------------------------------------
{
	bcs.calc_force_wall(this);
	sum_force	();
	sum_torque	();
	adjust_force(bcs);
	return;
}



void _cargo::calc_force
// --------------------------------------------------------------------
//
// Function: calculate force
// 
(
)
// --------------------------------------------------------------------
{
	_beads::init_force();
	// calc_cargo_force(); 	// moved to the begining of move()
	return;
}

void _cargo::calc_force
// --------------------------------------------------------------------
//
// Function: calculate force
// 
(
	_bcs 	&bcs 	   	// boundary 
)
// --------------------------------------------------------------------
{
	_beads::init_force();
	// calc_cargo_force(bcs); // moved to the begining of move()
	return;
}


void _cargo::calc_force
// --------------------------------------------------------------------
//
// Function: calculate force between this and the other _bead object
// 
(
	_bcs 	&bcs 	  , 	// boundary 
	_beads 	&o_beads_0		// the other _bead object
)
// --------------------------------------------------------------------
{
	_beads::init_force();
	_beads::calc_force_rp( bcs, o_beads_0, 0 );
	_beads::calc_force_fg( bcs, o_beads_0, 0 );
	// _beads::calc_force_rpfg( bcs, o_beads_0, 0 );
	// calc_cargo_force(bcs); // moved to the begining of move()
	return;
}



// void _cargo::calc_force
// // --------------------------------------------------------------------
// //
// // Function: calculate force between this and the other _bead object
// // 
// (
// 	_bcs 	&bcs 	  , 	// boundary 
// 	_beads 	&o_beads_0,		// the other _bead object
// 	_beads 	&o_beads_1		// the other _bead object
// )
// // --------------------------------------------------------------------
// {
// 	_beads::init_force();
// 	_beads::calc_force_rp( bcs, o_beads_0, 0 );
// 	_beads::calc_force_fg( bcs, o_beads_0, 0 );
// 	// _beads::calc_force_rpfg( bcs, o_beads_0, 0 );
// 	_beads::calc_force_rp( bcs, o_beads_1, 1 );
// 	_beads::calc_force_fg( bcs, o_beads_1, 1 );
// 	// _beads::calc_force_rpfg( bcs, o_beads_1, 1 );
// 	// calc_cargo_force(bcs); // moved to the begining of move()
// 	return;
// }


void _cargo::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	calc_cargo_force();
	update_center();
	update_quat	 ();
	update_orient();
	update_pos	 ();
	return;
}


void _cargo::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	calc_cargo_force(bcs);
	update_center();
	update_quat	 ();
	update_orient();
	update_pos	 ();

	bcs.move_at_boundary(this);
	// std::cout << coeff_ex_tra << " " << rand_std_tra << " " << 
	// coeff_ex_rot << " " << rand_std_rot << " " <<
	// frc_sum.x << " " << frc_sum.y << " " << frc_sum.z << std::endl;
	return;
}



void _cargo::output_asc
// --------------------------------------------------------------------
//
// Function: VTK output of vertex by ascii mode
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s_vtx.vtk", ofs_name_in );

	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "%lf %lf %lf\n", center.x, center.y, center.z );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET POLYDATA\n" );
	fprintf( f_out, "POINTS %lu float\n", pos.size() );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", pos[i].x, pos[i].y, pos[i].z );
	}

	fprintf( f_out, "POLYGONS %lu %lu\n", face.size(), 4*face.size() );
	for( int i=0; i<face.size(); i++ ){
		fprintf( f_out, "3 %d %d %d\n", std::get<0>(face[i]), std::get<1>(face[i]), std::get<2>(face[i]) );
	}

	fprintf( f_out, "POINT_DATA %lu\n", pos.size() );
	fprintf( f_out, "SCALARS hyd float\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			fprintf( f_out, "%lf\n", 0.0 );
		}else{
			fprintf( f_out, "%lf\n", hyd[hyd_inv_id[i]] );
		}
	}

	fprintf( f_out, "SCALARS state int\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			fprintf( f_out, "%d\n", -1 );
		}else{
			fprintf( f_out, "%d\n", hyd_state[hyd_inv_id[i]] );
		}
	}

	/* force output is in the unit of pN */
	fprintf( f_out, "VECTORS frc float\n" );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", frc[i].x * FRC_PN, frc[i].y * FRC_PN, frc[i].z * FRC_PN );
	}

	fclose(f_out);
	return;
}

void _cargo::output_asc_vtx
// --------------------------------------------------------------------
//
// Function: for vertex format file
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s_vtx.vtk", ofs_name_in );

	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "%lf %lf %lf\n", center.x, center.y, center.z );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET POLYDATA\n" );
	fprintf( f_out, "POINTS %lu float\n", pos.size() );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", pos[i].x, pos[i].y, pos[i].z );
	}

	fprintf( f_out, "VERTICES %lu %lu\n", pos.size(), 2*pos.size() );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "1 %d\n", i );
	}

	fprintf( f_out, "POINT_DATA %lu\n", pos.size() );
	fprintf( f_out, "SCALARS hyd float\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			fprintf( f_out, "%lf\n", 0.0 );
		}else{
			fprintf( f_out, "%lf\n", hyd[hyd_inv_id[i]] );
		}
	}

	fprintf( f_out, "SCALARS state int\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			fprintf( f_out, "%d\n", -1 );
		}else{
			fprintf( f_out, "%d\n", hyd_state[hyd_inv_id[i]] );
		}
	}

	/* force output is in the unit of pN */
	fprintf( f_out, "VECTORS frc float\n" );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", frc[i].x * FRC_PN, frc[i].y * FRC_PN, frc[i].z * FRC_PN );
	}

	fclose(f_out);
	return;
}


void _cargo::output_bin
// --------------------------------------------------------------------
//
// Function: VTK output of vertex by binary mode
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s_vtx.vtk", ofs_name_in );

	std::ofstream fout;
	fout.open(ofs_name, std::ios::binary);

	if(!fout){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

    fout << "# vtk DataFile Version 2.0" 	<<std::endl;
	// fout << "cg vertex"						<<std::endl;
   	fout << center.x << " " << center.y << " " << center.z << std::endl;
    fout << "BINARY" 						<<std::endl;
    fout << "DATASET POLYDATA" 				<<std::endl;
    fout << "POINTS " << pos.size() << " float" <<std::endl;

	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = pos[i].x; SwapEnd(tmp_x);
		float tmp_y = pos[i].y; SwapEnd(tmp_y);
		float tmp_z = pos[i].z; SwapEnd(tmp_z);
		fout.write((char*)&tmp_x, sizeof(float));
		fout.write((char*)&tmp_y, sizeof(float));
		fout.write((char*)&tmp_z, sizeof(float));
	}

	fout << "POLYGONS " << face.size() << " " << 4*face.size() << std::endl;
	for( int i=0; i<face.size(); i++ ){
		int num_fc = 3;						SwapEnd(num_fc);
		int face_0 = std::get<0>(face[i]);	SwapEnd(face_0);
		int face_1 = std::get<1>(face[i]);	SwapEnd(face_1);
		int face_2 = std::get<2>(face[i]);	SwapEnd(face_2);
		fout.write((char*)&num_fc, sizeof(int));
		fout.write((char*)&face_0, sizeof(int));
		fout.write((char*)&face_1, sizeof(int));
		fout.write((char*)&face_2, sizeof(int));
	}

	fout << "POINT_DATA " << pos.size() 	<< std::endl;
	fout << "SCALARS hyd float" 			<< std::endl;
	fout << "LOOKUP_TABLE default"			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			float tmp = 0.0;
			SwapEnd(tmp);
			fout.write((char*)&tmp, sizeof(float));
		}else{
			float tmp = hyd[hyd_inv_id[i]];
			SwapEnd(tmp);
			fout.write((char*)&tmp, sizeof(float));
		}
	}

	fout << "SCALARS state int" 			<< std::endl;
	fout << "LOOKUP_TABLE default"			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			int tmp = -1;
			SwapEnd(tmp);
			fout.write((char*)&tmp, sizeof(int));
		}else{
			int tmp = hyd_state[hyd_inv_id[i]];
			SwapEnd(tmp);
			fout.write((char*)&tmp, sizeof(int));
		}
	}

	/* force output is in the unit of pN */
	fout << "VECTORS frc float" 			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = frc[i].x *FRC_PN; SwapEnd(tmp_x);
		float tmp_y = frc[i].y *FRC_PN; SwapEnd(tmp_y);
		float tmp_z = frc[i].z *FRC_PN; SwapEnd(tmp_z);
		fout.write((char*)&tmp_x, sizeof(float));
		fout.write((char*)&tmp_y, sizeof(float));
		fout.write((char*)&tmp_z, sizeof(float));
	}

	return;
}


int _cargo::count_state
// --------------------------------------------------------------------
//
// Function: count total number of bound beads
// 
(
)
// --------------------------------------------------------------------
{
	int count = 0;
	for( int i=0; i<hyd.size(); i++ ){
		count += hyd_state[i];
		// if( pair_fg[i] != -1 ) count++;
	}
	return count;
}

void _cargo::output_ctr
// --------------------------------------------------------------------
//
// Function: VTK output for frc_sum
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s_ctr.vtk", ofs_name_in );

	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "pos: nm, frc: pN, trq: pN*nm\n" );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET POLYDATA\n" );

	fprintf( f_out, "POINTS 1 float\n" );
	fprintf( f_out, "%lf %lf %lf\n", center.x, center.y, center.z );

	fprintf( f_out, "VERTICES 1 2\n" );
	fprintf( f_out, "1 0\n" );

	/* state number output */
	fprintf( f_out, "POINT_DATA 1\n" );
	fprintf( f_out, "SCALARS state int\n" );
	fprintf( f_out, "LOOKUP_TABLE default\n" );
	fprintf( f_out, "%d\n", count_state() );

	/* force output is in the unit of pN */
	fprintf( f_out, "VECTORS frc float\n" );
	fprintf( f_out, "%lf %lf %lf\n", frc_sum.x * FRC_PN, frc_sum.y * FRC_PN, frc_sum.z * FRC_PN );

	/* torque output is in the unit of pN * nm */
	fprintf( f_out, "VECTORS trq float\n" );
	fprintf( f_out, "%lf %lf %lf\n", trq_sum.x * FRC_PN, trq_sum.y * FRC_PN, trq_sum.z * FRC_PN );

	fclose(f_out);
	return;
}


void _cargo::output
// --------------------------------------------------------------------
//
// Function: VTK output of both vertex (binary) and center
// 
(
	const char 	*ofs_name
)
// --------------------------------------------------------------------
{
	// output_bin(ofs_name);
	// output_asc(ofs_name);
	output_asc_vtx(ofs_name);
	output_ctr(ofs_name);
}

void _cargo::set_center
// --------------------------------------------------------------------
//
// Function: set center position of the cargo
// 
(
	double x,
	double y,
	double z
)
// --------------------------------------------------------------------
{
	_vec<double> shift( x-center.x, y-center.y, z-center.z );
	center.x = x;
	center.y = y;
	center.z = z;
	for( int i=0; i<pos.size(); i++ ){
		pos  [i] = pos[i] + shift;
		ori  [i] = pos[i] - center;
		ori_0[i] = pos[i] - center;
	}
	calc_A();
	calc_B();
	return; 
}


void _cargo::init_rand
// --------------------------------------------------------------------
//
// Function: initialize random number generator (seed default)
// 
(
)
// --------------------------------------------------------------------
{
	c_mt.seed(RANDSEED+SHIFT_SEED);
	seed = RANDSEED;
	return;
}

void _cargo::init_rand
// --------------------------------------------------------------------
//
// Function: initialize random number generator (seed from input)
// 
(
	int 	seed_i
)
// --------------------------------------------------------------------
{
	c_mt.seed(seed_i+SHIFT_SEED);
	seed = seed_i;
	return;
}


int _cargo::get_seed
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	return seed;
}

void _cargo::set_seed
// --------------------------------------------------------------------
//
// Function: 
// 
(
	int 	seed_i
)
// --------------------------------------------------------------------
{
	init_rand(seed_i);
	return;
}

_cargo::_cargo
// --------------------------------------------------------------------
//
// Constructor: set member variables by reading from input file
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "cargo Constructor" << std::endl;
	read_vars();
	calc_vars();
	calc_static();
	set_init();
	// display_vars();
	// init_rand();
}



_cargo::_cargo
// --------------------------------------------------------------------
//
// Constructor: set member variables by reading from input file
// 
(
	int 	seed_i
)
:_beads(seed_i)
// --------------------------------------------------------------------
{
	// std::cout << "cargo Constructor" << std::endl;
	read_vars();
	calc_vars();
	calc_static();
	set_init();
	// display_vars();
	init_rand(seed_i);
}


_cargo::~_cargo
// --------------------------------------------------------------------
//
// Deconstructor:
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "cargo Deconstructor" << std::endl;
}



