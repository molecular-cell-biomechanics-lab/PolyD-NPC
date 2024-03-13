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



void read_asc( char *fname )
{
	int 	N_bead, N_pol, bd_per_pol;
	double 	x, y, z;
	int 	dum, id_0, id_1, id_2, tmp_i;
	double 	val;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, fname );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for( int i=0; i<4; i++ ) std::getline(ifs, str); // skip 4 line
	std::getline(ifs, str); std::sscanf( str.data(), "POINTS %d float", &N_bead );
	
	for( int i=0; i<N_bead; i++ ){
		std::getline(ifs, str); std::sscanf( str.data(), "%lf\t%lf\t%lf", &x, &y, &z );
		pos.push_back( _vec<double>(x, y, z) );
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
	// 		bond.push_back(std::pair <int,int>(id_1,id_2));
	// 		id_0 = id_1;
	// 		id_1 = id_2;
	// 	}
	// }

	// std::getline(ifs, str);  // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "LINES %d %d", &N_pol, &dum );
	for( int i=0; i<N_pol; i++ ) std::getline(ifs, str); // skip N_pol lines

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

	return;
}

void output_bin(char *fname)
{

	char ofs_name[64];
	sprintf( ofs_name, "%s/%s", PATH_OUTPUT, fname );

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

	//***** Need conversion *************
	f_out << "VECTORS frc float" 			<< std::endl;
	for( int i=0; i<pos.size(); i++ ){
		float tmp_x = frc[i].x; SwapEnd(tmp_x);
		float tmp_y = frc[i].y; SwapEnd(tmp_y);
		float tmp_z = frc[i].z; SwapEnd(tmp_z);
		f_out.write((char*)&tmp_x, sizeof(float));
		f_out.write((char*)&tmp_y, sizeof(float));
		f_out.write((char*)&tmp_z, sizeof(float));
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
	output_bin(argv[1]);

	return 0;
}
