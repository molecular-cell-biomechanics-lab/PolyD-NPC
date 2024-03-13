#!/bin/bash
## Job name is specified externally by scheduling.sh
#SBATCH -o myjob.o_%x_%j    # Name of stdout output file
#SBATCH -e myjob.e_%x_%j    # Name of stderr error file
#SBATCH -N 1               # Total # of nodes
#SBATCH --ntasks-per-node=1
#SBATCH -t 03:00:00        # Run time (hh:mm:ss)
##SBATCH --mail-user=kondono731@gmail.com
##SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH -p shared
#SBATCH -A ucb332

# module load intel mvapich2
# module load parallel/20200822

vers="1"

#parameters
epsf=$1
cutf=$2
rcgo=$3			# modify for the automation later!!!
phyd=$4			# modify for the automation later!!!
seed=$5			# starting from 0

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


#output
echo 
echo "epsf    = ${epsf}"
echo "cutf    = ${cutf}"
echo "rcgo    = ${rcgo}"
echo "phyd    = ${phyd}"
echo "seed    = ${seed}"
echo 


# data directories
d_hom="/expanse/lustre/projects/ucb332/atsushim"
d_ver="${d_hom}/ga_v${vers}"
d_dat="${d_ver}/data"
d_eps="${d_dat}/epsf_${e_in}_${e_dc}"
d_cut="${d_eps}/cutf_${c_in}_${c_dc}"
d_car="${d_cut}/cargo_r${r_in}_${r_dc}"
d_per="${d_car}/per_${po_phyd}"

d_exe="$HOME/ga_v${vers}/sim/exe"


#make directory if it does not exist
if [ ! -d $d_dat ];then mkdir $d_dat;fi
if [ ! -d $d_eps ];then mkdir $d_eps;fi
if [ ! -d $d_cut ];then mkdir $d_cut;fi
if [ ! -d $d_car ];then mkdir $d_car;fi
if [ ! -d $d_per ];then mkdir $d_per;fi


# SSD directories
s_ssh="/scratch/$USER/job_$SLURM_JOB_ID"
s_sim="${s_ssh}/sim"
s_inp="${s_sim}/src/input"
s_vtk="${s_sim}/input"
s_out="${s_sim}/output"

cp -r ../../sim 	${s_ssh}/


./prep_input.sh $epsf $cutf $rcgo $SLURM_JOB_ID
# mv bcs_${SLURM_JOB_ID}.txt   ${s_inp}/bcs.txt
mv beads_${SLURM_JOB_ID}.txt ${s_inp}/beads.txt
# mv fgnup_${SLURM_JOB_ID}.txt ${s_inp}/fgnup.txt
mv cargo_${SLURM_JOB_ID}.txt ${s_inp}/cargo.txt



# main simulation ----------------------------------------------------
cd ${s_sim}
./a.out ${seed}

cd ${s_out}
tar -zcf fg_s${po_seed}.tar.gz fg*
tar -zcf cg_s${po_seed}.tar.gz cg*

mv *.tar.gz ${d_per}/



