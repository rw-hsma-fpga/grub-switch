#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit; fi

. _shared_objects.sh


#### _3_remove_generated_files ####
## removes .entries.txt, boot.*/*, grub_switch hashes

clear
echo -e -n "${fBOLD}"
echo "3 -   Remove generated files (bootfiles and hashes)"
echo "---------------------------------------------------"
echo -e -n "${fPLAIN}"
echo

### check bootfiles availability
if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	EXIT_WITH_KEYPRESS
fi

### delete files
rm -Rf ../bootfiles/.entries.txt
echo -e "Removed \e[1m.entries.txt\e[0m ..."
rm -Rf ../bootfiles/boot.*
echo -e "Removed \e[1mboot.*/SWITCH.GRB\e[0m ..."
rm -Rf ../bootfiles/grub_switch_hashes
echo -e "Removed \e[1mgrub_switch_hashes/*\e[0m ..."

EXIT_WITH_KEYPRESS
