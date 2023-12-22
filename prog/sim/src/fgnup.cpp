#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include "vec.h"
#include "beads.h"
#include "fgnup.h"
#include "env.h"
#include "bcs.h"


std::mt19937 							f_mt; 	// random number generator
std::uniform_real_distribution<double> 	f_unif (-1.0, 1.0);	// uniform distribution


// void _fgnup::test_set
// // --------------------------------------------------------------------
// //
// // Function: 
// // 
// (
// 	const char 	*ifs_name
// )
// // --------------------------------------------------------------------
// {
// 	int N_bead = 16720;
// 	double x, y, z;
// 	int dum, id_0, id_1;
// 	double val;
// 	char tmp[32];

// 	std::ifstream ifs(ifs_name);
// 	std::string str;

// 	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

// 	for( int i=0; i<5; i++ ) std::getline(ifs, str); // skip 5 line
	
// 	for( int i=0; i<N_bead; i++ ){
// 		std::getline(ifs, str); std::sscanf( str.data(), 
// 			"%d\t%lf\t%lf\t%lf%s\t%lf\t%d", &dum, &x, &y, &z, tmp, &val, &id_0 );
// 		hyd[i] = id_0;
// 	}

// 	return;
// }





// void _fgnup::set_beads
// // --------------------------------------------------------------------
// //
// // Function: 
// // 
// (
// 	const char 	*ifs_name
// )
// // --------------------------------------------------------------------
// {
// 	int N_bead, N_bond;
// 	double x, y, z;
// 	int dum, id_0, id_1;
// 	double val;

// 	std::ifstream ifs(ifs_name);
// 	std::string str;

// 	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

// 	for( int i=0; i<4; i++ ) std::getline(ifs, str); // skip 4 line
// 	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	
// 	for( int i=0; i<N_bead; i++ ){
// 		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
// 		pos.push_back( _vec<double>(x, y, z) );
// 		frc.push_back( _vec<double>(0, 0, 0) );
// 	}

// 	std::getline(ifs, str); // skip 1 line
// 	for( int i=0; i<N_bead; i++ ) std::getline(ifs, str); // skip N_bead lines

// 	std::getline(ifs, str); std::sscanf( str.data(), "LINES %d", &N_bond );
// 	for( int i=0; i<N_bond; i++ ){
// 		std::getline(ifs, str); std::sscanf( str.data(), "%d\t%d\t%d", &dum, &id_0, &id_1 );
// 		bond.push_back( std::pair<int,int>(id_0, id_1) );
// 	}

// 	for( int i=0; i<3; i++ ) std::getline(ifs, str); // skip 3 lines
// 	for( int i=0; i<N_bead; i++ ){
// 		std::getline(ifs, str); std::sscanf( str.data(), "%lf", &val );
// 		hyd.push_back(val);
// 	}

// 	std::cout << "finished setting " << pos.size() << " beads from " << ifs_name << std::endl;

// 	return;
// }



void _fgnup::set_beads_asc
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	int N_bead, N_pol, bd_per_pol;
	double x, y, z;
	int dum, id_0, id_1, id_2;
	double val;
	int tmp_i;

	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<4; i++ ) std::getline(ifs, str); // skip 4 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		pos.push_back( _vec<double>(x, y, z) );
		// frc.push_back( _vec<double>(0, 0, 0) );
	}

	// std::getline(ifs, str); std::sscanf( str.data(), "VERTICES %d %d", &N_pol, &dum );
	// for( int i=0; i<N_pol; i++ ){
	// 	std::getline(ifs, str);
	// 	std::istringstream stream(str);
	// 	stream >> bd_per_pol;

	// 	stream >> id_0;
	// 	stream >> id_1;
	// 	bond.push_back( std::pair<int,int>(id_0,id_1) );

	// 	for ( int j=2; j<bd_per_pol; j++ ){
	// 		stream >> id_2;
	// 		bond.push_back ( std::pair <int,int>    (id_1,id_2) 	 );
	// 		angle.push_back( std::tuple<int,int,int>(id_0,id_1,id_2) );
	// 		id_0 = id_1;
	// 		id_1 = id_2;
	// 	}
	// }

	// std::getline(ifs, str);  // skip 1 line
	// for( int i=0; i<N_pol; i++ ) std::getline(ifs, str); // skip N_pol lines

	std::getline(ifs, str); std::sscanf( str.data(), "LINES %d %d", &N_pol, &dum );
	if( N_bead == 0 ) N_pol = 0; //20230126
	for( int i=0; i<N_pol; i++ ){
		std::getline(ifs, str);
		std::istringstream stream(str);
		stream >> bd_per_pol;

		stream >> id_0;
		stream >> id_1;
		bond.push_back( std::pair<int,int>(id_0,id_1) );
		teth.push_back(1); // id_0: tethered
		teth.push_back(0); // id_1: non-tethered

		for ( int j=2; j<bd_per_pol; j++ ){
			stream >> id_2;
			bond.push_back ( std::pair <int,int>    (id_1,id_2) 	 );
			angle.push_back( std::tuple<int,int,int>(id_0,id_1,id_2) );
			id_0 = id_1;
			id_1 = id_2;
			teth.push_back(0); // id_2: non-tethered
		}
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
			pair_fg.push_back(tmp_i);
		}
	}

	for( int i=0; i<N_bead; i++ ){
		_vec<double> tmp_f(0.,0.,0.);
		frc.push_back(tmp_f);
	}

	_beads::init_pair();
	// std::cout << "finished setting " << pos.size() << " beads from " << ifs_name << std::endl;

	return;
}




void _fgnup::set_beads_bin
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*fname
)
// --------------------------------------------------------------------
{
	int 	N_bead, N_pol, bd_per_pol;
	double 	x, y, z;
	int 	dum, id_0, id_1, id_2;
	double 	val;
	float	tmp_x, tmp_y, tmp_z, tmp;
	int 	tmp_i;

	char ifs_name[64];
	sprintf( ifs_name, "%s", fname );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<4; i++ ) std::getline(ifs, str); // skip 4 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	
	for( int i=0; i<N_bead; i++ ){
		ifs.read((char *) &tmp_x, sizeof(float)); SwapEnd(tmp_x);
		ifs.read((char *) &tmp_y, sizeof(float)); SwapEnd(tmp_y);
		ifs.read((char *) &tmp_z, sizeof(float)); SwapEnd(tmp_z);
		pos.push_back(_vec<double>(tmp_x, tmp_y, tmp_z));
	}

	// std::getline(ifs, str); std::sscanf( str.data(), "LINES %d %d", &N_pol, &dum );
	// for( int i=0; i<N_pol; i++ ){
	// 	ifs.read((char *) &bd_per_pol, 	sizeof(int)); SwapEnd(bd_per_pol);
	// 	ifs.read((char *) &id_0, 		sizeof(int)); SwapEnd(id_0);

	// 	for( int i=0; i<bd_per_pol-1; i++ ){
	// 		ifs.read((char *) &id_1, sizeof(int)); SwapEnd(id_1);
	// 		bond.push_back( std::pair<int,int>(id_0,id_1) );
	// 		id_0 = id_1;
	// 	}
	// }

	std::getline(ifs, str); std::sscanf( str.data(), "LINES %d %d", &N_pol, &dum );
	if( N_bead == 0 ) N_pol = 0; //20230126
	for( int i=0; i<N_pol; i++ ){
		ifs.read((char *) &bd_per_pol, 	sizeof(int)); SwapEnd(bd_per_pol);

		ifs.read((char *) &id_0, 		sizeof(int)); SwapEnd(id_0);
		ifs.read((char *) &id_1, 		sizeof(int)); SwapEnd(id_1);
		bond.push_back( std::pair<int,int>(id_0,id_1) );
		teth.push_back(1); // id_0: tethered
		teth.push_back(0); // id_1: non-tethered

		for( int j=2; j<bd_per_pol; j++ ){
			ifs.read((char *) &id_2, sizeof(int)); SwapEnd(id_2);
			bond.push_back ( std::pair <int,int>    (id_1,id_2) 	 );
			angle.push_back( std::tuple<int,int,int>(id_0,id_1,id_2) );
			id_0 = id_1;
			id_1 = id_2;
			teth.push_back(0); // id_2: non-tethered
		}
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
			pair_fg.push_back(tmp_i);
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

	_beads::init_pair();

	return;
}




void _fgnup::set_beads
// --------------------------------------------------------------------
//
// Function: 
// 
(
	const char 	*ifs_name
)
// --------------------------------------------------------------------
{
	// _fgnup::set_beads_bin(ifs_name);
	_fgnup::set_beads_asc(ifs_name);
	return;
}



void _fgnup::read_vars
// --------------------------------------------------------------------
//
// Purpose: read member variables from fgnup.txt
// 
(
   	// char 	*ifs_name
)
// --------------------------------------------------------------------
{

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, "fgnup.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }


	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "k_tens = %lf", &(this->k_tens) );
	std::getline(ifs, str); // r_tens
	std::getline(ifs, str); std::sscanf( str.data(), "l_kuhn = %lf", &(this->l_kuhn) );
	std::getline(ifs, str); // k_bend

	return;
}

void _fgnup::calc_vars
// --------------------------------------------------------------------
//
// Purpose: unit conversion and calculate vairables
// 
(
)
// --------------------------------------------------------------------
{

	// unit conversion
	this->k_tens = this->k_tens * KBT_0 / this->l_segm / this->l_segm;	// {kBT_0/segm^2} -> (g /ns2)
	this->l_kuhn = this->l_kuhn * this->l_segm	;	// {segm}  -> (nm)

	// calculation of parameters in _param
	this->r_tens = this->l_segm;									// (nm)
	this->k_bend = KBT_0 * 0.5 * this->l_kuhn / this->l_segm;	// (g nm2 /ns2)

	return;
}


void _fgnup::display_vars
// --------------------------------------------------------------------
//
// Purpose: display input variables
// 
(
)
// --------------------------------------------------------------------
{

    std::cout << "\x1b[34m"	 << std::endl;

	std::cout << "k_tens = " << this->k_tens << std::endl;
	std::cout << "r_tens = " << this->r_tens << std::endl;
	
	std::cout << "l_kuhn = " << this->l_kuhn << std::endl;
	std::cout << "k_bend = " << this->k_bend << std::endl;
	
    std::cout << "\x1b[39m"	<< std::endl;
    return;

}



_fgnup::_fgnup
// --------------------------------------------------------------------
//
// Constructor: set member variables by reading from input file
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "fg Constructor" << std::endl;
	read_vars();
	calc_vars();
	// display_vars();
}

void _fgnup::init_rand
// --------------------------------------------------------------------
//
// Function: initialize random number generator (seed default)
// 
(
)
// --------------------------------------------------------------------
{
	f_mt.seed(RANDSEED+SHIFT_SEED_F);
	seed = RANDSEED;
	return;
}

void _fgnup::init_rand
// --------------------------------------------------------------------
//
// Function: initialize random number generator (seed from input)
// 
(
	int 	seed_i
)
// --------------------------------------------------------------------
{
	f_mt.seed(seed_i+SHIFT_SEED_F);
	seed = seed_i;
	return;
}

_fgnup::_fgnup
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
	// std::cout << "fg Constructor" << std::endl;
	read_vars();
	calc_vars();
	// display_vars();
	init_rand(seed_i);
}

_fgnup::~_fgnup
// --------------------------------------------------------------------
//
// Deconstructor:
// 
(
)
// --------------------------------------------------------------------
{
	// std::cout << "fg Deconstructor" << std::endl;
}



void _fgnup::calc_force_bond
// --------------------------------------------------------------------
//
// Function: calculate bond force 
// 
(
)
// --------------------------------------------------------------------
{
	for( int k=0; k<bond.size(); k++ ){
		int i = bond[k].first ;
		int j = bond[k].second;
		_vec<double> dr = pos[i] - pos[j];
		double dist     = dr.norm();
		double force    = -k_tens * (dist - r_tens);
		_vec<double> df = (force/dist) * dr;
		// frc[i].x += df.x;
		// frc[i].y += df.y;
		// frc[i].z += df.z;
		// frc[j].x -= df.x;
		// frc[j].y -= df.y;
		// frc[j].z -= df.z;
		frc[i] += df;
		frc[j] -= df;
	}
	return;
}

void _fgnup::calc_force_bond
// --------------------------------------------------------------------
//
// Function: calculate bond force 
// 
(
	_bcs &bcs
)
// --------------------------------------------------------------------
{
	for( int k=0; k<bond.size(); k++ ){
		int i = bond[k].first ;
		int j = bond[k].second;
		_vec<double> dr = pos[i] - pos[j];
		bcs.adjust_periodic(&dr);
		double dist     = dr.norm();
		double force    = -k_tens * (dist - r_tens);
		_vec<double> df = (force/dist) * dr;
		// frc[i].x += df.x;
		// frc[i].y += df.y;
		// frc[i].z += df.z;
		// frc[j].x -= df.x;
		// frc[j].y -= df.y;
		// frc[j].z -= df.z;
		frc[i] += df;
		frc[j] -= df;
	}
	return;
}


double sin_value
// --------------------------------------------------------------------
//
// Function:
// 
(
	_vec<double> v1,
	_vec<double> v2
)
// --------------------------------------------------------------------
{
	double sin_val;
	double cos_val = v1*v2 / v1.norm() / v2.norm();
	sin_val = sqrt( 1 - cos_val*cos_val + 1e-10 );
	return sin_val;
}

void _fgnup::calc_force_angle
// --------------------------------------------------------------------
//
// Function: calculate bending/angle force 
// 
(
)
// --------------------------------------------------------------------
{
	for( int n=0; n<angle.size(); n++ ){
		int i = std::get<0>(angle[n]);
		int j = std::get<1>(angle[n]);
		int k = std::get<2>(angle[n]);
		_vec<double> s_prev = pos[j] - pos[i];
		_vec<double> s_next = pos[k] - pos[j];
		_vec<double> p_vert = s_prev % s_next;
		p_vert = (k_bend * sin_value( s_prev, s_next ) / p_vert.norm() ) * p_vert;
		double s_prev_norm = s_prev.norm();
		double s_next_norm = s_next.norm();
		_vec<double> f_prev = (p_vert % s_prev) / s_prev_norm / s_prev_norm;
		_vec<double> f_next = (p_vert % s_next) / s_next_norm / s_next_norm;

		frc[j] += f_prev + f_next;
		frc[i] -= f_prev;
		frc[k] -= f_next;
	}
	return;
}

void _fgnup::calc_force_angle
// --------------------------------------------------------------------
//
// Function: calculate bending/angle force 
// 
(
	_bcs &bcs
)
// --------------------------------------------------------------------
{
	for( int n=0; n<angle.size(); n++ ){
		int i = std::get<0>(angle[n]);
		int j = std::get<1>(angle[n]);
		int k = std::get<2>(angle[n]);
		_vec<double> s_prev = pos[j] - pos[i];
		_vec<double> s_next = pos[k] - pos[j];
		bcs.adjust_periodic(&s_prev);
		bcs.adjust_periodic(&s_next);
		_vec<double> p_vert = s_prev % s_next;
		p_vert = (k_bend * sin_value( s_prev, s_next ) / p_vert.norm() ) * p_vert;
		double s_prev_norm = s_prev.norm();
		double s_next_norm = s_next.norm();
		_vec<double> f_prev = (p_vert % s_prev) / s_prev_norm / s_prev_norm;
		_vec<double> f_next = (p_vert % s_next) / s_next_norm / s_next_norm;

		frc[j] += f_prev + f_next;
		frc[i] -= f_prev;
		frc[k] -= f_next;
	}
	return;
}

void _fgnup::calc_force
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{
	_beads::calc_force();
	_fgnup::calc_force_bond();
	_fgnup::calc_force_angle();
	return;
}


void _fgnup::calc_force
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{
	_beads::calc_force(bcs);
	_fgnup::calc_force_bond(bcs);
	_fgnup::calc_force_angle(bcs);
	return;
}


void _fgnup::calc_force
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_bcs 	&bcs,
	_beads	&beads_0
)
// --------------------------------------------------------------------
{
	_beads::calc_force( bcs, beads_0 );
	_fgnup::calc_force_bond(bcs);
	_fgnup::calc_force_angle(bcs);
	return;
}


// void _fgnup::calc_force
// // --------------------------------------------------------------------
// //
// // Function: 
// // 
// (
// 	_bcs 	&bcs,
// 	_beads 	&beads_0,
// 	_beads 	&beads_1
// )
// // --------------------------------------------------------------------
// {
// 	_beads::calc_force( bcs, beads_0, beads_1 );
// 	_fgnup::calc_force_bond();
// 	_fgnup::calc_force_angle();
// 	return;
// }


// void _fgnup::output
// // --------------------------------------------------------------------
// //
// // Function: 
// // 
// (
// 	const char 	*ofs_name
// )
// // --------------------------------------------------------------------
// {
// 	FILE *f_out;
// 	f_out = fopen( ofs_name, "w" );
// 	if( f_out == NULL ){
// 		std::cout << "\x1b[31m" << 
// 		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
// 		exit(1);
// 	}

// 	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
// 	fprintf( f_out, "fgnup\n" );
// 	fprintf( f_out, "ASCII\n" );
// 	fprintf( f_out, "DATASET POLYDATA\n" );
// 	fprintf( f_out, "POINTS %lu float\n", this->pos.size() );
// 	for( int i=0; i<this->pos.size(); i++ ){
// 		fprintf( f_out, "%lf %lf %lf\n", this->pos[i].x, this->pos[i].y, this->pos[i].z );
// 	}

// 	fprintf( f_out, "VERTICES %lu %lu\n", this->pos.size(), 2*(this->pos.size()) );
// 	for( int i=0; i<this->pos.size(); i++ ){
// 		fprintf( f_out, "1 %d\n", i );
// 	}
// 	fprintf( f_out, "LINES %lu %lu\n", this->bond.size(), 3*(this->bond.size()) );
// 	for( int i=0; i<this->bond.size(); i++ ){
// 		fprintf( f_out, "2 %d %d\n", bond[i].first, bond[i].second );
// 	}

// 	fprintf( f_out, "POINT_DATA %lu\n", this->pos.size() );
// 	fprintf( f_out, "SCALARS hyd float\n" );
// 	fprintf( f_out, "LOOKUP_TABLE default\n" );
// 	for( int i=0; i<this->pos.size(); i++ ){
// 		fprintf( f_out, "%lf\n", this->hyd[i] );
// 	}


// 	fclose(f_out);
// 	return;
// }



void _fgnup::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
)
// --------------------------------------------------------------------
{

	for( int i=0; i<pos.size(); i++ ){
		if( teth[i]== 1 ) continue; // tetehred bead does not move
		pos[i].x = pos[i].x + coeff_ex * frc[i].x + rand_std * f_unif(f_mt);
		pos[i].y = pos[i].y + coeff_ex * frc[i].y + rand_std * f_unif(f_mt);
		pos[i].z = pos[i].z + coeff_ex * frc[i].z + rand_std * f_unif(f_mt);
	}

	return;
}


void _fgnup::move
// --------------------------------------------------------------------
//
// Function: 
// 
(
	_bcs 	&bcs
)
// --------------------------------------------------------------------
{

	for( int i=0; i<pos.size(); i++ ){
		if( teth[i]== 1 ) continue; // tetehred bead does not move
		pos[i].x = pos[i].x + coeff_ex * frc[i].x + rand_std * f_unif(f_mt); 
		pos[i].y = pos[i].y + coeff_ex * frc[i].y + rand_std * f_unif(f_mt); 
		pos[i].z = pos[i].z + coeff_ex * frc[i].z + rand_std * f_unif(f_mt); 
	}

	bcs.move_at_boundary(this);

	return;
}


void _fgnup::output_asc
// --------------------------------------------------------------------
//
// Function: VTK output by ascii mode
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s.vtk", ofs_name_in );

	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "fgnup\n" );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET POLYDATA\n" );
	fprintf( f_out, "POINTS %lu float\n", this->pos.size() );
	for( int i=0; i<this->pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", this->pos[i].x, this->pos[i].y, this->pos[i].z );
	}

	int count = 0;
	fprintf( f_out, "LINES %d %lu\n", NUP_SIZE, pos.size()+NUP_SIZE );
	for( int i=0; i<NUP_SIZE; i++ ){
		fprintf( f_out, "%lu ", pos.size()/NUP_SIZE );
		for( int j=0; j<pos.size()/NUP_SIZE; j++ ){
			fprintf( f_out, "%d ", count );
			count ++;
		}
		fprintf( f_out, "\n");
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
			fprintf( f_out, "%d\n", pair_fg[hyd_inv_id[i]] );
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


void _fgnup::output_bin
// --------------------------------------------------------------------
//
// Function: VTK output by binary mode
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	char ofs_name[128];
	sprintf( ofs_name, "%s.vtk", ofs_name_in );

	std::ofstream f_out;
	f_out.open(ofs_name, std::ios::binary);

	if(!f_out){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	f_out << "# vtk DataFile Version 2.0" 			<<std::endl;
	f_out << "fgnup"								<<std::endl;
	f_out << "BINARY" 								<<std::endl;
	f_out << "DATASET POLYDATA" 				 	<<std::endl;
	f_out << "POINTS " << pos.size() << " float" 	<<std::endl;

	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = pos[i].x; SwapEnd(tmp_x);
		float tmp_y = pos[i].y; SwapEnd(tmp_y);
		float tmp_z = pos[i].z; SwapEnd(tmp_z);
		f_out.write((char*)&tmp_x, sizeof(float));
		f_out.write((char*)&tmp_y, sizeof(float));
		f_out.write((char*)&tmp_z, sizeof(float));
	}

	f_out << "LINES " << NUP_SIZE << " " << pos.size() + NUP_SIZE <<std::endl;
	int count = 0;
	for( int i=0; i<NUP_SIZE; i++ ){
		int len = pos.size()/NUP_SIZE; SwapEnd(len);
		f_out.write((char*)&len, sizeof(int));
		for( int j=0; j<pos.size()/NUP_SIZE; j++ ){
			int tmp_i = count; SwapEnd(tmp_i);
			f_out.write((char*)&tmp_i, sizeof(int));
			count ++;
		}
	}

	f_out << "POINT_DATA " << pos.size() 	<< std::endl;
	f_out << "SCALARS hyd float" 			<< std::endl;
	f_out << "LOOKUP_TABLE default"			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			float tmp = 0.0;
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(float));
		}else{
			float tmp = hyd[hyd_inv_id[i]];
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(float));
		}
	}

	f_out << "SCALARS state int" 			<< std::endl;
	f_out << "LOOKUP_TABLE default"			<< std::endl;

	for( int i=0; i<pos.size(); i++ ){
		if( hyd_inv_id[i] == -1){
			int tmp = -1;
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(int));
		}else{
			int tmp = pair_fg[hyd_inv_id[i]];
			SwapEnd(tmp);
			f_out.write((char*)&tmp, sizeof(int));
		}
	}

	/* force output is in the unit of pN */
	f_out << "VECTORS frc float" 			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = frc[i].x *FRC_PN; SwapEnd(tmp_x);
		float tmp_y = frc[i].y *FRC_PN; SwapEnd(tmp_y);
		float tmp_z = frc[i].z *FRC_PN; SwapEnd(tmp_z);
		f_out.write((char*)&tmp_x, sizeof(float));
		f_out.write((char*)&tmp_y, sizeof(float));
		f_out.write((char*)&tmp_z, sizeof(float));
	}

	return;
}


void _fgnup::output
// --------------------------------------------------------------------
//
// Function: VTK output (default set with binary mode)
// 
(
	const char 	*ofs_name_in
)
// --------------------------------------------------------------------
{
	_fgnup::output_asc(ofs_name_in);
	// _fgnup::output_bin(ofs_name_in);
	return;
}

