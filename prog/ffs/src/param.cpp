#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "sim.h"
#include "param.h"

// const static char 	path_input[] 	= "./src/input";	// path to the input directory

#ifdef DEBUG
	const static char path_input[] = "../ffs/src/input/debug";	// path to the input directory	
#else
	const static char path_input[] = "../ffs/src/input";		// path to the input directory
#endif

void _param::read_init
// --------------------------------------------------------------------
//
// Purpose: read parameters from init.txt
// 
(
)
// --------------------------------------------------------------------
{

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", path_input, "init.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }


	std::getline(ifs, str); //skip 1 line
	std::getline(ifs, str); std::sscanf(str.data(), "strt_div = %d", &strt_div );

	return;
}

void _param::read_divs
// --------------------------------------------------------------------
//
// Purpose: read parameters from divs.txt
// 
(
)
// --------------------------------------------------------------------
{
	double val;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", path_input, "divs.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line
	while( std::getline(ifs, str) ){
		std::sscanf(str.data(), "%lf", &val );
		x_div.push_back(val);
	}
	n_divs = x_div.size() - 1;
	return;
}


void _param::read_kbra
// --------------------------------------------------------------------
//
// Purpose: read parameters from kbra.txt
// 
(
)
// --------------------------------------------------------------------
{
	int val;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", path_input, "kbra.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line
	while( std::getline(ifs, str) ){
		std::sscanf(str.data(), "%d", &val );
		k_bra.push_back(val);
	}
	if( n_divs != k_bra.size() ){
		std::cout << "k_bra length does not match n_divs" << std::endl;
		std::cout << "k_bra length	: " << k_bra.size()-1 << std::endl;
		std::cout << "n_divs 		: " << n_divs 		  << std::endl;
		exit(1);
	}
	return;
}

void _param::read_pend
// --------------------------------------------------------------------
//
// Purpose: read parameters from pend.txt
// 
(
)
// --------------------------------------------------------------------
{
	double val;

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", path_input, "pend.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }

	std::getline(ifs, str); //skip 1 line
	while( std::getline(ifs, str) ){
		std::sscanf(str.data(), "%lf", &val );
		p_end.push_back(val);
	}
	if( n_divs != p_end.size() ){
		std::cout << "p_end length does not match n_divs" << std::endl;
		std::cout << "p_end length	: " << p_end.size()-1 << std::endl;
		std::cout << "n_divs 		: " << n_divs 		  << std::endl;
		exit(1);
	}
	return;
}



void _param::read_input
// --------------------------------------------------------------------
//
// Purpose: read parameters from input files
// 
(
)
// --------------------------------------------------------------------
{
	read_init();
	read_divs();
	read_kbra();
	read_pend();
	return;
}


void _param::display_input
// --------------------------------------------------------------------
//
// Purpose: read parameters from input files
// 
(
)
// --------------------------------------------------------------------
{

    std::cout << "\x1b[34m"	 << std::endl;
	std::cout << "strt_div = " << strt_div << std::endl;

	std::cout << "x_div  = (";
    for( int i=0; i<x_div.size()-1; i++ ){
    	std::cout << x_div[i] << ", ";
    }
    std::cout << x_div.back() << ")" << std::endl;

   	std::cout << "k_bra  = (";
    for( int i=0; i<k_bra.size()-1; i++ ){
    	std::cout << k_bra[i] << ", ";
    }
    std::cout << k_bra.back() << ")" << std::endl;

   	std::cout << "p_end  = (";
    for( int i=0; i<p_end.size()-1; i++ ){
    	std::cout << p_end[i] << ", ";
    }
    std::cout << p_end.back() << ")" << std::endl;

    std::cout << "\x1b[39m"	<< std::endl;
    std::cout << std::endl;
    return;

}


void _param::set_para
// --------------------------------------------------------------------
//
// Function: read input parameters and setup initial conditions
// 
(
)
// --------------------------------------------------------------------
{

	read_input	 ();
	display_input();

	// std::cout << "_param::set_para finished" << std::endl;

	return;	
}


_param::_param
// --------------------------------------------------------------------
//
// Constructor: 
// 
(
)
// --------------------------------------------------------------------
{
	set_para();
}



_param::~_param
// --------------------------------------------------------------------
//
// Deconstructor: 
// 
(
)
// --------------------------------------------------------------------
{
}


