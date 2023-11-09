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
std::vector<int>		  bond;


// https://stackoverflow.com/questions/105252
template <typename T>
void SwapEnd(T& var)
{
  char* varArray = reinterpret_cast<char*>(&var);
  for(long i = 0; i < static_cast<long>(sizeof(var)/2); i++)
    std::swap(varArray[sizeof(var) - 1 - i],varArray[i]);
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

	fprintf( f_out, "%lu\n", pos.size() );
	for( int i=0; i<pos.size(); i++ ){
		fprintf( f_out, "%lf %lf %lf %d\n", pos[i].x, pos[i].y, pos[i].z, bond[i] );
	}

	fclose(f_out);
	return;
}


void read_bin( char *fname )
{
	int 	N_step;
	double 	tmp_x, tmp_y, tmp_z;
	int 	tmp_i;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", PATH_INPUT, fname );
	std::ifstream ifs(ifs_name, std::ios::binary);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); std::sscanf( str.data(), "%d", &N_step );

	for( int i=0; i<N_step; i++ ){
		ifs.read((char *) &tmp_x, sizeof(double));
		ifs.read((char *) &tmp_y, sizeof(double));
		ifs.read((char *) &tmp_z, sizeof(double));
		ifs.read((char *) &tmp_i, sizeof(int   ));
		pos.push_back(_vec<double>(tmp_x, tmp_y, tmp_z));
		bond.push_back(tmp_i);
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
	read_bin(argv[1]);
	output_asc(argv[1]);

	return 0;
}
