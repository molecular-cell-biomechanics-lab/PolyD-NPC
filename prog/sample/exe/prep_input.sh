#!/bin/bash

# create input file needed for sim

#parameters
pore=$1
kuhn=$2
rcgo=$3
k_on=$4
k_of=$5
temp=$6
j_id=$7

R_pore=$(($pore/2))

input_bcs="bcs_${j_id}.txt"
echo "/* bcs.txt */"    >  ${input_bcs}
echo "R_pore = ${R_pore}	// 		(nm)" >> ${input_bcs}
echo "cy_top = 40.0 		// 		(nm)" >> ${input_bcs}
echo "cy_btm = 0.0  		// 		(nm)" >> ${input_bcs}
echo "L_x    = 50.0 		// 		(nm)" >> ${input_bcs}
echo "L_y    = 50.0 		// 		(nm)" >> ${input_bcs}
echo "L_z    = 80.0 		// 		(nm)" >> ${input_bcs}


input_beads="beads_${j_id}.txt"
echo "/* beads.txt */"    >  ${input_beads}
echo "l_segm = 0.86 	// segment length of FG-Nups 						(nm)	" >> ${input_beads}
echo "t_delt = 0.02  	// simulation time step 							(ns)	" >> ${input_beads}
echo "T_body = ${temp}	// temperature 										(K)		" >> ${input_beads}
echo "h_visc = 5.0 		// viscosity of water in room temperature 			{cP}	" >> ${input_beads}
echo "r_stks = 0.5 		// Stokes radius of each bead 						{segm}	" >> ${input_beads}
echo "D_diff = --- 		// Diffusion coefficient 	 						(nm2/ns)" >> ${input_beads}
echo "eps_rp = 400		// height of repulsion potential 					{kBT_0}	" >> ${input_beads}
echo "sgm_rp = 1.0		// characteristic length of repulsion potential 	{segm}	" >> ${input_beads}
echo "cut_rp = 1.0		// cutoff distance of repulsion potential 			{segm}	" >> ${input_beads}
echo "eps_fg = not used	// interfacial energy of hydrophobic attraction 	{kBT_0}	" >> ${input_beads}
echo "sgm_fg = not used	// characteristic length of hydrophobic attraction 	{segm}	" >> ${input_beads}
echo "cut_fg = 2.0		// cutoff distance of hydrophobic attraction 		{segm}	" >> ${input_beads}
echo "kon_fg = ${k_on}	// on  rate of hydrophobic attraction 				(/ns)	" >> ${input_beads}
echo "kof_fg = ${k_of} 	// off rate of hydrophobic attraction 				(/ns)	" >> ${input_beads}


input_fgnup="fgnup_${j_id}.txt"
echo "/* fgnup.txt */"    >  ${input_fgnup}
echo "k_tens = 100.0 	// spring constant for tensil force 				{kBT_0/segm2}" 	>> ${input_fgnup}
echo "r_tens = ---		// natural length between neighboring beads			(nm)" 			>> ${input_fgnup}
echo "l_kuhn = ${kuhn}	// Kuhn length of FG-Nups 							{segm}" 		>> ${input_fgnup}
echo "k_bend = ---		// spring constant for bending 						(g nm2 /ns2)" 	>> ${input_fgnup}


input_cargo="cargo_${j_id}.txt"
echo "/* cargo.txt */"    >  ${input_cargo}
echo "R_cargo = ${rcgo} 		// radius of the cargo		(nm)" 	>> ${input_cargo}

