#!/bin/bash

JOBSCRIPT='jobscript_expanse.sh'

epsf_list='5'
cutf_list='1.5'
rcgo_list='3'
phyd_list='1'
seed_list='1'
# seed_list=$(seq 11 1 20)


for epsf in $epsf_list
do
for cutf in $cutf_list
do
for rcgo in $rcgo_list
do
for phyd in $phyd_list
do
for seed in $seed_list
do

#pounding
po_phyd=$(printf "%02d" $phyd)
po_seed=$(printf "%03d" $seed)


if [[ $epsf == *"."* ]]; then
	e_in=${epsf%.*}
	e_dc=${epsf#*.}
else
	e_in=$epsf
	e_dc=0
fi
e_in=$(printf "%02d"  $e_in)
po_epsf="$e_in$e_dc"

if [[ $cutf == *"."* ]]; then
	c_in=${cutf%.*}
	c_dc=${cutf#*.}
else
	c_in=$cutf
	c_dc=0
fi
c_in=$(printf "%02d"  $c_in)
po_cutf="$c_in$c_dc"

if [[ $rcgo == *"."* ]]; then
	r_in=${rcgo%.*}
	r_dc=${rcgo#*.}
else
	r_in=$rcgo
	r_dc=0
fi
r_in=$(printf "%02d"  $r_in)
po_rcgo="$r_in$r_dc"


jobname="e${po_epsf}c${po_cutf}r${po_rcgo}p${po_phyd}_s${po_seed}"
jobid=$(sbatch --job-name $jobname $JOBSCRIPT $epsf $cutf $rcgo $phyd $seed)
jobid=${jobid##* }
echo "Submitted " $jobid


done
done
done
done
done




