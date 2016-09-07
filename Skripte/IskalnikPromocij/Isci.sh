#!/bin/bash

trap control_c SIGINT

control_c()
{
  echo "Zakljucujem..."
  pkill -TERM -P $$
  killall TestMoveGenerator
  killall timeout
  sleep 1
  (( zadnjaPreverjena= currentIndex - threads ))
  echo "OK! Zadnja preverjena kombinacija je st. $zadnjaPreverjena"
  rm  CONFIG/*

  echo "čakam na podprocese..."
  wait

  exit
}

echo "Pripravljam kombinacije aktivnih figur"

config=${1:-config.conf}
threads=${2:-1}
ram=${3:-1}
currentIndex=${4:-0}


#Sparsaj config datoteko
plys=0
while IFS='' read -r line || [[ -n "$line" ]]
do
  key=$(echo $line | awk -F'=' '{print $1}')
  value=$(echo $line | awk -F'=' '{print $2}')
  
  if [[ $key = "MOVES" ]]; then (( plys= value * 2 )); fi
  if [[ $key = "WINCOLOR" ]] && [[ $value = "W" ]]; then (( plys-- )); fi
done < $config

polja=""
kmeti=""
osnovno=""

if (( plys % 2 == 1 ))
then
  polja=("a1" "a2" "b1" "b2" "c1" "c2" "d1" "d2" "e1" "e2" "f1" "f2" "g1" "g2" "h1" "h2" "")
  osnovno="a7,a8,b7,b8,c7,c8,d7,d8,e7,e8,f7,f8,g7,g8,h7,h8"
  kmeti=("a2" "b2" "c2" "d2" "e2" "f2" "g2" "h2")
else
  polja=("a7" "a8" "b7" "b8" "c7" "c8" "d7" "d8" "e7" "e8" "f7" "f8" "g7" "g8" "h7" "h8" "" )
  osnovno="a1,a2,b1,b2,c1,c2,d1,d2,e1,e2,f1,f2,g1,g2,h1,h2"
  kmeti=("a7" "b7" "c7" "d7" "e7" "f7" "g7" "h7")
fi

kombinacije=()
((stDodatnih=(plys+1)/2 -5))

for kmet in "${kmeti[@]}"
do
    indexi=(); for (( i=0; i<$stDodatnih; i++ )); do indexi[$i]=0; done
    while (( 1 == 1 ))
    do
      #Preberi polja iz indexov
      dodatnaPolja=""
      for dodatno in "${indexi[@]}"
      do
	dodatnaPolja=$dodatnaPolja,${polja[$dodatno]}
	aktivna=$osnovno,$kmet$dodatnaPolja
      done
      kombinacije+=($aktivna)
      
      #Premakni se na naslednjo kombinacijo
      (( indexi[stDodatnih-1]++ ))
      
      #Popravi indexe ze preglewdanih polj
      for (( i=$stDodatnih-1; 0<=i; i-- ))
      do
	if [[ -z ${polja[ ${indexi[i]} ]} ]]
	then 
	  if [[ $i -eq 0 ]]; then break 2; fi
	  (( indexi[i-1]++ ))
	  indexi[i]=0
	fi
      done
      
     done
done


echo "***************************"
echo "Iscem mat z promocijo v $plys polpotezah"
echo "Vzporeznih procesov: $threads"
(( ramProc= ram/threads ))
echo "RAM: $ram GB ( $ramProc GB / proces )"

stKombinacij=${#kombinacije[@]}
echo "Za preveriti $stKombinacij kombinacij aktivnih figur"
echo "Nadaljujem od kombinacije st: $currentIndex"


echo "***************************"
echo "OK! Pritisni enter za zacetek iskanja."
read
echo "Iscem... Za izhod pritisnite Ctrl-c"

while [[ ${#kombinacije[@]} -gt $currentIndex ]]
do
  sleep 1
  kombinacija=${kombinacije[$currentIndex]}
  
  child_count=$(ps -o pid= --ppid $$ | wc -l)
  ((child_count--))
  
  if [[ $child_count -ge $threads ]]; then continue; fi
  
  ((stKomb= stKombinacij-1 ))
  (( procenti= currentIndex * 100 / stKomb ))
  echo "$currentIndex/$stKomb: $kombinacija ... ($procenti %)"
  
  #**********PROCES V OZADJU*************
  
  cat $config > "CONFIG/$currentIndex.conf"
  echo "ACTIVE=$kombinacija" >> "CONFIG/$currentIndex.conf"
  
  (( mem= $ramProc * 1000000 ))
  ./timeout -m $mem ./TestMoveGenerator CONFIG/$currentIndex.conf REZ/$currentIndex.pgn &>/dev/null &
  
  #**************************************
  (( currentIndex++ ))
done


echo "čakam na podprocese..."
wait
