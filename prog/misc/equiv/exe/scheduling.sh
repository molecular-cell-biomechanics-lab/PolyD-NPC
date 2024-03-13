#!/bin/bash

JOBSCRIPT='jobscript_expanse.sh'

pore_list='40'
leng_list='105'
kuhn_list='1'
k_on_list='1e-1'
k_of_list='1e-2'
temp_list='310.15'


for pore in $pore_list
do
for leng in $leng_list
do
for kuhn in $kuhn_list
do
for k_on in $k_on_list
do
for k_of in $k_of_list
do
for temp in $temp_list
do

jobname="p${pore}_b${kuhn}_l${leng}_k${k_on}_k${k_of}_t${temp}"
jobid=$(sbatch --job-name $jobname $JOBSCRIPT $pore $leng $kuhn $k_on $k_of $temp)
jobid=${jobid##* }
echo "Submitted " $jobid


done
done
done
done
done
done




