#!/bin/sh

LIST=`ls -1 boards/include/ | grep .h | grep -v boards.h | tr [a-z] [A-Z] | cut -d"." -f-1`


echo
echo "Running make_all_boards ..."
echo


for BRD in $LIST
do
	echo
	echo "############################"
	echo "#"
	echo "# BOARD NAME:  ${BRD}"
	echo "#"
	echo "############################"
	echo
	make BOARD=${BRD}
done
