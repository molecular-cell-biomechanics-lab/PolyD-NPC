#!/bin/bash

vers="3.3"

#parameters
pore=40
rcgo=4
phyd=3
kuhn=1
k_on="1e-5"
k_of="1e-4"

seed_list=$(seq 0 1 20)

#pounding
po_pore=$(printf "%02d"  $pore)
po_rcgo=$(printf "%02d"  $rcgo)
po_phyd=$(printf "%02d"  $phyd)

#output
echo 
echo "pore    = ${po_pore}"
echo "rcgo    = ${po_rcgo}"
echo "phyd    = ${po_p_hyd}"
echo "kuhn    = ${kuhn}"
echo "k_on    = ${k_on}"
echo "k_of    = ${k_of}"
echo 


# data directories
# d_hom="/expanse/lustre/projects/ucb124/atsushim"
d_hom="../../.."
d_ver="${d_hom}/bd_v${vers}"
d_dat="${d_ver}/data"
d_por="${d_dat}/pore_d${po_pore}"
d_kuh="${d_por}/kuhn_${kuhn}"
d_kon="${d_kuh}/kon_${k_on}"
d_kof="${d_kon}/kof_${k_of}"
d_car="${d_kof}/cargo_r${po_rcgo}"
d_per="${d_car}/per_${po_phyd}"
d_bas="${d_per}/basin"
d_ffs="${d_per}/ffs"


cd $d_ffs

for seed in $seed_list
do
	po_seed=$(printf "%03d" $seed)
	file="pt_s${po_seed}.tar.gz"
	if [ ! -f $file ];then continue;fi
	tar -zxf $file
	echo $po_seed
done




