#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

. _shared_objects.sh




clear
echo -e -n "${fBOLD}"
echo "3 -   Remove generated files (bootfiles and hashes)"
echo "---------------------------------------------------"
echo -e -n "${fPLAIN}"
echo


### check work path, bootfiles availability
check_in_script_path

if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	EXIT_WITH_KEYPRESS
fi

rm -Rf ../bootfiles/.entries.txt
echo -e "Removed \e[1m.entries.txt\e[0m ..."
rm -Rf ../bootfiles/boot.*
echo -e "Removed \e[1mboot.*/SWITCH.GRB\e[0m ..."
rm -Rf ../bootfiles/grub_switch_hashes
echo -e "Removed \e[1grub_switch_hashes/*\e[0m ..."

EXIT_WITH_KEYPRESS
