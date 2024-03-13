#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <tuple>
#include "vec.h"

// run:: ./a.out [file name]

#define PATH_INPUT	"input"
#define PATH_OUTPUT "output"

#define NUP_SIZE 	80

std::vector<_vec<double> > pos;
std::vector<std::pair <int,int> > bond;
std::vector<double>	hyd;
std::vector<int>	hyd_id;
std::vector<int>	hyd_inv_id;
std::vector<_vec<double> > frc;
std::vector<int> pair_fg;


// https://stackoverflow.com/questions/105252
template <typename T>
void SwapEnd(T& var)
{
  char* varArray = reinterpret_cast<char*>(&var);
  for(long i = 0; i < static_cast<long>(sizeof(var)/2); i++)
    std::swap(varArray[sizeof(var) - 1 - i],varArray[i]);
}


void read_bin( char *fname )
{
	int 	N_bead, N_pol, bd_per_pol;
	double 	x, y, z;
	int 	dum, id_0, id_1, id_2, tmp_i;
	double 	val;
	float	tmp_x, tmp_y, tmp_z, tmp;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, fname );
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

	std::getline(ifs, str); std::sscanf( str.data(), "LINES %d %d", &N_pol, &dum );
	for( int i=0; i<N_pol; i++ ){
		ifs.read((char *) &bd_per_pol, 	sizeof(int)); SwapEnd(bd_per_pol);
		ifs.read((char *) &id_0, 		sizeof(int)); SwapEnd(id_0);

		for( int i=0; i<bd_per_pol-1; i++ ){
			ifs.read((char *) &id_1, sizeof(int)); SwapEnd(id_1);
			bond.push_back( std::pair<int,int>(id_0,id_1) );
			id_0 = id_1;
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
		frc.push_back(tmp_f);
	}

	return;
}



void output_asc( char *fname )
{
	char ofs_name[64];
	sprintf( ofs_name, "%s/%s", PATH_OUTPUT, fname );
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
	fprintf( f_out, "POINTS %lu float\n", pos.size() );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", pos[i].x, pos[i].y, pos[i].z );
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

	fprintf( f_out, "VECTORS frc float\n" );
	// for( int i=0; i<pos.size(); i++ ){
	// 	if( hyd_inv_id[i] == -1){
	// 		fprintf( f_out, "%lf %lf %lf\n", 0., 1., 0.);
	// 	}else{
	// 		fprintf( f_out, "%lf %lf %lf\n", 0., 0., 1.);
	// 	}
	// }
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", frc[i].x, frc[i].y, frc[i].z);
	}

	fclose(f_out);
	return;
}

void check_cmd_line(int argc)
{
    if( argc != 2 ){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"Specify the file name"
		<< "\x1b[39m" << std::endl;
        exit(1);
    }
    return;
}


int main(int argc, char *argv[]){

	check_cmd_line(argc);
	read_bin(argv[1]);
	output_asc(argv[1]);

	return 0;
}
