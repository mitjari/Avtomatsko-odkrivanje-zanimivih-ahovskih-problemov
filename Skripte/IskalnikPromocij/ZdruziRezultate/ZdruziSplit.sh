#!/bin/bash

vhod=$1
izhod=$2
vrstic=$3
threads=$4

echo "Pripravljam okolje"
rm -R temp
mkdir temp
echo "Ok. Delim datoteko na dele z $vrstic vrsticami"
split -l $vrstic --additional-suffix .tmp $vhod temp/
echo "Ok. Zacenjam iskanje."

numFiles=$( ls -1 temp/*.tmp | wc -l )
counter=0
for f in temp/*.tmp
do
	while (( 1 == 1 ))
	do
		child_count=$(ps -o pid= --ppid $$ | wc -l)
		((child_count--))
		if [[ $child_count -ge $threads ]]; then continue; fi
		
		((counter++))
		echo "Preiskujem daoteko $f ( $counter / $numFiles )"
		./ZdruziRezultate.sh $f temp/$counter.res &>/dev/null &
		break
	done
done

echo "Cakam na procese"
wait

echo "OK. Združujem začasne datoteke"

echo "promocija,st. poteze, primer" > $izhod
for i in {2..176}
do
	echo -ne "Združujem $i / 176\r"
	
	min="xxx,100,"
	for f in temp/*.res
	do
		line=$(sed -n ${i}p < $f)
		
		trenutna=$(echo $line | awk -F "\"*,\"*" '{print $2}')
		minimalna=$(echo $min | awk -F "\"*,\"*" '{print $2}')
		
		if [[ $minimalna -ge $trenutna ]]; then min=$line; fi
	done
	echo $min >> $izhod
done

echo ""
echo "OK"

echo "..."
wait
echo "KONEC"