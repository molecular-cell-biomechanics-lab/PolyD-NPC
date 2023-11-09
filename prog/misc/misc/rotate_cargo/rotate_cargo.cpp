#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <tuple>
#include "vec.h"


#define PATH_INPUT	"../../../../bcs/cargo"
#define PATH_OUTPUT "../../../../bcs/cargo"


std::vector<_vec<double> > 	pos;
std::vector<_vec<double> > 	frc;
std::vector<double>			hyd;
std::vector<int>			hyd_id;
std::vector<int>			hyd_inv_id;
_vec<double> 				center;
std::vector<std::tuple<int,int,int> > face;	
std::vector<int> pair_fg;

int rcar; 
int phyd;

// https://stackoverflow.com/questions/105252
template <typename T>
void SwapEnd(T& var)
{
  char* varArray = reinterpret_cast<char*>(&var);
  for(long i = 0; i < static_cast<long>(sizeof(var)/2); i++)
    std::swap(varArray[sizeof(var) - 1 - i],varArray[i]);
}


void input_para()
{
	char ifs_name[64];
	sprintf( ifs_name, "init.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "rcar = %d", &rcar );
	std::getline(ifs, str); std::sscanf( str.data(), "phyd = %d", &phyd );
}


void read_bin()
{
	int 	N_bead, N_face;
	double 	x, y, z;
	double 	val;
	int 	dum;
	float 	tmp_x, tmp_y, tmp_z, tmp;
	int 	face_0, face_1, face_2, tmp_i;


	char ifs_name[64];
	sprintf( ifs_name, "%s/cargo_r%02dp%02d.vtk", PATH_INPUT, rcar, phyd );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str);
	std::getline(ifs, str); std::sscanf( str.data(), "%lf %lf %lf", &x, &y, &z );
	center = _vec<double> (x, y, z);
	std::getline(ifs, str);
	std::getline(ifs, str);
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


void rotate()
// http://www.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/tech0007.html
{
	for( int i=0; i<pos.size(); i++ ){
		_vec<double> ori_prev = pos[i] - center;
		_vec<double> ori_next; 
		ori_next.x = ori_prev.x;
		ori_next.y = -ori_prev.y;
		ori_next.z = -ori_prev.z;
		pos[i] = center + ori_next;
	}
	return;
}


void output_bin()
{
	char ofs_name[64];
	sprintf( ofs_name, "%s/inverted/cargo_r%02dp%02d.vtk", PATH_OUTPUT, rcar, phyd );

	std::ofstream fout;
	fout.open(ofs_name, std::ios::binary);

	if(!fout){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

    fout << "# vtk DataFile Version 2.0" 					<<std::endl;
    fout << center.x << " " << center.y << " " << center.z  <<std::endl;
    fout << "BINARY" 										<<std::endl;
    fout << "DATASET POLYDATA" 								<<std::endl;
    fout << "POINTS " << pos.size() << " float" 			<<std::endl;

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

// void check_cmd_line(int argc)
// {
//     if( argc != 2 ){
// 		std::cout << "\x1b[31m"	<< "ERROR:: " << 
// 		"Specify the file name"
// 		<< "\x1b[39m" << std::endl;
//         exit(1);
//     }
//     return;
// }


int main(int argc, char *argv[]){

	// check_cmd_line(argc);
	input_para();
	read_bin();
	rotate();
	output_bin();

	return 0;
}
