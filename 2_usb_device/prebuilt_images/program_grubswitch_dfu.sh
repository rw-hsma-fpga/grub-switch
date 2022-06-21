#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi


# terminal format macros
fPLAIN="\e[0m"
fBOLD="\e[1m"


DFU_PATH=`which dfu-programmer`
test -x "${DFU_PATH}"
if [ "$?" -ne "0" ]
then
	echo -e "ERROR: ${fBOLD}dfu-programmer${fPLAIN} not installed or not executable."
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



echo
echo "Push PROG button to let board enter DFU bootloader mode;"
echo "then press key to proceed with programming"
echo

OLD_IFS=$IFS
IFS=''

read -s -N 1 KEY
until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush

IFS=$OLD_IFS
sleep 0.5

ATMEGA_TYPE=""

## try 32u4 first
ATMEGA=`lsusb | grep "03eb:2ff4"`
if [ "${ATMEGA}" != "" ]
then
	ATMEGA_TYPE="32u4"
   echo "ATmega32u4 DFU device found."
else
   ATMEGA=`lsusb | grep "03eb:2ff3"`
   if [ "${ATMEGA}" != "" ]
   then
   	ATMEGA_TYPE="16u4"
      echo "ATmega16u4 DFU device found."
   fi
fi

if [ "${ATMEGA}" = "" ]
then
	echo "ERROR: No ATmegaXXu4 DFU device found. Trigger DFU bootloader first."
	exit
fi



BUSNO=`echo $ATMEGA | sed 's/^Bus 0*//' | sed 's/ Device.*$//'`
DEVICENO=`echo $ATMEGA | sed 's/^.*Device 0*//' | sed 's/:.*$//'`


if [ "${BUSNO}" = "" ]
then
	echo "ERROR: Could not extra Bus number."
	exit
fi
if [ "${DEVICENO}" = "" ]
then
	echo "ERROR: Could not extra Device number."
	exit
fi

if [ "${ATMEGA_TYPE}" = "32u4" ]
then
   dfu-programmer atmega32u4:${BUSNO},${DEVICENO} erase --force
   dfu-programmer atmega32u4:${BUSNO},${DEVICENO} flash ${HEXFILE}
   dfu-programmer atmega32u4:${BUSNO},${DEVICENO} launch
fi

if [ "${ATMEGA_TYPE}" = "16u4" ]
then
   dfu-programmer atmega16u4:${BUSNO},${DEVICENO} erase --force
   dfu-programmer atmega16u4:${BUSNO},${DEVICENO} flash ${HEXFILE}
   dfu-programmer atmega16u4:${BUSNO},${DEVICENO} launch
fi

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
