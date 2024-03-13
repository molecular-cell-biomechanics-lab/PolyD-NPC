#!/bin/bash
## Job name is specified externally by scheduling.sh
#SBATCH -o myjob.o_%x_%j    # Name of stdout output file
#SBATCH -e myjob.e_%x_%j    # Name of stderr error file
#SBATCH -N 1               # Total # of nodes
#SBATCH --ntasks-per-node=1
#SBATCH -t 48:00:00        # Run time (hh:mm:ss)
##SBATCH --mail-user=kondono731@gmail.com
##SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH -p shared
#SBATCH -A ucb124

# module load intel mvapich2
# module load parallel/20200822

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
seed=$9			#starting from 0 (modified 2022/10/10)

#pounding
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

#output
echo 
echo "pore    = ${po_pore}"
echo "leng    = ${po_leng}"
echo "rcgo    = ${rcgo}"
echo "phyd    = ${po_phyd}"
echo "kuhn    = ${kuhn}"
echo "k_on    = ${k_on}"
echo "k_of    = ${k_of}"
echo "temp    = ${temp}"
echo "seed    = ${seed}"
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
d_inf="${d_per}/influx"
# d_arc="${d_inf}/arch"

d_exe="$HOME/bd_v${vers}/influx/exe"

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
if [ ! -d $d_inf ];then mkdir $d_inf;fi
# if [ ! -d $d_arc ];then mkdir $d_arc;fi

# SSD directories
s_ssh="/scratch/$USER/job_$SLURM_JOB_ID"
s_sim="${s_ssh}/sim/src/input"
s_inf="${s_ssh}/influx"
s_src="${s_inf}/src/input"
s_inp="${s_inf}/input"
s_out="${s_inf}/output"
s_rec="${s_inf}/rec"


cp -r ../../* 	${s_ssh}/


./prep_input.sh $pore $kuhn $rcgo $k_on $k_of $temp $SLURM_JOB_ID
mv bcs_${SLURM_JOB_ID}.txt   ${s_sim}/bcs.txt
mv beads_${SLURM_JOB_ID}.txt ${s_sim}/beads.txt
mv fgnup_${SLURM_JOB_ID}.txt ${s_sim}/fgnup.txt
mv cargo_${SLURM_JOB_ID}.txt ${s_sim}/cargo.txt


file_init="init_${SLURM_JOB_ID}.txt"
echo "/* init.txt */"   >  ${file_init}
echo "pore = ${pore}" 	>> ${file_init}
echo "rcar = ${rcgo}" 	>> ${file_init}
echo "phyd = ${phyd}" 	>> ${file_init}
echo "kuhn = ${kuhn}" 	>> ${file_init}
echo "k_on = ${k_on}" 	>> ${file_init}
echo "k_of = ${k_of}" 	>> ${file_init}
echo "temp = ${temp}" 	>> ${file_init}
mv $file_init  	${s_inp}/init.txt


cd ${d_inf}
f_prog="prog_s${po_seed}.txt"

if [ -f $f_prog ];then
	# continuation from the previous run
	line=$(cat $f_prog | head -n 1)
	col3=$(echo ${line} | cut -d ' ' -f 3)
	file_param="init_${SLURM_JOB_ID}".txt
	echo "/* init.txt */"   	>  ${file_param}
	echo "strt_div = ${col3}" 	>> ${file_param}
	cp $file_param  ${s_src}/init.txt
	mv $file_param  ${s_src}/debug/init.txt
	cp $f_prog $s_rec
	cp rec_s${po_seed}.tar.gz $s_rec
	cd $s_rec
	tar -zxf rec_s${po_seed}.tar.gz
	rm rec_s${po_seed}.tar.gz
else
	# start from the beginning
	cd ${d_bas}
	ip_name=$(cat ${d_bas}/input.txt | head -n $(($seed+1)) | tail -n 1)  #modified 2022/10/10
	file_cg="cg_${ip_name}_vtx.vtk"
	file_fg="fg_${ip_name}.vtk"
	tar -zxf cg.tar.gz ${file_cg}
	tar -zxf fg.tar.gz ${file_fg}
	mv ${file_cg} 	${s_inp}/s${po_seed}_cargo.vtk
	mv ${file_fg} 	${s_inp}/s${po_seed}_fgnup.vtk
	# cp ${file_cg} 	${s_inp}/s${po_seed}_cargo.vtk
	# cp ${file_fg} 	${s_inp}/s${po_seed}_fgnup.vtk		
fi

# main simulation ----------------------------------------------------
cd ${s_inf}
./a.out ${seed}
cont_id=$? 		# 0:finished, 1:error, 2:continue

./cxn.out ${seed}

# after simulation ends
f_prog="prog_s${po_seed}.txt"
f_rate="rate_s${po_seed}.txt"

if [ -f ${s_out}/${f_prog} ];then
	# record tlll middle
	cd ${s_rec}
	tar -zcf rec_s${po_seed}.tar.gz tmp*
	mv *.tar.gz ${d_inf}/
	mv ${s_out}/${f_prog} ${d_inf}/
else
	cd ${s_out}
	tar -zcf fg_s${po_seed}.tar.gz fg*
	tar -zcf cg_s${po_seed}.tar.gz cg*
	tar -zcf pt_s${po_seed}.tar.gz pt*
	mv *.tar.gz ${d_inf}/
	mv ${f_rate} ${d_inf}/

	cd ${d_inf}
	if [ -f $f_prog ];then
		mv $f_prog $d_arc
		mv rec_s${po_seed}.tar.gz $d_arc
	fi

fi

# run the next job if it did not end
cd $d_exe
if [ $cont_id == 2 ];then
	./cont.sh $pore $leng $rcgo $phyd $kuhn $k_on $k_of $temp $seed $SLURM_JOB_ID
fi



