#!/bin/bash

JOBSCRIPT='jobscript_expanse.sh'

pore=$1
leng=$2
rcgo=$3
phyd=$4
kuhn=$5
k_on=$6
k_of=$7
temp=$8
seed=$9
prev_id=$10

po_pore=$(printf "%02d" $pore)
po_leng=$(printf "%03d" $leng)
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

# move previous run files into log
mv myjob.o_${jobname}_${prev_id} log/
mv myjob.e_${jobname}_${prev_id} log/

# launch new job
jobid=$(sbatch --job-name $jobname $JOBSCRIPT $pore $leng $rcgo $phyd $kuhn $k_on $k_of $temp $seed)
jobid=${jobid##* }
echo "Continuing from " $prev_id
echo "Submitted " $jobid

