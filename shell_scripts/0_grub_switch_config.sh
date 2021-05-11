#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi


## 0_grub_switch_config.sh
## Master script that controls, sequences all GRUBswitch configuration scripts


# terminal format macros
fPLAIN="\e[0m"
fBOLD="\e[1m"
fUNBOLD="\e[22m"
fDIM="\e[2m"
fUNDIM="\e[22m"
fREVERSE="\e[7m"
# color format macros
fgDEFAULT="\e[39m" ; bgDEFAULT="\e[49m"
fgBLACK="\e[30m" ; fgRED="\e[31m" ; fgGREEN="\e[32m" ; fgYELLOW="\e[33m"
fgBLUE="\e[34m" ; fgMAGENTA="\e[35m" ; fgCYAN="\e[36m" ; fgLIGHTGRAY="\e[37m"
fgDARKGRAY="\e[90m" ; fgLIGHTRED="\e[91m" ; fgLIGHTGREEN="\e[92m" ; fgLIGHTYELLOW="\e[93m"
fgLIGHTBLUE="\e[94m" ; fgLIGHTMAGENTA="\e[95m" ; fgLIGHTCYAN="\e[96m" ; fgWHITE="\e[97m"
bgBLACK="\e[40m" ; bgRED="\e[41m" ; bgGREEN="\e[42m" ; bgYELLOW="\e[43m"
bgBLUE="\e[44m" ; bgMAGENTA="\e[45m" ; bgCYAN="\e[46m" ; bgLIGHTGRAY="\e[47m"
bgDARKGRAY="\e[100m" ; bgLIGHTRED="\e[101m" ; bgLIGHTGREEN="\e[102m" ; bgLIGHTYELLOW="\e[103m"
bgLIGHTBLUE="\e[104m" ; bgLIGHTMAGENTA="\e[105m" ; bgLIGHTCYAN="\e[106m" ; bgWHITE="\e[107m"


### FUNCTIONS

# keyboard polling function... puts key name in $INPUT; empty if no interesting key
function GET_KEYPRESS () {

	OLD_IFS=$IFS
	IFS=''

	INPUT=""
	read -s -N 1 KEY
	case $KEY in
	[1-9])
		INPUT=$KEY
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	[q])
		KEY=`echo ${KEY,,}`
		INPUT=$KEY
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	esac

	IFS=$OLD_IFS
} # END function GET_KEYPRESS

function show_status_menu {

	clear

	echo -e -n "$fBOLD"
	echo "GRUBswitch Configuration Menu"
	echo "============================="
	echo -e -n "$fPLAIN"

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

	echo -e -n "$fBOLD"
	echo "Status of files:"
	echo "----------------"
	echo -e -n "$fPLAIN"
	echo

	echo -e -n "$fgLIGHTBLUE"
	# grubmenu_all_entries.lst
	echo "Extracted list of GRUB menu entries  (../bootfiles/grubmenu_all_entries.lst): "
	if [[ -f "../bootfiles/grubmenu_all_entries.lst" ]]
	then
		moddate=`stat -c %Y ../bootfiles/grubmenu_all_entries.lst`
		echo "-> last extracted at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`
	else
		echo "-> not present"
	fi
	echo

	# /bootfiles/boot.[1..f]
	echo "Generated boot files for regular flash drives (../bootfiles/boot.[1..f]):"
	moddate=""
	for j in 1 2 3 4 5 6 7 8 9 a b c d e f
	do
		if [[ -f "../bootfiles/boot.${j}/SWITCH.GRB" ]]
		then
			moddate=`stat -c %Y "../bootfiles/boot.${j}/SWITCH.GRB"`
		fi
	done
	if [ -z "$moddate" ]
	then
		echo "-> not present"
	else
		echo "-> last updated at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`
	fi

	# /bootfiles/.entries.txt
	echo "Generated boot file for GRUBswitch USB device (../bootfiles/.entries.txt):"
	if [[ -f "../bootfiles/.entries.txt" ]]
	then
		moddate=`stat -c %Y ../bootfiles/.entries.txt`
		echo "-> last updated at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`
	else
		echo "-> not present"
	fi
	echo
	echo -e -n "$fgDEFAULT"

	# grub_switch_hashes/*
	echo -e -n "$fgGREEN"
	echo "Permitted SWITCH.GRB file hashes  (${GRUB_CFG_DIR}/grub_switch_hashes/*):"
	if [[ -e "${GRUB_CFG_DIR}/grub_switch_hashes/" ]]
	then
		moddate=`stat -c %Y ${GRUB_CFG_DIR}/grub_switch_hashes/`
		echo "-> last updated at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`
	else
		echo "-> not present (no permission checking)"
	fi
	echo
	echo -e -n "$fgDEFAULT"

	# grub.cfg
	echo -e -n "$fgRED"
	echo "GRUB config file  (${GRUB_CFG_DIR}/grub.cfg):"
	if [[ -f "${GRUB_CFG_DIR}/grub.cfg" ]]
	then
		moddate=`stat -c %Y ${GRUB_CFG_DIR}/grub.cfg`
		echo "-> last modified at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`

		CURR_GRUBCFG_VER=`cat ${GRUB_CFG_PATH} | grep GRUBswitch_script`
		if [ -z "$CURR_GRUBCFG_VER" ]
		then
			echo "   No GRUBswitch code included"
		else
			CURR_MODSCR_VER=`cat modifier_script/99_grub_switch | grep GRUBswitch_script`
			if [ "$CURR_GRUBCFG_VER" = "$CURR_MODSCR_VER" ]
			then
				echo "   contains up-to-date GRUBswitch code"
			else
				echo "   contains outdated GRUBswitch code"
			fi
		fi
	else
		echo "-> not present (that's a problem)"
	fi
	echo
	echo -e -n "$fgDEFAULT"



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
			if [ ${last_sudo_state} = "INACTIVE" ]
			then
				echo "ERROR: Failed getting sudo access to grub.cfg" >&2
				return -1
			else
				sudo test -r ${GRUB_CFG_PATH}
				if [ "$?" -ne "0" ]
				then
					echo "sudo rights acquired. Still no read access to ${GRUB_CFG_PATH} possible."
					return -1
				else
					echo "sudo rights acquired, can read grub.cfg"
				fi
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
		test -r ${GRUB_CFG_PATH}
		if [ "$?" -ne "0" ]
		then
			check_request_sudo
			CFG_SCRIPTS_DIR=`sudo cat ${GRUB_CFG_PATH} | grep -m 1 "### BEGIN "| sed -n 's/^### BEGIN\o040\(\o057[^\o040]*\o057\).*###$/\1/p'`
		else
			CFG_SCRIPTS_DIR=`cat ${GRUB_CFG_PATH} | grep -m 1 "### BEGIN "| sed -n 's/^### BEGIN\o040\(\o057[^\o040]*\o057\).*###$/\1/p'`
		fi
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

function check_request_sudo_write {

	if [ "$sudo_write" = "INACTIVE" ]
	then
		echo; echo "This action requires sudo (super user) write access." ;	echo
		sleep 1
		sudo -K
		check_request_sudo
		if [ "$last_sudo_state" = "ACTIVE" ]
		then
			sudo_write="ACTIVE"
			echo ; echo "sudo write access acquired."
			sleep 2
		else
			echo ; echo "No sudo acquired. Returning to menu."
			sleep 2
		fi
	else # sudo write ACTIVE
		check_sudo
		if [ "$last_sudo_state" = "INACTIVE" ]
		then
			echo; echo "This action requires sudo (super user) write access." ;	echo
			sleep 1
			check_request_sudo
			if [ "$last_sudo_state" = "ACTIVE" ]
			then
				echo ; echo "sudo write access acquired."
				sleep 2
			else
				echo ; echo "No sudo acquired. Returning to menu."
				sleep 2
			fi
		fi
	fi
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
	exit -1
fi

if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	exit -1
fi

if [[ -f "../bootfiles/template" ]]
then :
else
	echo "ERROR: ../bootfiles/template not present" >&2
	exit -1
fi



### parse commandline parameters for grub dirs, check existence/access
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
	exit -1
fi


### MAIN LOOP
while [[ true ]]
do

	### show status menu
	show_status_menu

	### show actions
	echo
	echo -e -n "$fBOLD"
	echo "ACTIONS:"
	echo "--------"
	echo -e -n "$fPLAIN"
	echo
	echo -e -n "$fgLIGHTBLUE"
	echo "1 - Extract all menu entries from grub.cfg"
	echo "2 - Configure GRUBswitch order and generate bootfiles and hashes"
	echo "3 -   Remove generated files"
	echo "4 - Write GRUBswitch bootfile to GRUBswitch USB device        (requires sudo)"
	echo -e -n "$fgDEFAULT"
	echo
	echo -e -n "$fgGREEN"
	echo "5 - Install up-to-date hashes for permitted SWITCH.GRB files  (requires sudo)"
	echo "6 -   Remove all hashes, no permission checking               (requires sudo)"
	echo -e -n "$fgDEFAULT"
	echo
	echo -e -n "$fgRED"
	echo "7 - Install GRUBswitch into grub.cfg                          (requires sudo)"
	echo "8 -   Remove GRUBswitch from grub.cfg                         (requires sudo)"
	echo -e -n "$fgDEFAULT"
	echo
	echo "q - Quit"

	echo
	echo

	GET_KEYPRESS

	case ${INPUT} in
			"q")
				clear
				echo "Exited GRUBswitch configuration."
				echo
				exit ;;
			"1")
				./_1_extract_menuentries.sh -g $GRUB_CFG_DIR
				;;
			"2")
				#
				;;
			"3")
				./_3_remove_generated_files.sh
				#
				;;
			"4")
				#
				;;
			"5")
				#
				;;
			"6")
				#
				;;
			"7")
				clear
				echo -e -n "$fBOLD"
				echo "7 - Install GRUBswitch into grub.cfg"	
				echo "------------------------------------"
				echo -e -n "$fPLAIN"
				check_request_sudo_write
				./_7_install_grubswitch.sh
				;;
			"8")
				clear
				echo -e -n "$fBOLD"
				echo "8 -   Remove GRUBswitch from grub.cfg"	
				echo "-------------------------------------"
				echo -e -n "$fPLAIN"
				check_request_sudo_write
				./_8_remove_grubswitch.sh
				;;
				;;
			"9")
				#
				;;
			"*")
				# Whatever.
				;;
	esac

done
