#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <sys/stat.h>

#define PATH_OUT "output"
#define PATH_REC "rec"

/*
change name of the temporal output files (e.g. fg_s000_i00_00000.vtk)
into sequential names (e.g. *****)
*/

std::vector<int> k_id;
std::vector<int> step;


void read_input(int seed)
{
	char ifs_name[64];
	sprintf( ifs_name, "%s/rate_s%03d.txt", PATH_OUT, seed );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line

	while(std::getline(ifs, str)){
		double	dum;
		int 	k,n;
		std::sscanf(str.data(), "%lf %lf %d %d", &dum, &dum, &k, &n);
		k_id.push_back(k);
		step.push_back(n);
	}

	k_id.pop_back();	// remove the last line
	step.pop_back();	// remove the last line

	return;
}


void read_prog(int seed, std::string dire)
{
	char ifs_name[64];
	sprintf( ifs_name, "%s/prog_s%03d.txt", dire.c_str(), seed );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	for(int i=0; i<2; i++) std::getline(ifs, str); //skip lines

	while(std::getline(ifs, str)){
		double	dum;
		int 	k,n;
		std::sscanf(str.data(), "%lf %lf %d %d", &dum, &dum, &k, &n);
		k_id.push_back(k);
		step.push_back(n);
	}

	return;
}

void rename_file(
	int seed, 	// seed
	int i,		// interface
	int k, 		// k_id
	int n, 		// step
	int count	// count
)
{
	char cg_c_old[64], cg_v_old[64], fg_v_old[64], cpos_old[64];
	char cg_c_new[64], cg_v_new[64], fg_v_new[64], cpos_new[64];

	sprintf( cg_c_old, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_ctr.vtk", PATH_OUT, seed, i, k, n );
	sprintf( cg_v_old, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_vtx.vtk", PATH_OUT, seed, i, k, n );
	sprintf( fg_v_old, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d.vtk"    , PATH_OUT, seed, i, k, n );
	sprintf( cpos_old, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt"    , PATH_OUT, seed, i, k, n );

	sprintf( cg_c_new, "%s/cg_s%03d_%05d_ctr.vtk", PATH_OUT, seed, count );
	sprintf( cg_v_new, "%s/cg_s%03d_%05d_vtx.vtk", PATH_OUT, seed, count );
	sprintf( fg_v_new, "%s/fg_s%03d_%05d.vtk"    , PATH_OUT, seed, count );
	sprintf( cpos_new, "%s/pt_s%03d_%05d.txt"    , PATH_OUT, seed, count );

	struct stat buff;
	if( stat(cg_c_old, &buff) != 0 ){ std::cerr << cg_c_old << " do not exist\n"; exit(0); }
	if( stat(cg_v_old, &buff) != 0 ){ std::cerr << cg_v_old << " do not exist\n"; exit(0); }
	if( stat(fg_v_old, &buff) != 0 ){ std::cerr << fg_v_old << " do not exist\n"; exit(0); }

	rename( cg_c_old, cg_c_new );
	rename( cg_v_old, cg_v_new );
	rename( fg_v_old, fg_v_new );

	if(i!=0){
		if( stat(cpos_old, &buff) != 0 ){ std::cerr << cpos_old << " do not exist\n"; exit(0); }
		rename( cpos_old, cpos_new );
	}

	return;
}

void move_file(
	int seed, 	// seed
	int i,		// interface
	int k, 		// k_id
	int n, 		// step
	int count	// count
)
{
	char cg_c_old[64], cg_v_old[64], fg_v_old[64], cpos_old[64];
	char cg_c_new[64], cg_v_new[64], fg_v_new[64], cpos_new[64];

	sprintf( cg_c_old, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_ctr.vtk", PATH_OUT, seed, i, k, n );
	sprintf( cg_v_old, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_vtx.vtk", PATH_OUT, seed, i, k, n );
	sprintf( fg_v_old, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d.vtk"    , PATH_OUT, seed, i, k, n );
	sprintf( cpos_old, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt"    , PATH_OUT, seed, i, k, n );

	sprintf( cg_c_new, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_ctr.vtk", PATH_REC, seed, i, k, n );
	sprintf( cg_v_new, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_vtx.vtk", PATH_REC, seed, i, k, n );
	sprintf( fg_v_new, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d.vtk"    , PATH_REC, seed, i, k, n );
	sprintf( cpos_new, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt"    , PATH_REC, seed, i, k, n );

	struct stat buff;
	if( stat(cg_c_old, &buff) != 0 ){ std::cerr << cg_c_old << " do not exist\n"; exit(0); }
	if( stat(cg_v_old, &buff) != 0 ){ std::cerr << cg_v_old << " do not exist\n"; exit(0); }
	if( stat(fg_v_old, &buff) != 0 ){ std::cerr << fg_v_old << " do not exist\n"; exit(0); }

	rename( cg_c_old, cg_c_new );
	rename( cg_v_old, cg_v_new );
	rename( fg_v_old, fg_v_new );

	if(i!=0){
		if( stat(cpos_old, &buff) != 0 ){ std::cerr << cpos_old << " do not exist\n"; exit(0); }
		rename( cpos_old, cpos_new );
	}

	return;
}


void copy_file(
	int seed, 	// seed
	int i,		// interface
	int k, 		// k_id
	int n, 		// step
	int count	// count
)
{
	char cg_c_old[64], cg_v_old[64], fg_v_old[64], cpos_old[64];
	char cg_c_new[64], cg_v_new[64], fg_v_new[64], cpos_new[64];

	sprintf( cg_c_old, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_ctr.vtk", PATH_REC, seed, i, k, n );
	sprintf( cg_v_old, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_vtx.vtk", PATH_REC, seed, i, k, n );
	sprintf( fg_v_old, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d.vtk"    , PATH_REC, seed, i, k, n );
	sprintf( cpos_old, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt"    , PATH_REC, seed, i, k, n );

	sprintf( cg_c_new, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_ctr.vtk", PATH_OUT, seed, i, k, n );
	sprintf( cg_v_new, "%s/tmp_cg_s%03d_i%02d_k%02d_%05d_vtx.vtk", PATH_OUT, seed, i, k, n );
	sprintf( fg_v_new, "%s/tmp_fg_s%03d_i%02d_k%02d_%05d.vtk"    , PATH_OUT, seed, i, k, n );
	sprintf( cpos_new, "%s/tmp_pt_s%03d_i%02d_k%02d_%05d.txt"    , PATH_OUT, seed, i, k, n );

	struct stat buff;
	if( stat(cg_c_old, &buff) != 0 ){ std::cerr << cg_c_old << " do not exist\n"; exit(0); }
	if( stat(cg_v_old, &buff) != 0 ){ std::cerr << cg_v_old << " do not exist\n"; exit(0); }
	if( stat(fg_v_old, &buff) != 0 ){ std::cerr << fg_v_old << " do not exist\n"; exit(0); }

	rename( cg_c_old, cg_c_new );
	rename( cg_v_old, cg_v_new );
	rename( fg_v_old, fg_v_new );

	if(i!=0){
		if( stat(cpos_old, &buff) != 0 ){ std::cerr << cpos_old << " do not exist\n"; exit(0); }
		rename( cpos_old, cpos_new );
	}

	return;
}

void connect(int seed)
{
	int count = 0;
	rename_file(seed, 0, 0, 0, count);
	count ++;

	// k_id = -1 (failed run) is automatically discarded here
	for( int i=0; i<k_id.size(); i++ ){
		for( int n=0; n<step[i]; n++ ){
			// std::cout << i << " " << k_id[i] << " " << n << std::endl;
			rename_file(seed, i+1, k_id[i], n, count);
			count ++;
		}
	}

	return;
}

void transfer(int seed)
{
	int count = 0;
	move_file(seed, 0, 0, 0, count);
	count ++;

	for( int i=0; i<k_id.size(); i++ ){
		for( int n=0; n<step[i]; n++ ){
			// std::cout << i << " " << k_id[i] << " " << n << std::endl;
			move_file(seed, i+1, k_id[i], n, count);
			count ++;
		}
	}

	return;
}

void copy(int seed)
{
	int count = 0;
	copy_file(seed, 0, 0, 0, count);
	count ++;

	for( int i=0; i<k_id.size(); i++ ){
		for( int n=0; n<step[i]; n++ ){
			// std::cout << i << " " << k_id[i] << " " << n << std::endl;
			copy_file(seed, i+1, k_id[i], n, count);
			count ++;
		}
	}

	return;
}

void check_cmd_line(int argc)
{
    if( argc != 2 ){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"Specify a random number seed"
		<< "\x1b[39m" << std::endl;
        exit(1);
    }
    return;
}

bool prog_exists( int seed, std::string dire )
{
	// simulation ended due to time limit
	char ifs_name[64];
	sprintf( ifs_name, "%s/prog_s%03d.txt", dire.c_str(), seed );
	std::ifstream ifs(ifs_name);
	std::string str;
	if(ifs.fail()){ return false; }
	return true;
}

int main(int argc, char *argv[]){

	check_cmd_line(argc);
	int seed = atoi(argv[1]);

	if(prog_exists(seed, PATH_OUT)){ // current simulation ended due to time limit
		if(prog_exists(seed, PATH_REC)){ // previous simulation ended due to time limit
			read_prog(seed, PATH_REC);
			copy(seed);
			k_id.clear();
			step.clear();
		}
		read_prog(seed, PATH_OUT);
		transfer(seed);
	}else{
		// simulation completed in the current run
		if(prog_exists(seed, PATH_REC)){ // previous simulation ended due to time limit
			read_prog(seed, PATH_REC);
			copy(seed);
			k_id.clear();
			step.clear();
		}
		read_input(seed);
		connect(seed);
	}

	return 0;
}


