#!/bin/bash

## 0_grub_switch_config.sh
## Master script that controls, sequences all GRUBswitch configuration scripts



### FUNCTIONS
function show_status_menu {

	clear

	echo "GRUBswitch Configuration Menu"
	echo "============================="

	## check sudo state
	check_sudo
	echo -n "Last sudo (super user) status:  ${last_sudo_state}"

	if [ "${last_sudo_state}" = "ACTIVE" ]
	then
		if [ "${sudo_write}" = "ACTIVE" ]
		then
			echo -n " for read/write"
		else
			echo -n " for read-only"
		fi

		echo "  at ${last_sudo_date}"
		echo "                                (this may expire and require password re-entry)"
	else
		echo
		echo
	fi

	echo

	echo "Status of files:"
	echo "----------------"
	echo
	echo "Extracted list of GRUB menu entries  (../bootfiles/grubmenu_all_entries.lst): "
	if [[ -f "../bootfiles/grubmenu_all_entries.lst" ]]
	then
		moddate=`stat -c %Y ../bootfiles/grubmenu_all_entries.lst`
		echo "-> last extracted at " `date -d @"${moddate}"`
	else
		echo "-> not present"
	fi
	echo

	echo "GRUB config file  (${GRUB_CFG_DIR}/grub.cfg):"
	if [[ -f "${GRUB_CFG_DIR}/grub.cfg" ]]
	then
		moddate=`stat -c %Y ${GRUB_CFG_DIR}/grub.cfg`
		echo "-> last modified at " `date -d @"${moddate}"`
	else
		echo "-> not present (that's a problem)"
	fi
	echo

	echo "Permitted SWITCH.GRB file hashes  (${GRUB_CFG_DIR}/grub_switch_hashes/):"
	if [[ -e "${GRUB_CFG_DIR}/grub_switch_hashes/" ]]
	then
		moddate=`stat -c %Y ${GRUB_CFG_DIR}/grub_switch_hashes/`
		echo "-> last updated at " `date -d @"${moddate}"`
	else
		echo "-> not present"
	fi
	echo

	echo "Generated boot files for regular flash drives (../bootfiles/boot.[1..f]):"
	echoout="-> not present"
	for bootfile in `ls ../bootfiles/boot.*/SWITCH.GRB`
	do
		moddate=`stat -c %Y ${bootfile}`
		echoout="-> last updated at `date -d @"${moddate}"`"
		break
	done
	echo $echoout

	echo "Generated boot file for GRUBswitch USB device (../bootfiles/.entries.txt):"
	if [[ -f "../bootfiles/.entries.txt" ]]
	then
		moddate=`stat -c %Y ../bootfiles/.entries.txt`
		echo "-> last updated at" `date -d @"${moddate}"`
	else
		echo "-> not present"
	fi
	echo

	echo
	echo
}

function check_or_find_paths {

	### check specified or default grub.cfg dirs
	if [ "" = "${GRUB_CFG_DIR}" ]
	then
		echo "No valid GRUB boot directory specified."
		echo "Trying default path /boot/grub/..."
		GRUB_CFG_DIR="/boot/grub/"
		if [[ -e "${GRUB_CFG_DIR}" ]]
		then
			echo "...path exists."
		else
			echo "Trying default path /boot/grub2/..."
			GRUB_CFG_DIR="/boot/grub2/"
			if [[ -e "${GRUB_CFG_DIR}" ]]
			then
				echo "...path exists."
			else
				echo "ERROR: No valid specified or default path for GRUB boot files" >&2
				return -1
			fi
		fi
	fi

	### check grub.cfg existence and readability, acquire sudo if required
	GRUB_CFG_PATH="${GRUB_CFG_DIR}/grub.cfg"
	if [[ -e "${GRUB_CFG_PATH}" ]]
	then
		if [[ -r "${GRUB_CFG_PATH}" ]]
		then
			echo "grub.cfg exists and is readable."
		else
			# not readable, so try to get sudo
			echo "grub.cfg exists but sudo rights required to read:"
			check_request_sudo
			if [ "last_sudo_state" = "INACTIVE" ]
			then
				echo "ERROR: Failed getting sudo access to grub.cfg" >&2
				return -1
			else
				echo "sudo rights acquired."
			fi
		fi
	else
		echo "ERROR: grub.cfg does not exist in specified directory" >&2
		return -1
	fi

	### check specified script dir for grub.cfg rebuilds, otherwise extra from grub.cfg
	if [ "" = "${CFG_SCRIPTS_DIR}" ]
	then
		echo "No directory for grub.cfg build scripts specified."
		echo "Trying to extract from grub.cfg comments..."
		# read grub.cfg and find first comment that indicates a generation script, extract full path except file name itself

		# TODO check HOW and WHETHER to require sudo
		CFG_SCRIPTS_DIR=`sudo cat ${GRUB_CFG_PATH} | grep -m 1 "### BEGIN "| sed -n 's/^### BEGIN\o040\(\o057[^\o040]*\o057\).*###$/\1/p'`
		echo "Extracted ${CFG_SCRIPTS_DIR}..."
	fi

	### check path existence
	if [[ -e "${CFG_SCRIPTS_DIR}" ]]
	then :
	else
		echo "ERROR: specified or extracted directory for grub.cfg generation scripts does not exist" >&2
		return -1
	fi

	return 0 # success
}


function check_sudo {
	local sudostate=`sudo -n whoami 2> /dev/null`
	if [ "$sudostate" = "root" ]
	then
		last_sudo_state="ACTIVE"
	else
		last_sudo_state="INACTIVE"
	fi
	last_sudo_date=`date`
}

function check_request_sudo {
	local sudostate=`sudo -n whoami 2> /dev/null`

	if [ "$sudostate" = "root" ]
	then :
	else
		sudostate=`sudo whoami`
	fi

	if [ "$sudostate" = "root" ]
	then
		last_sudo_state="ACTIVE"
	else
		last_sudo_state="INACTIVE"
	fi
	last_sudo_date=`date`
}



### UNSET/INIT RELEVANT VARIABLES

unset last_sudo_state
unset last_sudo_date

unset GRUB_CFG_DIR
unset GRUB_CFG_PATH

unset CFG_SCRIPTS_DIR

sudo_write="INACTIVE"


### CHECKING TOOL AVAILABILITY


### check work path, bootfiles availability
CURR_DIR=`pwd`
if [[ "$CURR_DIR" =~ ^.*/shell_scripts$ ]]
then :
else
	echo "ERROR: Script not started from  shell_scripts  directory" >&2
	return -1
fi

if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	return -1
fi

if [[ -f "../bootfiles/template" ]]
then :
else
	echo "ERROR: ../bootfiles/template not present" >&2
	return -1
fi



### parse commandline parameters for grub dirs, check
OPTIND=1
while getopts ":g:s:" callarg
do
	case ${callarg} in
		g)
			GRUB_CFG_DIR=${OPTARG}
			#echo "grub.cfg dir specified as ${GRUB_CFG_DIR}"
			;;
		s)
			CFG_SCRIPTS_DIR=${OPTARG}
			#echo "script dir for cfg-building specified as ${CFG_SCRIPTS_DIR}"
			;;
		*)
			;;
	esac
done

check_or_find_paths
if [ "$?" -ne "0" ]
then
	return
fi




### show status menu
show_status_menu

