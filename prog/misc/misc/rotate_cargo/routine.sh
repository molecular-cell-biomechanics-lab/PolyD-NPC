#!/bin/bash

vers="3.3"

#parameters
rcar_list=$(seq 1 1 19)
phyd_list=$(seq 0 1 10)


for rcar in $rcar_list
do
for phyd in $phyd_list
do

#pounding
po_r_cgo=$(printf "%02d" $rcar)
po_p_hyd=$(printf "%02d" $phyd)

echo "/* init.txt */"   >  init.txt
echo "rcar = ${rcar}" 	>> init.txt
echo "phyd = ${phyd}" 	>> init.txt

./a.out

done
done

