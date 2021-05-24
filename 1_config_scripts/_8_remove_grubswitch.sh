#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit; fi

. _shared_objects.sh


#### _8_remove_grubswitch.sh ####
## deletes modifier script from /etc/...
## runs update-grub

clear
echo -e -n "$fBOLD"
echo "8 - Remove GRUBswitch from grub.cfg"	
echo "-----------------------------------"
echo -e -n "$fPLAIN"
check_request_sudo


### parse commandline parameters for grub dir and script dir
get_path_arguments $@


### check writability of GRUB directories
sudo test -w "${GRUB_CFG_DIR}"
if [ "$?" -ne "0" ]
then
	echo "ERROR: GRUB boot directory not writable" >&2
	exit -1	
fi

sudo test -w "${CFG_SCRIPTS_DIR}"
if [ "$?" -ne "0" ]
then
	echo "ERROR: GRUB script directory not writable" >&2
	exit -1	
fi


### Ask for write confirmation
echo -e "This action will re-generate the ${fBOLD}grub.cfg${fPLAIN} file by calling ${fBOLD}update-grub${fPLAIN}"
echo -e "Do you want to proceed? (${fBOLD}y${fPLAIN})es / (${fBOLD}n${fPLAIN})o"

while [[ true ]]
do
	GET_KEY
	case ${INPUT} in
			"y")
				sudo rm -rf $CFG_SCRIPTS_DIR/99_grub_switch
				echo ; echo "GRUBswitch modifier script removed" ; echo
				sudo update-grub
				echo ; echo -e "${fBOLD}grub.cfg${fPLAIN} re-generated without GRUBswitch" ; echo
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


echo
echo "Press any key to return to main menu."
echo
GET_ANY_KEYPRESS