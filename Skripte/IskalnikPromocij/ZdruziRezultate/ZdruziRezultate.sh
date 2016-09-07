#!/bin/bash

vsePromocije=(

#Promocije brez jemanja
#Beli
"a7a8Q" "b7b8Q" "c7c8Q" "d7d8Q" "e7e8Q" "f7f8Q" "g7g8Q" "h7h8Q"
"a7a8R" "b7b8R" "c7c8R" "d7d8R" "e7e8R" "f7f8R" "g7g8R" "h7h8R"
"a7a8N" "b7b8N" "c7c8N" "d7d8N" "e7e8N" "f7f8N" "g7g8N" "h7h8N"
"a7a8B" "b7b8B" "c7c8B" "d7d8B" "e7e8B" "f7f8B" "g7g8B" "h7h8B"

#Crni
"a2a1Q" "b2b1Q" "c2c1Q" "d2d1Q" "e2e1Q" "f2f1Q" "g2g1Q" "h2h1Q"
"a2a1R" "b2b1R" "c2c1R" "d2d1R" "e2e1R" "f2f1R" "g2g1R" "h2h1R"
"a2a1N" "b2b1N" "c2c1N" "d2d1N" "e2e1N" "f2f1N" "g2g1N" "h2h1N"
"a2a1B" "b2b1B" "c2c1B" "d2d1B" "e2e1B" "f2f1B" "g2g1B" "h2h1B"


#Promocije z jemanjem
#Beli
"a7xb8Q" "b7xa8Q" "b7xc8Q" "c7xb8Q" "c7xd8Q" "d7xc8Q" "d7xe8Q" "e7xd8Q" "e7xf8Q" "f7xe8Q" "f7xg8Q" "g7xf8Q" "g7xh8Q" "h7xg8Q"
"a7xb8R" "b7xa8R" "b7xc8R" "c7xb8R" "c7xd8R" "d7xc8R" "d7xe8R" "e7xd8R" "e7xf8R" "f7xe8R" "f7xg8R" "g7xf8R" "g7xh8R" "h7xg8R"
"a7xb8N" "b7xa8N" "b7xc8N" "c7xb8N" "c7xd8N" "d7xc8N" "d7xe8N" "e7xd8N" "e7xf8N" "f7xe8N" "f7xg8N" "g7xf8N" "g7xh8N" "h7xg8N"
"a7xb8B" "b7xa8B" "b7xc8B" "c7xb8B" "c7xd8B" "d7xc8B" "d7xe8B" "e7xd8B" "e7xf8B" "f7xe8B" "f7xg8B" "g7xf8B" "g7xh8B" "h7xg8B"

#Crni
"a2xb1Q" "b2xa1Q" "b2xc1Q" "c2xb1Q" "c2xd1Q" "d2xc1Q" "d2xe1Q" "e2xd1Q" "e2xf1Q" "f2xe1Q" "f2xg1Q" "g2xf1Q" "g2xh1Q" "h2xg1Q"
"a2xb1R" "b2xa1R" "b2xc1R" "c2xb1R" "c2xd1R" "d2xc1R" "d2xe1R" "e2xd1R" "e2xf1R" "f2xe1R" "f2xg1R" "g2xf1R" "g2xh1R" "h2xg1R"
"a2xb1N" "b2xa1N" "b2xc1N" "c2xb1N" "c2xd1N" "d2xc1N" "d2xe1N" "e2xd1N" "e2xf1N" "f2xe1N" "f2xg1N" "g2xf1N" "g2xh1N" "h2xg1N"
"a2xb1B" "b2xa1B" "b2xc1B" "c2xb1B" "c2xd1B" "d2xc1B" "d2xe1B" "e2xd1B" "e2xf1B" "f2xe1B" "f2xg1B" "g2xf1B" "g2xh1B" "h2xg1B"
)

primeri=()
najkrajsePoti=()
for i in {0..175}; do najkrajsePoti[$i]=100; done
vrstic=`cat $1 | wc -l`
j=0

while IFS='' read -r line || [[ -n "$line" ]]
do
    ((j++))
    if [[ $line != [* ]]
    then
      ((proc= j/vrstic))
      echo -ne "preverjeno: $j / $vrstic ( $proc )\r"

      zadnjaPoteza=$( echo $line | rev | cut -d' ' -f1 | rev )
      stPromocije=0
      #Poisci index promocije v polju ce obstaja
      for i in "${!vsePromocije[@]}"
      do
	if [[ "${vsePromocije[$i]}" = "${zadnjaPoteza}" ]]; then stPromocije=${i}; fi
      done
      
      if [[ $stPromocije -gt 0 ]]
      then
	#Poisci st. poteze
	IFS=' ' read -r -a vrstica <<< "$line"
	zadnjaSt=0
	for v in "${vrstica[@]}"
	do
	  re='^[0-9]+$'
	  st=${v:0:1}
	  if [[ $st =~ $re ]] ; then zadnjaSt=$st; fi
	done
	
	if [[ ${najkrajsePoti[$stPromocije]} -gt $zadnjaSt ]]; then
	  echo "Najdena nova krajsa resiev za $zadnjaPoteza v $zadnjaSt potezah"
	  najkrajsePoti[$stPromocije]=$zadnjaSt;
	  primeri[$stPromocije]=$line
	fi
      fi
    fi
done < $1

echo "promocija,st. poteze, primer" > $2
for i in "${!vsePromocije[@]}"
do
  echo ${vsePromocije[$i]},${najkrajsePoti[$i]},${primeri[$i]} >> $2
done
