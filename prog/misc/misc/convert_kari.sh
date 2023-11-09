#!/bin/bash

vers="3.4.2"

#parameters
pore_list='70 80'
leng_list='209'
rcgo_list='1 2 3 4 5 6 7'
phyd_list='0'
kuhn_list='1'
k_on_list='1e-1'
k_of_list='1e-2'

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

#pounding
po_pore=$(printf "%02d"  $pore)
po_leng=$(printf "%03d"  $leng)
po_phyd=$(printf "%02d"  $phyd)

r_in=$rcgo
r_in=$(printf "%02d"  $r_in)

#output
echo 
echo "pore    = ${po_pore}"
echo "leng    = ${po_leng}"
echo "rcgo    = ${rcgo}"
echo "phyd    = ${po_phyd}"
echo "kuhn    = ${kuhn}"
echo "k_on 	  = ${k_on}"
echo "k_of 	  = ${k_of}"
echo 


# data directories
# d_hom="/expanse/lustre/projects/ucb124/atsushim"
d_hom="../../../.."
d_ver="${d_hom}/bd_v${vers}"
d_dat="${d_ver}/data"
d_por="${d_dat}/pore_d${po_pore}"
d_len="${d_por}/leng_${po_leng}"
d_kuh="${d_len}/kuhn_${kuhn}"
d_kon="${d_kuh}/kon_${k_on}"
d_kof="${d_kon}/kof_${k_of}"
d_car_old="${d_kof}/cargo_r${r_in}"
d_car_new="${d_kof}/cargo_r${r_in}_0"
# d_per="${d_car}/per_${po_phyd}"
# d_bas="${d_per}/basin"

# #make directory if it does not exist
# if [ ! -d $d_dat ];then mkdir $d_dat;fi
# if [ ! -d $d_por ];then mkdir $d_por;fi
# if [ ! -d $d_len ];then mkdir $d_len;fi
# if [ ! -d $d_kuh ];then mkdir $d_kuh;fi
# if [ ! -d $d_kon ];then mkdir $d_kon;fi
# if [ ! -d $d_kof ];then mkdir $d_kof;fi
# if [ ! -d $d_car ];then mkdir $d_car;fi
# if [ ! -d $d_per ];then mkdir $d_per;fi
# if [ ! -d $d_bas ];then mkdir $d_bas;fi

if [ -d $d_car_old ];then
	mv $d_car_old $d_car_new
	# echo $d_car_old 
	# echo $d_car_new
fi

done
done
done
done
done
done
done


