#!/bin/bash

osnovna="$(tail -1 $1)"
mkdir REZ

declare -a polja=("a1" "a2" "a7" "a8" "b1" "b2" "b7" "b8" "c1" "c2" "c7" "c8" "d1" "d2" "d7" "d8" 
		  "e1" "e2" "e7" "e8" "f1" "f2" "f7" "f8" "g1" "g2" "g7" "g8" "h1" "h2" "h7" "h8"
		  )

declare -a arr=(
"10:BrezDodatnih:0"
"10:Plus1Figura:1"
"10:Plus2Figuri:2"
"10:Plus3Figure:3"
"10:Plus4Figure:4"
"10:Plus5FIgur:5"
"10:Plus6Figur:6"
"10:Plus7Figur:7"
"10:Plus8Figur:8"
"10:Plus9Figur:9"
"10:Plus10Figur:10"
)

cp $1 TestConfig.conf


for i in "${arr[@]}"
do    
    IFS=':' read -ra ADDR <<< "$i"
    
    Cas="0";
    imeTesta=${ADDR[1]}
    stPonovitev=${ADDR[0]}
    
    echo -e "\e[91m Testiram $stPonovitev x z ${ADDR[2]} dodatnih figur \e[39m"
    
    stevec=1
    stPonovitev=$(( stPonovitev+1 ))
    while [[ $stevec -ne $stPonovitev ]]
    do
	izbira=""
	stDodatnih=${ADDR[2]}
	
	while [[ 0 < $stDodatnih ]]
	do
	    index=$((  RANDOM % 32  ))
	    
	    test="$(echo $osnovna$izbira | grep ${polja[$index]})"
	    
	    if [[ -z $test ]]
	    then
		izbira=$izbira,${polja[$index]}
		((stDodatnih--))
	    fi
	done
    
	echo "PASS $stevec: $izbira"
      
	sed -i '$ d' TestConfig.conf
	echo $osnovna$izbira >> TestConfig.conf
    
	execTime="$(/usr/bin/time -f "\t%e" ./Diploma TestConfig.conf 2>&1 1>/dev/null )"
	
	tmpTxt="$Cas + $execTime"
	Cas="$(echo $tmpTxt | bc -l)"
	
	((stevec++))
    done
    
    ((stevec--))
    PovpCas="$(echo $Cas/$stevec | bc -l)"
    echo -e "\e[32m Povprecni cas: $PovpCas sec. \e[39m"
    outFileName="$(echo $1 | tr / _)"
    echo "$imeTesta: $PovpCas" >> "REZ/$outFileName"
done