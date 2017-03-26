#!/bin/bash
# Print input and output files
echo "Input file is" $1 "Output file is" $2
# Infinite loop
while (true)
do
	# Read the pattern from user
	echo "Please enter the pattern"
	read pattern
	# Take the number of occurence of the pattern with grep -c command
	count=$(grep $pattern $1 -c)
	# Print the count
	echo $pattern $count
	# Show pattern occurences and line numbers
	grep $pattern $1 -n
	# Parse the line numbers
	linenumbers=$(grep -n $pattern $1 | cut -d : -f 1)
	# Put the results to the output file
	echo $(date) $1 $pattern $count "{" $linenumbers "}">> $2
done
