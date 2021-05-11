#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi



# terminal format macros
fPLAIN="\e[0m"
fBOLD="\e[1m"

# keypress polling function
function GET_ANY_KEYPRESS () {

	OLD_IFS=$IFS
	IFS=''

	read -s -N 1 KEY
	until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush

	IFS=$OLD_IFS
} # END function GET_ANY_KEYPRESS




clear
echo -e -n "${fBOLD}"
echo "3 -   Remove generated files (bootfiles and hashes)"
echo "---------------------------------------------------"
echo -e -n "${fPLAIN}"
echo


### check work path, bootfiles availability
CURR_DIR=`pwd`
if [[ "$CURR_DIR" =~ ^.*/shell_scripts$ ]]
then :
else
	echo "ERROR: Script not started from  shell_scripts  directory" >&2
	exit -1
fi

if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	exit -1
fi

rm -Rf ../bootfiles/.entries.txt
echo -e "Removed \e[1m.entries.txt\e[0m ..."
rm -Rf ../bootfiles/boot.*
echo -e "Removed \e[1mboot.*/SWITCH.GRB\e[0m ..."
rm -Rf ../bootfiles/grub_switch_hashes
echo -e "Removed \e[1grub_switch_hashes/*\e[0m ..."


echo
echo "Press any key to return to main menu."
echo
GET_ANY_KEYPRESS
