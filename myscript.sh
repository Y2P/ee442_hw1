#!/bin/bash
echo "Input file is" $1 "Output file is" $2
while (true)
do
	echo "Please enter the pattern"
	read pattern
	count=$(grep $pattern $1 -c)
	echo $pattern $count
	grep $pattern $1 -n
	linenumbers=$(grep -n $pattern $1 | cut -d : -f 1)
	echo $(date) $1 $pattern $count "{" $linenumbers "}">> $2
done
