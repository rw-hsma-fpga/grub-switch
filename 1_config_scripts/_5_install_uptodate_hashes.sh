#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit; fi

. _shared_objects.sh


#### _5_install_uptodate_hashes.sh ####
## copies ../bootfiles/grub_switch_hashes to boot dir

clear
echo -e -n "$fBOLD"
echo "5 - Install up-to-date hashes"	
echo "-----------------------------"
echo -e -n "$fPLAIN"
check_request_sudo


### parse commandline parameters for grub dir
get_path_arguments $@


### check writability of GRUB directories
sudo test -w "${GRUB_CFG_DIR}"
if [ "$?" -ne "0" ]
then
	echo "ERROR: GRUB boot directory not writable" >&2
	EXIT_WITH_KEYPRESS
fi


### check hash dir availability
if [[ -e "../bootfiles/grub_switch_hashes" ]]
then :
else
	echo "ERROR: ../bootfiles/grub_switch_hashes path not present" >&2
	EXIT_WITH_KEYPRESS
fi


### Ask for write confirmation
echo -e "This action will install up-to-date hashes of permitted SWITCH.GRB files to ${fBOLD}${GRUB_CFG_DIR}${fPLAIN}"
echo -e "Do you want to proceed? (${fBOLD}y${fPLAIN})es / (${fBOLD}n${fPLAIN})o"

while [[ true ]]
do
	GET_KEY
	case ${INPUT} in
			"y")
				sudo rm -rf ${GRUB_CFG_DIR}/grub_switch_hashes
				sudo cp -rf ../bootfiles/grub_switch_hashes ${GRUB_CFG_DIR}/
				echo ; echo "Hashes installed." ; echo
				break
				;;
			"n")
				echo ; echo "Action canceled." ; echo
				sleep 1
				break
				;;
			"*")
				;;
	esac
done

EXIT_WITH_KEYPRESS
