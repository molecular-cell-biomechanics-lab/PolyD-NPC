#!/bin/bash

# r_in=${rcgo%.*}
# r_in=$(printf "%02d"  $r_in)
# # r_dc=${rcgo#*.}
# r_dc=$(($rcgo))
# po_rcgo="$r_in$r_dc"

# echo $r_in $r_dc $po_rcgo


real_number=2

if [[ $real_number == *"."* ]]; then
  integer=${real_number%.*}
  decimal=${real_number#*.}
else
  integer=$real_number
  decimal=0
fi

echo "Integer: $integer"
echo "Decimal: $decimal"