#ifndef FFS_PARAM_H
#define FFS_PARAM_H

#include "vec.h"
#include "sys.h"


class _param
{
public:
	std::vector<double> x_div;	// boundary of interface 			i=0..n
	std::vector<int> 	k_bra;	// # of launched paths at interface i=0..n-1
	std::vector<double>	p_end;	// pruning probability			    i=0..n-1 (always 1 for i=0)
	int 	n_divs; 			// # of interfaces
	// int 	N_strt;				// # of starting points
	int 	strt_div;			// starting interface position

	 _param();
	~_param();

private:
	void set_para();
	void read_init();
	void read_divs();
	void read_kbra();
	void read_pend();
	void read_input();
	void display_input();
};

#endif