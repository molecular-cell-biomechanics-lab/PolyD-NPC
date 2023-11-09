#ifndef ENV_H
#define ENV_H

/* ____ unit ______

length		: nm
time		: ns
mass		: g
temperature	: K
___________________ */


// Macro definition ===========================================================================

#define RANDSEED 	1 				// seed for random mumber (it will be changed at constructor)
#define KBT_0		4.114e-18		// default KBT (Boltzmann const * temperature)
#define K_BOLTZ 	1.380649e-20 	// Boltzmann constant (nm2 g /ns2 /K)
#define PI 			3.14159265359 	// pi
#define PATH_INPUT	"../../sim/src/input"		// path to the input files

#define FRC_PN		1e18 			// force unit conversion (g nm /ns2) -> {pN} = {g nm /s2}
#define LIST_STEP 	100

#define INTF_CT_X 	0			// center of the first interface determination sphere
#define INTF_CT_Y 	0			// center of the first interface determination sphere
#define INTF_CT_Z 	0			// center of the first interface determination sphere

/* memo
KBT_0	= 4.114 				{pN nm}
		= 4.114e-12				{kg m  / s2 nm}
		= 4.114e-12	* 1e-6		( g nm /ns2 nm) = (g nm2 /ns2)

K_BOLTZ = 1.380649e-23 			{ m^2 kg /  s^2 / K}
		= 1.380649e-23 * 1e3 	(nm^2 g  / ns^2 / K)
*/



// https://stackoverflow.com/questions/105252
template <typename T>
void SwapEnd(T& var)
{
  char* varArray = reinterpret_cast<char*>(&var);
  for(long i = 0; i < static_cast<long>(sizeof(var)/2); i++)
    std::swap(varArray[sizeof(var) - 1 - i],varArray[i]);
}


#endif