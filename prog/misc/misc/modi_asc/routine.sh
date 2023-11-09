#!/bin/bash

R_cargo=$(seq 1 1 19)
p_hyd=$(seq 0 1 20)
# R_cargo='5'
# p_hyd='0'
# p_hyd=$(seq 11 1 20)

for r in $R_cargo
do
for p in $p_hyd
do

	#pounding
	rcgo=$(printf "%02d"  $r)
	phyd=$(printf "%02d"  $p)

	f_inp="../../../../bcs/cargo/cargo_r${rcgo}p${phyd}.vtk"
	
	if [ ! -f $f_inp ];then continue ;fi
	
	cp $f_inp input/cg.vtk
	./a.out cg.vtk
	mv output/cg.vtk $f_inp
	# ./a.out $r $p

done
done
