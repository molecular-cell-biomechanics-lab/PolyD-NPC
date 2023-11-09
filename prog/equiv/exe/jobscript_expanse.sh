#!/bin/bash
## Job name is specified externally by scheduling.sh
#SBATCH -o myjob.o_%x_%j    # Name of stdout output file
#SBATCH -e myjob.e_%x_%j    # Name of stderr error file
#SBATCH -N 1               # Total # of nodes
#SBATCH --ntasks-per-node=1
#SBATCH -t 04:00:00        # Run time (hh:mm:ss)
#SBATCH --mail-user=kondono731@gmail.com
#SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH -p shared
#SBATCH -A ucb124


#parameters
pore=$1
leng=$2
kuhn=$3
k_on=$4
k_of=$5
temp=$6

po_pore=$(printf "%02d"  $pore)
po_leng=$(printf "%03d"  $leng)

po_temp=$(echo "scale=0; $temp/1" | bc)
po_temp=$(printf "%04d"  $po_temp)


#output
echo 
echo "pore = ${pore}"
echo "leng = ${leng}"
echo "kuhn = ${kuhn}"
echo "rcgo = ${rcgo}"
echo "k_on = ${k_on}"
echo "k_of = ${k_of}"
echo "temp = ${temp}"
echo 



# directories
d_hom="/expanse/lustre/projects/ucb124/atsushim"
d_ver="${d_hom}/bd_v3.4.2"

# bcs directories
b_bcs="${d_ver}/bcs"
b_car="${b_bcs}/cargo"
b_fgn="${b_bcs}/fgnup"

# ssd directories
s_ssh="/scratch/$USER/job_$SLURM_JOB_ID"
s_sim="${s_ssh}/sim/src/input"
s_equ="${s_ssh}/equiv"
s_out="${s_equ}/output"
s_inp="${s_equ}/input"

cp -r ../../* ${s_ssh}/

./prep_input.sh $pore $kuhn 1 $k_on $k_of $temp $SLURM_JOB_ID
mv bcs_${SLURM_JOB_ID}.txt   ${s_sim}/bcs.txt
mv beads_${SLURM_JOB_ID}.txt ${s_sim}/beads.txt
mv fgnup_${SLURM_JOB_ID}.txt ${s_sim}/fgnup.txt
mv cargo_${SLURM_JOB_ID}.txt ${s_sim}/cargo.txt

# cp ${b_fgn}/before_equiv/fgnup_p${po_pore}.vtk ${s_inp}/fgnup_init.vtk
cp ${b_fgn}/before_equiv/fgnup_p${po_pore}_l${po_leng}.vtk ${s_inp}/fgnup_init.vtk


# main simulation ----------------------------------------------------
cd ${s_equ}
./a.out


#after simulation ends
cd ${s_out}
cp fgnup_equiv.vtk ${b_fgn}/fgnup_equiv_p${po_pore}_l${po_leng}_b${kuhn}_k${k_on}_k${k_of}_t${po_temp}.vtk



