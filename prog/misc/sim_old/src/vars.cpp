#include <stdio.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include "vars.h"


const char 		path_input[] 	= "src/input";		// path to the input directory
const double 	k_Boltz			= 1.380649e-11; 	// Boltzmann constant (nm2 g /ns2 /K)

/* memo

k_Boltz = 1.380649e-23 			{ m^2 kg /  s^2 / K}
		= 1.380649e-23 * 1e12 	(nm^2 g  / ns^2 / K)

*/


void read_para
// ====================================================================
//
// Purpose: read parameters from parameter.txt
// 
(
   	_param 	*param
)
// --------------------------------------------------------------------
{

	char ifs_name[64];
	sprintf( ifs_name, "%s/%s", path_input, "parameter.txt" );
	std::ifstream ifs(ifs_name);
	std::string str;

	if(ifs.fail()){ std::cerr << ifs_name << " do not exist\n"; exit(0); }


	std::getline(ifs, str); // skip 1 line
	std::getline(ifs, str); std::sscanf( str.data(), "l_segm = %lf", &(param->l_segm) );
	std::getline(ifs, str); std::sscanf( str.data(), "t_delt = %lf", &(param->t_delt) );
	std::getline(ifs, str); std::sscanf( str.data(), "T_body = %lf", &(param->T_body) );
	std::getline(ifs, str); std::sscanf( str.data(), "h_visc = %lf", &(param->h_visc) );
	std::getline(ifs, str); // r_stks
	std::getline(ifs, str); // D_diff
	std::getline(ifs, str); std::sscanf( str.data(), "eps_rp = %lf", &(param->eps_rp) );
	std::getline(ifs, str); std::sscanf( str.data(), "sgm_rp = %lf", &(param->sgm_rp) );
	std::getline(ifs, str); // cut_rp
	std::getline(ifs, str); std::sscanf( str.data(), "eps_fg = %lf", &(param->eps_fg) );
	std::getline(ifs, str); std::sscanf( str.data(), "sgm_fg = %lf", &(param->sgm_fg) );
	std::getline(ifs, str); std::sscanf( str.data(), "cut_fg = %lf", &(param->cut_fg) );
	std::getline(ifs, str); std::sscanf( str.data(), "k_tens = %lf", &(param->k_tens) );
	std::getline(ifs, str); // r_tens
	std::getline(ifs, str); std::sscanf( str.data(), "l_kuhn = %lf", &(param->l_kuhn) );
	std::getline(ifs, str); // k_bend

	return;
}


void read_input
// ====================================================================
//
// Purpose: read parameters from input files
// 
(
	_param 	*param
)
// --------------------------------------------------------------------
{
	read_para( param );
	return;
}


void calc_para
// ====================================================================
//
// Purpose: unit conversion and calcultion of parameters in _param
// 
(
   	_param 	*param
)
// --------------------------------------------------------------------
{

	// unit conversion
	param->h_visc = param->h_visc * 1e-18			;	// {mPa s} = {g /m /s} -> (g /nm /ns)
	param->eps_rp = param->eps_rp * D_KBT0			;	// {kBT_0} -> (nm2 g /ns2)
	param->sgm_rp = param->sgm_rp * param->l_segm	;	// {segm}  -> (nm)
	param->eps_fg = param->eps_fg * D_KBT0			;	// {kBT_0} -> (nm2 g /ns2)
	param->sgm_fg = param->sgm_fg * param->l_segm	;	// {segm}  -> (nm)
	param->cut_fg = param->cut_fg * param->l_segm	;	// {segm}  -> (nm)
	param->k_tens = param->k_tens * D_KBT0 / param->l_segm / param->l_segm;	// {kBT_0/segm^2} -> (g /ns2)
	param->l_kuhn = param->l_kuhn * param->l_segm	;	// {segm}  -> (nm)


	// calculation of parameters in _param
	param->r_stks = param->l_segm / 2;								// (nm)
	param->D_diff = k_Boltz * param->T_body / 6 / D_PI / param->h_visc / param->r_stks; // (nm2 / ns)
	param->cut_rp = param->l_segm; 									// (nm)
	param->r_tens = param->l_segm;									// (nm)
	param->k_bend = D_KBT0 * 0.5 * param->l_kuhn / param->l_segm;	// (g nm2 /ns2)


	return;
}


void calc_extn
// ====================================================================
//
// Purpose: calcultion of parameters in external variables
// 
(
   	_param 	*param
)
// --------------------------------------------------------------------
{

	// TODO: coefficient for external force
	// TODO: coefficient for random force 

	// cof_ex = param->D_diff * param->t_delt / k_BolT;
	// cof_ex = param->D_diff * param->t_delt * param->V_higt;
	// x_c    = param->L_pore /2;
	// sigma  = x_c /2;
	// sigma2 = sigma * sigma;
	// rnd_sd = sqrt(3) * sqrt( 2 * param->D_diff * param->t_delt );

	return;
}


void calc_input
// ====================================================================
//
// Purpose: calculate default parameters
// 
(
	_param 	*param
)
// --------------------------------------------------------------------
{
	calc_para( param );
	calc_extn( param );
	return;
}


void display_input
// ====================================================================
//
// Purpose: read parameters from input files
// 
(
	_param 	*param
)
// --------------------------------------------------------------------
{

    std::cout << "\x1b[34m"	 << std::endl;

	std::cout << "l_segm = " << param->l_segm << std::endl;
	std::cout << "t_delt = " << param->t_delt << std::endl;

	std::cout << "T_body = " << param->T_body << std::endl;
	std::cout << "h_visc = " << param->h_visc << std::endl;
	std::cout << "r_stks = " << param->r_stks << std::endl;
	std::cout << "D_diff = " << param->D_diff << std::endl;
	
	std::cout << "eps_rp = " << param->eps_rp << std::endl;
	std::cout << "sgm_rp = " << param->sgm_rp << std::endl;
	std::cout << "cut_rp = " << param->cut_rp << std::endl;
	
	std::cout << "eps_fg = " << param->eps_fg << std::endl;
	std::cout << "sgm_fg = " << param->sgm_fg << std::endl;
	std::cout << "cut_fg = " << param->cut_fg << std::endl;
	
	std::cout << "k_tens = " << param->k_tens << std::endl;
	std::cout << "r_tens = " << param->r_tens << std::endl;
	
	std::cout << "l_kuhn = " << param->l_kuhn << std::endl;
	std::cout << "k_bend = " << param->k_bend << std::endl;
    
    std::cout << "\x1b[39m"	<< std::endl;
    return;

}


void vars_init
// ====================================================================
//
// Purpose: read input parameters and setup initial conditions
// 
(
   	_param 		*param
)
// --------------------------------------------------------------------
{

	read_input	  ( param );
	calc_input 	  ( param );
	display_input ( param );

	std::cout << "vars_init finished" << std::endl;

	return;
}