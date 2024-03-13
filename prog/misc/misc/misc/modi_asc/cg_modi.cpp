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

std::vector<_vec<double> > pos;
std::vector<_vec<double> > frc;
std::vector<double>	hyd;
std::vector<int>	hyd_id;
std::vector<int>	hyd_inv_id;
_vec<double> 						center;
std::vector<std::tuple<int,int,int> > face;	
std::vector<int> pair_fg;


// https://stackoverflow.com/questions/105252
template <typename T>
void SwapEnd(T& var)
{
  char* varArray = reinterpret_cast<char*>(&var);
  for(long i = 0; i < static_cast<long>(sizeof(var)/2); i++)
    std::swap(varArray[sizeof(var) - 1 - i],varArray[i]);
}


void read_asc( char *fname )
{
	int 	N_bead, N_face;
	double 	x, y, z;
	double 	val;
	int 	dum;
	int 	a, b, c;
	int 	tmp_i;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, fname );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	// for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "%lf %lf %lf", &x, &y, &z );
	center = _vec<double>(x, y, z);

	
	for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		_vec<double> tmp(x, y, z);
		pos.push_back( tmp );
		_vec<double> tmp_f(0.0, 0.0, 0.0);
		frc.push_back(tmp_f);
	}

	std::getline(ifs, str);
	for( int i=0; i<N_bead; i++ ) std::getline(ifs, str); // skip vertices

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
			pair_fg.push_back(-1); // NEW
			count ++;
		}else{
			hyd_inv_id.push_back(-1);
		}
	}

	// for( int i=0; i<2; i++ ) std::getline(ifs, str); // skip 2 lines
	// for( int i=0; i<N_bead; i++ ){
	// 	std::getline(ifs, str); std::sscanf( str.data(), "%d", &tmp_i );
	// 	if( hyd_inv_id[i] != -1 ){
	// 		pair_fg.push_back(tmp_i);
	// 	}
	// }

	// for( int i=0; i<1; i++ ) std::getline(ifs, str); // skip 1 lines
	// for( int i=0; i<N_bead; i++ ){
	// 	std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
	// 	_vec<double> tmp(x, y, z);
	// 	frc.push_back(tmp);
	// }

	return;
}




void output_asc( char *fname )
{
	char ofs_name[64];
	sprintf( ofs_name, "%s/asc_%s", PATH_OUTPUT, fname );
	FILE *f_out;
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	fprintf( f_out, "# vtk DataFile Version 2.0\n" );
	fprintf( f_out, "%lf %lf %lf\n", center.x, center.y, center.z );
	// fprintf( f_out, "decoded ascii\n" );
	fprintf( f_out, "ASCII\n" );
	fprintf( f_out, "DATASET POLYDATA\n" );
	fprintf( f_out, "POINTS %lu float\n", pos.size() );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", pos[i].x, pos[i].y, pos[i].z );
	}

	int count = 0;
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
			fprintf( f_out, "%d\n", pair_fg[hyd_inv_id[i]] );
		}
	}
	
	fprintf( f_out, "VECTORS frc float\n" );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf\n", frc[i].x, frc[i].y, frc[i].z);
	}

	fclose(f_out);
	return;
}


void output_bin( char *fname )
{
	char ofs_name[64];
	sprintf( ofs_name, "%s/%s", PATH_OUTPUT, fname );

	std::ofstream fout;
	fout.open(ofs_name, std::ios::binary);

	if(!fout){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

    fout << "# vtk DataFile Version 2.0" 	<<std::endl;
    // fout << "Exemple"						<<std::endl;
    fout << center.x << " " << center.y << " " << center.z	<<std::endl;
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
			int tmp = pair_fg[hyd_inv_id[i]];
			SwapEnd(tmp);
			fout.write((char*)&tmp, sizeof(int));
		}
	}

	//***** Need conversion *************
	fout << "VECTORS frc float" 			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = frc[i].x; SwapEnd(tmp_x);
		float tmp_y = frc[i].y; SwapEnd(tmp_y);
		float tmp_z = frc[i].z; SwapEnd(tmp_z);
		fout.write((char*)&tmp_x, sizeof(float));
		fout.write((char*)&tmp_y, sizeof(float));
		fout.write((char*)&tmp_z, sizeof(float));
	}

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
	read_asc(argv[1]);
	// output_asc(argv[1]);
	output_bin(argv[1]);

	return 0;
}
