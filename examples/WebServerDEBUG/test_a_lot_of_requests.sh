#!/bin/bash

i=0
j=0

while [ 1 ]; do
	clear
	echo $i/$j $(echo "($i/$j)*100" | bc -l)%
	wget --timeout=1 -t 1 --server-response http://192.168.1.123/ -O - -q && let i="$i + 1"
	echo
	let j="$j + 1"
	sleep 0.1
done

