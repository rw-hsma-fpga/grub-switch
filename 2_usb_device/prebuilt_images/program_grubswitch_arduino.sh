#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi


# terminal format macros
fPLAIN="\e[0m"
fBOLD="\e[1m"


AVRDUDE_PATH=`which avrdude`
test -x "${AVRDUDE_PATH}"
if [ "$?" -ne "0" ]
then
	echo -e "ERROR: ${fBOLD}avrdude${fPLAIN} programmer not installed or not executable."
	exit
fi



if [ "$1" = "" ]
then
	echo "ERROR: No *.hex file specified."
	exit
fi



HEXFILE=$1

if [ -f "${HEXFILE}" ]
then :
else
	echo "ERROR: No readable hex file ${HEXFILE}"
	exit
fi



for f in /dev/tty*
do
	TTYTIME=`stat -c %Y ${f}`
	NOWTIME=`date +%s`

	DIFFTIME="$((${NOWTIME}-${TTYTIME}))"

	if [ "$DIFFTIME" -lt "10" ]
	then
		NEW_TTY_DEVICE="${f}"
	fi
done



if [ "${NEW_TTY_DEVICE}" = "" ]
then
	echo "ERROR: No tty device found. Push board reset twice before running script"
	exit
else
	echo $f $DIFFTIME
fi


avrdude -v -F -p atmega32u4 -c avr109 -P "${NEW_TTY_DEVICE}" -Uflash:w:${HEXFILE}:i

echo
echo "Waiting for USB device detection..."
sleep 3

NEWDEV=`lsusb | grep "1209:2015"`
if [ "${NEWDEV}" = "" ]
then
	echo
	echo -e "ERROR: ${fBOLD}lsusb${fPLAIN} found no GRUBswitch device with VID 1209 / PID 2015."
	exit
else
	echo
	echo -e "${fBOLD}lsusb${fPLAIN} found flashed GRUBswitch device:"
	echo $NEWDEV
fi
