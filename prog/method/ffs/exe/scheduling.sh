#!/bin/bash

JOBSCRIPT='jobscript_expanse.sh'

pore_list='40'
leng_list='105'
rcgo_list='1.5'
phyd_list='0'
kuhn_list='1'
k_on_list='1e-1'
k_of_list='1e-2'
temp_list='310.15'
seed_list='1'
# seed_list=$(seq 11 1 20)


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
for seed in $seed_list
do

#pounding
po_pore=$(printf "%02d" $pore)
po_phyd=$(printf "%02d" $phyd)
po_seed=$(printf "%03d" $seed)

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


jobname="p${pore}l${leng}b${kuhn}r${po_rcgo}p${po_phyd}k${k_on}k${k_of}t${po_temp}_s${po_seed}"
jobid=$(sbatch --job-name $jobname $JOBSCRIPT $pore $leng $rcgo $phyd $kuhn $k_on $k_of $temp $seed)
jobid=${jobid##* }
echo "Submitted " $jobid


done
done
done
done
done
done
done
done
done




