#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi


### Script to extract all menuentry titles from 'grub.cfg', write them to '.entries.txt' file
### Default parameters for choice display time and choice highlight color will also be added


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

clear
echo -e -n "${fBOLD}"
echo "1 - Extract all menu entries from grub.cfg"
echo "------------------------------------------"
echo -e -n "${fPLAIN}"
echo

SUB_1_BOOTFILES_DIR="../bootfiles/"
SUB_1_GRUB_CFG_DATA=""
SUB_1_GRUB_CFG_PATH=""

### parse commandline parameters for grub dirs, check existence/access
OPTIND=1
while getopts ":g:" callarg
do
	case ${callarg} in
		g)
			SUB_1_GRUB_CFG_DIR=${OPTARG}
			#echo "grub.cfg dir specified as ${GRUB_CFG_DIR}"
			;;
		*)
			;;
	esac
done


### check grub.cfg existence and readability, acquire sudo if required
SUB_1_GRUB_CFG_PATH="${SUB_1_GRUB_CFG_DIR}/grub.cfg"
if [[ -e "${SUB_1_GRUB_CFG_PATH}" ]]
then
	if [[ -r "${SUB_1_GRUB_CFG_PATH}" ]]
	then
		echo "grub.cfg exists and is readable."
		SUB_1_GRUB_CFG_DATA=`cat "${SUB_1_GRUB_CFG_PATH}"`
	else
		# not readable, so try to get sudo
		echo "grub.cfg exists but sudo rights required to read:"
		check_request_sudo
		if [ "${last_sudo_state}" = "INACTIVE" ]
		then
			echo "ERROR: Failed getting sudo access to grub.cfg" >&2
		else
			sudo test -r ${SUB_1_GRUB_CFG_PATH}
			if [ "$?" -ne "0" ]
			then
				echo "sudo rights acquired. Still no read access to ${SUB_1_GRUB_CFG_PATH} possible."
			else
				echo "sudo rights acquired, can read grub.cfg"
				SUB_1_GRUB_CFG_DATA=`sudo cat "${SUB_1_GRUB_CFG_PATH}"`
			fi
		fi
	fi
else
	echo "ERROR: grub.cfg does not exist in specified directory" >&2
fi


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



if [ -z "$SUB_1_GRUB_CFG_DATA" ]
then
	echo "reading in grub.cfg did not succeed"
else

	### printing default options and comments into '.entries.txt.all' und 'grubmenu_all_entries.lst'
	echo "### #1 specifies display time for subsequent entries" > ${SUB_1_BOOTFILES_DIR}/.entries.txt.all
	echo "### #2 specifies display colors for subsequent entries" > ${SUB_1_BOOTFILES_DIR}/.entries.txt.all
	echo "### Uncommented lines are GRUB switch choices 1..15 (0x1..0xF)" >> ${SUB_1_BOOTFILES_DIR}/.entries.txt.all
	echo "### An empty line leads to the GRUB menu, as does choice 0" >> ${SUB_1_BOOTFILES_DIR}/.entries.txt.all
	echo "#1 005" >> ${SUB_1_BOOTFILES_DIR}/.entries.txt.all
	echo "#2 white/blue" >> ${SUB_1_BOOTFILES_DIR}.entries.txt.all

	### printing title comment into grubmenu_all_entries.lst
	echo "### All entries, extracted from current GRUB menu" > ${SUB_1_BOOTFILES_DIR}/grubmenu_all_entries.lst



	### find menuentry, submenu and closing brace '}' lines
	OLD_IFS=$IFS
	IFS=$'\t\n'
	menuentry_open=false
	submenu_level=0

	# The real magic is extracting all submenu and menuentry titles with regular expressions:
	#    zero or more tabs at the beginning of the line
	#    the word "submenu" or "menuentry", followed by a space and an apostrophe
	#     the title to be extracted: multiple characters, anything but an apostrophe
	#    an apostrophe and a space, followed by anything until the end of the line
	# Results written one each per line into .entries.txt.all with submenu hierarchy>
	# Note: '.entries.txt.all' is hidden and only visible with 'ls -a' option
	while read line
	do
		# menuentry
		if [[ $line =~ ^\t*menuentry.*$ ]]
		then
			entry_hierarchy=""
			#echo $line
			menuentry_open=true
			menuentry=`echo ${line} | sed -n 's/^\o011*menuentry\o040\o047\([^\o047]*\)\o047\o040.*$/\1/p'`

			for (( i=1 ; i<=${submenu_level} ; i++ ))
			do
				entry_hierarchy="${entry_hierarchy}${submenu[${i}]}>"
			done

			entry_hierarchy="${entry_hierarchy}${menuentry}"
			echo $entry_hierarchy >> ${SUB_1_BOOTFILES_DIR}/.entries.txt.all
			echo $entry_hierarchy >> ${SUB_1_BOOTFILES_DIR}/grubmenu_all_entries.lst
		fi

		# submenu
		if [[ $line =~ ^\t*submenu.*$ ]]
		then
			#echo $line
			let submenu_level++
			submenu[${submenu_level}]=`echo ${line} | sed -n 's/^\o011*submenu\o040\o047\([^\o047]*\)\o047\o040.*$/\1/p'`
			
		fi

		# }
		if [[ $line =~ ^\t*}.*$ ]]
		then
			if [[ true = $menuentry_open ]]
			then
				menuentry_open=false
				#echo $line
			else
				if [[ "${submenu_level}" -gt "0" ]]
				then
					let submenu_level--
					#echo $line
				fi
			fi
		fi

	done < <(echo "$SUB_1_GRUB_CFG_DATA")
	IFS=$OLD_IFS

	echo
	echo -e "Wrote all menu entries to \e[1m${SUB_1_BOOTFILES_DIR}/.entries.txt.all\e[0m"
	echo -e "and \e[1m${SUB_1_BOOTFILES_DIR}/grubmenu_all_entries.lst\e[0m ..."
fi

echo
echo "Press any key to return to main menu."
echo
GET_ANY_KEYPRESS
