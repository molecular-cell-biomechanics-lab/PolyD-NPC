#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <algorithm>

// EXECUTE:: ./a.out

int  	pore;
int  	leng;
double  rcar;
int  	phyd;
int  	kuhn;
char  	k_on[16];
char  	k_of[16];
double 	temp;

int 	r_int, r_dec;
int 	temp_int;

std::vector<std::vector<int> > i_num;
std::vector<int> o_seed;
std::vector<int> o_count;

#define SEED_MAX 	100
#define INP_MAX 	100
#define INP_MIN 	 10
#define PATH  "/expanse/lustre/projects/ucb124/atsushim/bd_v3.4.2/data"


void convertToIntegers(double input, int &intPart, int &decimalPart) {
	intPart = (int)input;
	decimalPart = (input - intPart) * 10;
}


void read_para()
{
	char ifs_name[64];
	sprintf( ifs_name, "para.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line
	std::getline(ifs, str); std::sscanf(str.data(), "pore = %d" , &pore	);
	std::getline(ifs, str); std::sscanf(str.data(), "leng = %d" , &leng	);
	std::getline(ifs, str); std::sscanf(str.data(), "rcar = %lf", &rcar	);
	std::getline(ifs, str); std::sscanf(str.data(), "phyd = %d" , &phyd	);
	std::getline(ifs, str); std::sscanf(str.data(), "kuhn = %d" , &kuhn	);
	std::getline(ifs, str); std::sscanf(str.data(), "k_on = %s" , k_on	);
	std::getline(ifs, str); std::sscanf(str.data(), "k_of = %s" , k_of 	);
	std::getline(ifs, str); std::sscanf(str.data(), "temp = %lf", &temp	);
}



void input()
{
	char ifs_name[256];
	sprintf( ifs_name, 
		"%s/temp_%04d/pore_d%02d/leng_%03d/kuhn_%d/kon_%s/kof_%s/cargo_r%02d_%d/per_%02d/basin/list.txt", 
		PATH, temp_int, pore, leng, kuhn, k_on, k_of, r_int, r_dec, phyd );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	int count = 0;
	while(std::getline(ifs, str)){
		int val1, val2, dum1, dum2;
		// std::sscanf( str.data(), "cg_s%03d_%03d.vtk", &val1, &val2 );
		// std::sscanf( str.data(), "cg_s%03d_%03d_ctr.vtk", &dum1, &dum2 );
		std::sscanf( str.data(), "cg_s%03d_%03d_vtx.vtk", &val1, &val2 );
		// std::cout << dum1 << " " << val1 << " " << dum2 << " " << val2	 << std::endl;
		i_num[val2].push_back(val1);
		count ++;
	}

	// if( count < SEED_MAX ){
	if( count < INP_MIN ){
		std::cout << "\x1b[31m"	<< "ERROR:: " << 
		"input file is less than " << SEED_MAX
		<< "\x1b[39m" << std::endl;
        exit(1);
	}

	return;
}

void shuffle()
{
	for( int i=0; i<INP_MAX; i++ ){
		std::random_shuffle(i_num[i].begin(), i_num[i].end());
		// std::shuffle(i_num[i].begin(), i_num[i].end());
		// for( int j=0; j<i_num[i].size(); j++ ){
		// 	std::cout << i << " " << i_num[i][j] << std::endl;
		// }
		// std::cout << i << " " << i_num[i].size() << std::endl;
	}
	return;
}

void assign()
{
	int count = 0;
	int round = 0;
	while(count < SEED_MAX){
		for( int i=0; i<i_num[round].size(); i++ ){
			o_seed [count] = i_num[round][i];
			o_count[count] = round;
			count ++;
			if( count >= SEED_MAX ) break;
		}
		round ++;
		if( round >= i_num.size() ){
			round = 0;
			// std::cout << round << " " << i_num.size() << std::endl;
		}
	}
	return;
}

void output()
{
	
	FILE *f_out;
	char ofs_name[256];
	sprintf( ofs_name, 
		"%s/temp_%04d/pore_d%02d/leng_%03d/kuhn_%d/kon_%s/kof_%s/cargo_r%02d_%d/per_%02d/basin/input.txt", 
		PATH, temp_int, pore, leng, kuhn, k_on, k_of, r_int, r_dec, phyd );
	f_out = fopen( ofs_name, "w" );
	if( f_out == NULL ){
		std::cout << "\x1b[31m" << 
		"ERROR: cannot open " << ofs_name <<"\x1b[349" << std::endl;
		exit(1);
	}

	for( int i=0; i<SEED_MAX; i++ ){
		// fprintf( f_out, "%d\t%d\t%d\n", i, o_seed[i], o_count[i] );
		fprintf( f_out, "s%03d_%03d\n", o_seed[i], o_count[i] );
	}

	fclose(f_out);
	return;
}

// void check_cmd_line(int argc)
// {
//     if( argc != 6 ){
// 		std::cout << "\x1b[31m"	<< "ERROR:: " << 
// 		"./a.out {rcar} {phyd} {kuhn} {k_on} {k_of}"
// 		<< "\x1b[39m" << std::endl;
//         exit(1);
//     }
//     return;
// }


int main(int argc, char const *argv[])
{
	// check_cmd_line(argc);
	// rcar = atoi(argv[1]);
	// phyd = atoi(argv[2]);
	// kuhn = atoi(argv[3]);
	// k_on = argv[4];
	// k_of = argv[5];

	read_para();

	std::cout << "pore : " << pore << std::endl;
	std::cout << "leng : " << leng << std::endl;
	std::cout << "rcar : " << rcar << std::endl;
	std::cout << "phyd : " << phyd << std::endl;
	std::cout << "kuhn : " << kuhn << std::endl;
	std::cout << "k_on : " << k_on << std::endl;
	std::cout << "k_of : " << k_of << std::endl;
	std::cout << "temp : " << temp << std::endl;

	int dum;
	convertToIntegers(rcar, r_int, r_dec);
	convertToIntegers(temp, temp_int, dum);


	for( int i=0; i<SEED_MAX; i++ ){
		o_seed. push_back(-1);
		o_count.push_back(-1);
	}

	for( int i=0; i<INP_MAX; i++ ){
		std::vector<int> tmp;
		i_num.push_back(tmp);
	}

	input();
	shuffle();
	assign();
	output();

	return 0;
}