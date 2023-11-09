#!/bin/bash

vers="3.4.2"

#parameters
pore_list='40'
leng_list='209'
rcar_list='2'
phyd_list='0'
kuhn_list='1'
k_on_list='1e-1'
k_of_list='1e-2'
temp_list='400'


for pore in $pore_list
do
for leng in $leng_list
do
for rcar in $rcar_list
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
po_pore=$(printf "%02d" $pore)
po_leng=$(printf "%03d" $leng)
po_phyd=$(printf "%02d" $phyd)

po_temp=$(echo "scale=0; $temp/1" | bc)
po_temp=$(printf "%04d"  $po_temp)

if [[ $rcar == *"."* ]]; then
	r_in=${rcar%.*}
	r_dc=${rcar#*.}
else
	r_in=$rcar
	r_dc=0
fi
r_in=$(printf "%02d"  $r_in)
po_rcgo="$r_in$r_dc"

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

d_exe="$HOME/bd_v${vers}/misc/basin_list"

cd $d_bas
tar -tf cg.tar.gz *vtx.vtk > list.txt
cd $d_exe



echo "/* para.txt */"   >  para.txt
echo "pore = ${pore}" 	>> para.txt
echo "leng = ${leng}" 	>> para.txt
echo "rcar = ${rcar}" 	>> para.txt
echo "phyd = ${phyd}" 	>> para.txt
echo "kuhn = ${kuhn} " 	>> para.txt
echo "k_on = ${k_on} " 	>> para.txt
echo "k_of = ${k_of} " 	>> para.txt
echo "temp = ${temp} " 	>> para.txt


./a.out


done
done
done
done
done
done
done
done
