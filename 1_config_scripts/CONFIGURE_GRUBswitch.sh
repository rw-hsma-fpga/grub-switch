#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit 13 ; fi  ## ERROR_PERMISSION_DENIED

. _shared_objects.sh


#### CONFIGURE_GRUBswitch.sh ####
## Master script that controls, sequences all GRUBswitch configuration scripts

### FUNCTIONS

function show_status_menu {

	clear

	## check sudo state
	check_sudo_reacquire_or_exit
	EXIT_ON_FAIL

	clear

	echo -e -n "$fBOLD"
	echo "GRUBswitch Configuration Menu"
	echo "============================="
	echo -e -n "$fPLAIN"


	echo

	echo -e -n "$fBOLD"
	echo "Status of files:"
	echo "----------------"
	echo -e -n "$fPLAIN"
	echo

	echo -e -n "$fgDEFAULT"
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
	echo -e -n "$fgCYAN"
	echo "Permitted SWITCH.GRB file hashes  (${GRUB_CFG_DIR}grub_switch_hashes/*):"
	sudo test -e "${GRUB_CFG_DIR}/grub_switch_hashes/"
	if [ "$?" -eq "0" ]	
	then
		moddate=`sudo stat -c %Y ${GRUB_CFG_DIR}/grub_switch_hashes/`
		echo "-> last updated at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`
	else
		echo "-> not present (no permission checking)"
	fi
	echo
	echo -e -n "$fgDEFAULT"

	# grub.cfg
	echo -e -n "$fgGREEN"
	echo "GRUB menu config file  (${GRUB_CFG_DIR}grub.cfg):"
	sudo test -f "${GRUB_CFG_DIR}/grub.cfg"
	if [ "$?" -eq "0" ]	
	then
		moddate=`sudo stat -c %Y ${GRUB_CFG_DIR}/grub.cfg`
		echo "-> last modified at" `date -d @"${moddate}" +"%d %b %Y - %H:%M:%S"`

		CURR_GRUBCFG_VER=`sudo cat ${GRUB_CFG_PATH} | grep GRUBswitch_script`

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
		echo "No GRUB boot directory argument specified on command-line."
		echo "Trying default path /boot/grub/..."
		GRUB_CFG_DIR="/boot/grub/"
		sudo test -e ${GRUB_CFG_DIR}
		if [ "$?" -eq "0" ]
		then
			echo "...path exists."
		else
			echo "Trying default path /boot/grub2/..."
			GRUB_CFG_DIR="/boot/grub2/"
			sudo test -e ${GRUB_CFG_DIR}
			if [ "$?" -eq "0" ]
			then
				echo "...path exists."
			else
				echo "ERROR: No valid specified or default path for GRUB boot files" >&2
				return "$ERROR_NO_SUCH_FILE_OR_DIR"
			fi
		fi
	fi

	### check grub.cfg existence and readability
	GRUB_CFG_PATH="${GRUB_CFG_DIR}/grub.cfg"
	sudo test -e ${GRUB_CFG_PATH}
	if [ "$?" -eq "0" ]
	then
		echo "grub.cfg exists"
		check_sudo_reacquire_or_exit
		EXIT_ON_FAIL
		sudo test -r ${GRUB_CFG_PATH}
		if [ "$?" -ne "0" ]
		then
			echo "sudo rights acquired. Still no read access to ${GRUB_CFG_PATH} possible."
			return "$ERROR_PERMISSION_DENIED"
		else
			echo "sudo rights acquired, can read grub.cfg"
		fi
	else
		echo "ERROR: grub.cfg does not exist in specified directory" >&2
		return "$ERROR_NO_SUCH_FILE_OR_DIR"
	fi

	### check specified script dir for grub.cfg rebuilds, otherwise extra from grub.cfg
	if [ "" = "${CFG_SCRIPTS_DIR}" ]
	then
		echo "No directory for grub.cfg build scripts specified."
		echo "Trying to extract from grub.cfg comments..."
		# read grub.cfg and find first comment that indicates a generation script, extract full path except file name itself
		CFG_SCRIPTS_DIR=`sudo cat ${GRUB_CFG_PATH} | grep -m 1 "### BEGIN "| sed -n 's/^### BEGIN\o040\(\o057[^\o040]*\o057\).*###$/\1/p'`
		echo "Extracted ${CFG_SCRIPTS_DIR}..."
	fi

	### check path existence
	sudo test -e ${CFG_SCRIPTS_DIR}
	if [ "$?" -eq "0" ]
	then :
	else
		echo "ERROR: specified or extracted directory for grub.cfg generation scripts does not exist" >&2
		return "$ERROR_NO_SUCH_FILE_OR_DIR"
	fi

	echo "Looking for Boot Loader Specification (BLS) entries path"
	echo "(Fedora Linux etc.) :"
	### BootLoaderSpecification conf file dir (only used by Fedora et al. so far)
	if [ "" = "${BLS_CONF_DIR}" ]
	then
		# No BLS config directory argument specified on command-line.
		echo "No BLS config directory argument specified on command-line;"
		BLS_CONF_DIR="/boot/loader/entries"
		echo "Trying standard path at ${BLS_CONF_DIR}"
	fi

	sudo test -e ${BLS_CONF_DIR}
	if [ "$?" -eq "0" ]
	then
		echo "Found BLS entries path at ${BLS_CONF_DIR}"
	else
		echo "No valid BLS entries path found." >&2
		BLS_CONF_DIR=""
	fi
exit
	return 0 # success
}


### UNSET/INIT RELEVANT VARIABLES



### TODO: CHECKING TOOL AVAILABILITY
check_tools_availability

if $TOOLS_ALL_THERE
then
	:
else
	echo
	echo "ERROR: Required tools missing; exiting..." >&2
	echo
	exit "$ERROR_NO_SUCH_COMMAND"
fi


### get sudo
LAST_SUDO_STATE="INACTIVE"
initial_sudo_acquisition
EXIT_ON_FAIL

### check bootfiles availability
if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	exit "$ERROR_NO_SUCH_FILE_OR_DIR"
fi

if [[ -f "../bootfiles/template" ]]
then :
else
	echo "ERROR: ../bootfiles/template not present" >&2
	exit "$ERROR_NO_SUCH_FILE_OR_DIR"
fi



### parse commandline parameters for grub dirs, check existence/access
get_path_arguments $@
check_or_find_paths
EXIT_ON_FAIL


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
	echo -e -n "$fgDEFAULT"
	echo "1 - Extract all menu entries from grub.cfg"
	echo "2 - Configure GRUBswitch order and generate bootfiles and hashes"
	echo "3 -   Remove generated files"
	echo "4 - Write GRUBswitch bootfile to GRUBswitch USB device        (requires sudo)"
	echo -e -n "$fgDEFAULT"
	echo
	echo -e -n "$fgCYAN"
	echo "5 - Install up-to-date hashes for permitted SWITCH.GRB files  (requires sudo)"
	echo "6 -   Remove all hashes, no permission checking               (requires sudo)"
	echo -e -n "$fgDEFAULT"
	echo
	echo -e -n "$fgGREEN"
	echo "7 - Install GRUBswitch into grub.cfg                          (requires sudo)"
	echo "8 -   Remove GRUBswitch from grub.cfg                         (requires sudo)"
	echo -e -n "$fgDEFAULT"
	echo
	echo "q - Quit"
	echo

	GET_KEY

	case ${INPUT} in
			"q")
				clear
				echo "Exited GRUBswitch configuration."
				echo
				exit "$SUCCESS"
				;;
			"1")
				clear
				echo -e -n "${fBOLD}"
				echo "1 - Extract all menu entries from grub.cfg"
				echo "------------------------------------------"
				echo -e -n "${fPLAIN}"
				echo

				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL
				./_1_extract_menuentries.sh -g $GRUB_CFG_DIR
				EXIT_ON_FAIL
				;;
			"2")
				clear
				echo -e -n "$fBOLD"
				echo "2 - Configure and generate bootfiles"	
				echo "------------------------------------"
				echo -e -n "$fPLAIN"
				echo
				./_2_configure_and_generate_bootfiles.sh
				;;
			"3")
				clear
				echo -e -n "${fBOLD}"
				echo "3 -   Remove generated files (bootfiles and hashes)"
				echo "---------------------------------------------------"
				echo -e -n "${fPLAIN}"
				echo
				./_3_remove_generated_files.sh
				;;
			"4")
				clear
				echo -e -n "$fBOLD"
				echo "4 - Write .entries.txt to GRUBswitch USB device"	
				echo "-----------------------------------------------"
				echo -e -n "$fPLAIN"
				echo

				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL
				./_4_write_entries_to_usb_device.sh
				EXIT_ON_FAIL
				;;
			"5")
				clear
				echo -e -n "$fBOLD"
				echo "5 - Install up-to-date hashes"	
				echo "-----------------------------"
				echo -e -n "$fPLAIN"
				echo

				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL
				./_5_install_uptodate_hashes.sh -g $GRUB_CFG_DIR
				EXIT_ON_FAIL
				;;
			"6")
				clear
				echo -e -n "$fBOLD"
				echo "6 - Remove all hashes"	
				echo "---------------------"
				echo -e -n "$fPLAIN"
				echo

				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL
				./_6_remove_all_hashes.sh -g $GRUB_CFG_DIR
				EXIT_ON_FAIL
				;;
			"7")
				clear
				echo -e -n "$fBOLD"
				echo "7 - Install GRUBswitch into grub.cfg"	
				echo "------------------------------------"
				echo -e -n "$fPLAIN"
				echo

				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL
				./_7_install_grubswitch.sh -g $GRUB_CFG_DIR -s $CFG_SCRIPTS_DIR
				EXIT_ON_FAIL
				;;
			"8")
				clear
				echo -e -n "$fBOLD"
				echo "8 - Remove GRUBswitch from grub.cfg"	
				echo "-----------------------------------"
				echo -e -n "$fPLAIN"
				echo

				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL
				./_8_remove_grubswitch.sh -g $GRUB_CFG_DIR -s $CFG_SCRIPTS_DIR
				EXIT_ON_FAIL
				;;
			"9")
				#
				;;
			"*")
				# Whatever.
				;;
	esac

done
