#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit 13 ; fi  ## ERROR_PERMISSION_DENIED

. _shared_objects.sh


#### _1_extract_menuentries.sh ###
## Script to extract all menuentry titles from 'grub.cfg' and write them to
## 'grubmenu_all_entries.lst' and '.entries.txt.all' files


BOOTFILES_DIR="../bootfiles/"


### parse commandline parameters for grub dirs, check existence/access
get_path_arguments $@

### check grub.cfg existence and readability
check_sudo_reacquire_or_exit
EXIT_ON_FAIL

# optional update-grub
echo -e "Before extracting GRUB menu entries, we recommend re-generating the"
echo -e "${fBOLD}grub.cfg${fPLAIN} file by calling ${fBOLD}update-grub${fPLAIN}. This way, you can be sure the"
echo -e "extracted list of entries is up-to-date. However, this is optional."
echo -e "Do you want to re-generate grub.cfg?  (${fBOLD}y${fPLAIN})es / (${fBOLD}n${fPLAIN})o"

### find tool path
find_update_grub_tool

while [[ true ]]
do
	GET_KEY
	case ${INPUT} in
			"y")
				check_sudo_reacquire_or_exit
				EXIT_ON_FAIL

				echo
				run_update_grub_tool

				echo ; echo -e "${fBOLD}grub.cfg${fPLAIN} re-generated." ; echo
				sleep 1
				break
				;;
			"n")
				echo ; echo "Re-generation skipped." ; echo
				sleep 1
				break
				;;
			"*")
				;;
	esac
done

check_sudo_reacquire_or_exit
EXIT_ON_FAIL

GRUB_CFG_PATH="${GRUB_CFG_DIR}/grub.cfg"
sudo test -e ${GRUB_CFG_PATH}
if [ "$?" -eq "0" ]
then
	sudo test -r ${GRUB_CFG_PATH}
	if [ "$?" -eq "0" ]
	then
		echo "grub.cfg exists and is readable."
		GRUB_CFG_DATA=`sudo cat "${GRUB_CFG_PATH}"`
	else
		echo "No read access to ${GRUB_CFG_PATH} possible despite sudo."
		EXIT_WITH_KEYPRESS
	fi
else
	echo "ERROR: grub.cfg does not exist in specified directory" >&2
	EXIT_WITH_KEYPRESS
fi


### check bootfiles availability
if [[ -e "../bootfiles/" ]]
then :
else
	echo "ERROR: ../bootfiles/ path not present" >&2
	EXIT_WITH_KEYPRESS
fi



### printing title comment into grubmenu_all_entries.lst
echo "### All entries, extracted from current GRUB menu" > ${BOOTFILES_DIR}/grubmenu_all_entries.lst

# BLS menu entries first
if [ "" != "${BLS_CONF_DIR}" ]
then
	echo "Checking Boot Level Specification entries..."
	OLD_IFS=$IFS
	IFS=$'\t\n'

	BLS_CONF_LS=($(sudo ls "${BLS_CONF_DIR}" | sort -r))
	for i in ${BLS_CONF_LS[@]}
	do
		if [[ "$i" =~ ^.*conf$ ]]
		then

			BLS_CONF_FILE="${BLS_CONF_DIR}/$i"
			BLS_CONF_DATA=`sudo cat "${BLS_CONF_FILE}"`
			while read line
			do
				# title
				if [[ $line =~ ^.*title.*$ ]]
				then
					echo ${line} | sed -n  's/^[\o040\o011]*title[\o040\o011]\+\([^\o011]*\)*$/\1/p' >> ${BOOTFILES_DIR}/grubmenu_all_entries.lst
				fi
			done < <(echo "$BLS_CONF_DATA")
		fi
	done

	IFS=$OLD_IFS
fi


if [ -z "$GRUB_CFG_DATA" ]
then
	echo "reading in grub.cfg did not succeed"
else
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
	# Results written one each per line into grubmenu_all_entries.lst with submenu hierarchy>
	while read line
	do
		# menuentry
		if [[ $line =~ ^\t*menuentry.*$ ]]
		then
			entry_hierarchy=""
			menuentry_open=true
			# RegEx fixed, now works with ' and " - thanks to Maxime Gado
			menuentry=`echo ${line} | sed -n 's/^\o011*menuentry\o040[\o042\o047]\([^\o042^\o047]*\)[\o042\o047].*$/\1/p'`

			for (( i=1 ; i<=${submenu_level} ; i++ ))
			do
				entry_hierarchy="${entry_hierarchy}${submenu[${i}]}>"
			done

			entry_hierarchy="${entry_hierarchy}${menuentry}"
			echo $entry_hierarchy >> ${BOOTFILES_DIR}/grubmenu_all_entries.lst
		fi

		# submenu
		if [[ $line =~ ^\t*submenu.*$ ]]
		then
			let submenu_level++
			# RegEx fixed, now works with ' and " - thanks to Maxime Gado
			submenu[${submenu_level}]=`echo ${line} | sed -n 's/^\o011*submenu\o040[\o042\o047]\([^\o042^\o047]*\)[\o042\o047].*$/\1/p'`
			
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

	done < <(echo "$GRUB_CFG_DATA")
	IFS=$OLD_IFS
fi

echo
echo -e "Wrote all menu entries to ${fBOLD}${BOOTFILES_DIR}grubmenu_all_entries.lst${fPLAIN} ..."

EXIT_WITH_KEYPRESS
