#!/bin/bash
## Job name is specified externally by scheduling.sh
#SBATCH -o myjob.o_%x_%j    # Name of stdout output file
#SBATCH -e myjob.e_%x_%j    # Name of stderr error file
#SBATCH -N 1               # Total # of nodes
#SBATCH --ntasks-per-node=128
#SBATCH -t 08:00:00        # Run time (hh:mm:ss)
#SBATCH --mail-user=kondono731@gmail.com
#SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH -p compute
#SBATCH -A ucb124

# module load intel mvapich2
module load parallel/20200822

vers="3.4.2"

#parameters
pore=$1
leng=$2
rcgo=$3
phyd=$4
kuhn=$5
k_on=$6
k_of=$7
temp=$8
seed_max=128
seed=$(seq 1 1 ${seed_max})

#pounding
if [[ $rcgo == *"."* ]]; then
	r_in=${rcgo%.*}
	r_dc=${rcgo#*.}
else
	r_in=$rcgo
	r_dc=0
fi
r_in=$(printf "%02d"  $r_in)
po_rcgo="$r_in$r_dc"

po_pore=$(printf "%02d"  $pore)
po_leng=$(printf "%03d"  $leng)
po_phyd=$(printf "%02d"  $phyd)

po_temp=$(echo "scale=0; $temp/1" | bc)
po_temp=$(printf "%04d"  $po_temp)

#output
echo 
echo "pore    = ${po_pore}"
echo "leng    = ${po_leng}"
echo "rcgo    = ${rcgo}"
echo "phyd    = ${po_phyd}"
echo "kuhn    = ${kuhn}"
echo "k_on	  = ${k_on}"
echo "k_of	  = ${k_of}"
echo "temp	  = ${temp}"
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
d_out="${d_per}/basin"

#make directory if it does not exist
if [ ! -d $d_dat ];then mkdir $d_dat;fi
if [ ! -d $d_tem ];then mkdir $d_tem;fi
if [ ! -d $d_por ];then mkdir $d_por;fi
if [ ! -d $d_len ];then mkdir $d_len;fi
if [ ! -d $d_kuh ];then mkdir $d_kuh;fi
if [ ! -d $d_kon ];then mkdir $d_kon;fi
if [ ! -d $d_kof ];then mkdir $d_kof;fi
if [ ! -d $d_car ];then mkdir $d_car;fi
if [ ! -d $d_per ];then mkdir $d_per;fi
if [ ! -d $d_out ];then mkdir $d_out;fi

# bcs directories
b_bcs="${d_ver}/bcs"
b_car="${b_bcs}/cargo"
b_fgn="${b_bcs}/fgnup"

# SSD directories
s_ssh="/scratch/$USER/job_$SLURM_JOB_ID"
s_sim="${s_ssh}/sim/src/input"
s_bas="${s_ssh}/basin"
s_out="${s_bas}/output"
s_inp="${s_bas}/input"
s_tra="${s_bas}/traj"



cp -r ../../* ${s_ssh}/


#input files
file_init="init_${SLURM_JOB_ID}.txt"
echo "/* init.txt */"  	>  ${file_init}
echo "D_por = ${pore}" 	>> ${file_init}
echo "leng  = ${leng}" 	>> ${file_init}
echo "R_car = ${rcgo}" 	>> ${file_init}
echo "p_hyd = ${phyd}" 	>> ${file_init}
echo "kuhn  = ${kuhn}" 	>> ${file_init}
echo "k_on  = ${k_on}" 	>> ${file_init}
echo "k_of  = ${k_of}" 	>> ${file_init}
echo "temp  = ${temp}" 	>> ${file_init}

file_cgo="${b_car}/cargo_r${r_in}_${r_dc}p${po_phyd}.vtk"
file_fgn="${b_fgn}/fgnup_equiv_p${po_pore}_l${po_leng}_b${kuhn}_k${k_on}_k${k_of}_t${po_temp}.vtk"

cp $file_cgo 	${s_inp}/
cp $file_fgn 	${s_inp}/
mv $file_init  	${s_inp}/init.txt

./prep_input.sh $pore $kuhn $rcgo $k_on $k_of $temp $SLURM_JOB_ID
mv bcs_${SLURM_JOB_ID}.txt   ${s_sim}/bcs.txt
mv beads_${SLURM_JOB_ID}.txt ${s_sim}/beads.txt
mv fgnup_${SLURM_JOB_ID}.txt ${s_sim}/fgnup.txt
mv cargo_${SLURM_JOB_ID}.txt ${s_sim}/cargo.txt



# main simulation ----------------------------------------------------
cd ${s_bas}
parallel -k --lb ./a.out ::: ${seed}

cd ${s_out}
tar -zcf fg.tar.gz fg*
tar -zcf cg.tar.gz cg*
mv *.tar.gz ${d_out}/

cd ${s_bas}
tar -zcf traj.tar.gz traj
mv traj.tar.gz ${d_out}/



