#!/bin/bash

vers="3.4.2"

#parameters
pore_list='40'
leng_list='209'
# rcgo_list=$(seq 1 1 7)
rcgo_list='0.5'
phyd_list='0'
kuhn_list='1'
k_on_list='1e-1'
k_of_list='1e-2'
temp_list='310'

for pore in $pore_list
do
for leng in $leng_list
do
for rcgo in $rcgo_list
do
for phyd in $phyd_list
do
for kuhn in $kuhn_list
do
for k_on in $k_on_list
do
for k_of in $k_of_list
do
for temp in $temp_list
do

#pounding
po_pore=$(printf "%02d"  $pore)
po_leng=$(printf "%03d"  $leng)
po_phyd=$(printf "%02d"  $phyd)

po_temp=$(echo "scale=0; $temp/1" | bc)
po_temp=$(printf "%04d"  $po_temp)

if [[ $rcgo == *"."* ]]; then
	r_in=${rcgo%.*}
	r_dc=${rcgo#*.}
else
	r_in=$rcgo
	r_dc=0
fi
r_in=$(printf "%02d"  $r_in)
po_rcgo="$r_in$r_dc"

#output
echo 
echo "pore    = ${po_pore}"
echo "leng    = ${po_leng}"
echo "rcgo    = ${po_rcgo}"
echo "phyd    = ${po_phyd}"
echo "kuhn    = ${kuhn}"
echo "k_on 	  = ${k_on}"
echo "k_of 	  = ${k_of}"
echo "temp    = ${temp}"
echo 


# data directories
d_hom="/expanse/lustre/projects/ucb124/atsushim"
d_ver="${d_hom}/bd_v${vers}"
d_dat="${d_ver}/data"
d_tem="${d_dat}/temp_${po_temp}"
d_por="${d_tem}/pore_d${po_pore}"
d_len="${d_por}/leng_${po_leng}"
d_kuh="${d_len}/kuhn_${kuhn}"
d_kon="${d_kuh}/kon_${k_on}"
d_kof="${d_kon}/kof_${k_of}"
d_car="${d_kof}/cargo_r${r_in}_${r_dc}"
d_per="${d_car}/per_${po_phyd}"
d_bas="${d_per}/basin"
d_ffs="${d_per}/ffs"
d_msd="${d_ffs}/msd"


name="p${pore}l${leng}b${kuhn}k${k_on}k${k_of}r${po_rcgo}h${phyd}t${temp}"
cd $d_msd
cp msd_alpha.txt 	$HOME/msd_alpha_${name}.txt
cp msd_reduced.txt 	$HOME/msd_reduced_${name}.txt


done
done
done
done
done
done
done
done


