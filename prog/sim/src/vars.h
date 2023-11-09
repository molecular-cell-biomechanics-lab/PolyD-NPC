#ifndef VARS_H
#define VARS_H

/* ____ unit ______

length		: nm
time		: ns
mass		: g
temperature	: K
___________________ */


// Macro definition ===========================================================================

#define D_RANDSEED 	1 				// seed for random mumber generation
#define D_KBT0		4.114e-18		// default KBT (Boltzmann const * temperature)
#define D_PI 		3.14159265359 	// pi

/* memo
k_BT0	= 4.114 				{pN nm}
		= 4.114e-12				{kg m  / s2 nm}
		= 4.114e-12	* 1e-6		( g nm /ns2 nm) = (g nm2 /ns2)
*/


// Extern declaration =========================================================================

// extern const double k_Bolz; 	// Boltzmann constant
extern 		 double cof_ex;		// coeff for the external force, D*dt*V_higt (kBT cancels out)
extern 		 double rnd_sd;		// deviation of the rnd force, sqrt(3) * sqrt(2*D*dt)


// Type definition ============================================================================

struct _param
{
	/* basic 		*/
	double 	l_segm 	;		// segment length of FG-Nups 						(nm)
	double 	t_delt 	; 		// simulation time step 							(ns)

	/* diffusion 	*/
	double 	T_body	; 		// temperature 										(K)
	double  h_visc 	; 		// viscosity of water in room temperature 			{cP}={mPa s}
	double  r_stks 	; 		// Stokes radius of each bead 			  			(nm)
	double  D_diff 	; 		// Diffusion coefficient 	 			  			(nm2 /ns)
	
	/* repulsion 	*/
	double 	eps_rp	;		// height of repulsion potential 					{kBT_0}
	double 	sgm_rp	;		// characteristic length of repulsion potential 	{segm}
	double 	cut_rp	;		// cutoff distance of repulsion potential 			(nm)

	/* hydrophobic  */
	double 	eps_fg	;		// interfacial energy of hydrophobic attraction 	{kBT_0}
	double 	sgm_fg	;		// characteristic length of hydrophobic attraction 	{segm}
	double 	cut_fg	;		// cutoff distance of hydrophobic attraction 		{segm}

	/* tension		 */
	double 	k_tens	;		// spring constant for tensil force 				{kBT_0/segm^2}
	double 	r_tens	;		// natural length between neighboring beads			(nm)

	/* bending 		*/
	double 	l_kuhn 	;		// Kuhn length of FG-Nups 							{segm}
	double 	k_bend	;		// spring constant for bending 						(g nm2 /ns2)
};

struct _time
{
};


struct _ffs
{
};


// Prototype declaration ======================================================================

void vars_init
// Purpose	: read input parameters and setup initial conditions
// File		: vars.cpp
(
   	_param 		*param
   	// _time 	*time,
   	// _ffs 	*ffs
);

#endif