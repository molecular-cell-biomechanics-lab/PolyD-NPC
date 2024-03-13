#!/bin/bash

# create input files

#parameters
epsf=$1
cutf=$2
rcgo=$3
j_id=$4


# input_bcs="bcs_${j_id}.txt"
# echo "/* bcs.txt */"    >  ${input_bcs}
# echo "R_pore = 25.0			// 		(nm)" >> ${input_bcs}
# echo "cy_top = 8.0	 		// 		(nm)" >> ${input_bcs}
# echo "cy_btm = 0.0  		// 		(nm)" >> ${input_bcs}
# echo "L_x    = 50.0 		// 		(nm)" >> ${input_bcs}
# echo "L_y    = 50.0 		// 		(nm)" >> ${input_bcs}
# echo "L_z    = 50.0 		// 		(nm)" >> ${input_bcs}


input_beads="beads_${j_id}.txt"
echo "/* beads.txt */"    >  ${input_beads}
echo "l_segm = 0.86 	// segment length of FG-Nups 						(nm)	" >> ${input_beads}
echo "t_delt = 0.01  	// simulation time step 							(ns)	" >> ${input_beads}
echo "T_body = 310.15	// temperature 										(K)		" >> ${input_beads}
echo "h_visc = 5.0 		// viscosity of water in room temperature 			{cP}	" >> ${input_beads}
echo "r_stks = 0.5 		// Stokes radius of each bead 						{segm}	" >> ${input_beads}
echo "D_diff = --- 		// Diffusion coefficient 	 						(nm2/ns)" >> ${input_beads}
echo "eps_rp = 1.0		// height of repulsion potential 					{kBT_0}	" >> ${input_beads}
echo "sgm_rp = 1.0		// characteristic length of repulsion potential 	{segm}	" >> ${input_beads}
echo "cut_rp = 1.04		// cutoff distance of repulsion potential 			{segm}	" >> ${input_beads}
echo "sft_rp = 0.732	// shift parameter making the soft-core 			(-)		" >> ${input_beads}
echo "eps_fg = ${epsf}	// interfacial energy of hydrophobic attraction 	{kBT_0}	" >> ${input_beads}
echo "sgm_fg = 1.0 		// characteristic length of hydrophobic attraction 	{segm}	" >> ${input_beads}
echo "cut_fg = ${cutf}	// cutoff distance of hydrophobic attraction 		{segm}	" >> ${input_beads}
echo "sft_fg = 0.732	// shift parameter making the soft-core 			(-)		" >> ${input_beads}


# input_fgnup="fgnup_${j_id}.txt"
# echo "/* fgnup.txt */"    >  ${input_fgnup}
# echo "k_tens = 100.0 	// spring constant for tensil force 				{kBT_0/segm2}" 	>> ${input_fgnup}
# echo "r_tens = ---		// natural length between neighboring beads			(nm)" 			>> ${input_fgnup}
# echo "l_kuhn = 1.0 		// Kuhn length of FG-Nups 							{segm}" 		>> ${input_fgnup}
# echo "k_bend = ---		// spring constant for bending 						(g nm2 /ns2)" 	>> ${input_fgnup}


input_cargo="cargo_${j_id}.txt"
echo "/* cargo.txt */"    >  ${input_cargo}
echo "R_cargo = ${rcgo} 		// radius of the cargo		(nm)" 	>> ${input_cargo}




