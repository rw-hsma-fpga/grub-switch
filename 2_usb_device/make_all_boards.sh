#!/bin/bash

LIST=`ls -1 boards/include/ | grep .h | grep -v boards.h | tr [a-z] [A-Z] | cut -d"." -f-1`


echo
echo "Running make_all_boards ..."
echo


for BRD in $LIST
do
	MCU="atmega32u4"
	if [[ "$BRD" == *"16U4"* ]]
	then
		MCU="atmega16u4"
	fi

	echo
	echo "############################"
	echo "#"
	echo "# BOARD NAME:  ${BRD}"
	echo "# MCU:         ${MCU}"
	echo "#"
	echo "############################"
	echo
	make MCU=${MCU} BOARD=${BRD}
done
