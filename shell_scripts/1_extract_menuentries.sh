#!/bin/bash

### Script to extract all menuentry titles from 'grub.cfg', write them to '.entries.txt' file
### Default parameters for choice display time and choice highlight color will also be added

CURR_DIR=`pwd`
BOOTFILES_DIR="../bootfiles/"

if [[ -n "${1}" ]]
then
	CFG_FILE_PATH=$1
	if [ -e $CFG_FILE_PATH ]
	then	:
	else
		1>&2 echo -e "ERROR: \e[1m${CFG_FILE_PATH}\e[0m is not a file"
		return 1
	fi
else
	CFG_FILE_PATH=""
	# check default locations
	if [[ -e '/boot/grub2/grub.cfg' ]]
	then
		CFG_FILE_PATH="/boot/grub2/grub.cfg"
	fi
	if [[ -e '/boot/grub/grub.cfg' ]]
	then
		CFG_FILE_PATH="/boot/grub/grub.cfg"
	fi

	if [[ -z $CFG_FILE_PATH ]]
	then
		1>&2 echo -e "ERROR: No\e[1m grub.cfg\e[0m specified or found in default locations"
		return 1
	fi
fi


if [[ -r $CFG_FILE_PATH ]]
then	:
else
	1>&2 echo -n -e "ERROR: \e[1m${CFG_FILE_PATH}\e[0m is not readable by user \e[1m"
	1>&2 whoami
	1>&2 echo -n -e "\e[0m"
	1>&2 echo       "       Try entering root shell with 'sudo bash'"
	1>&2 echo       "       and then execute script again"
	return 1
fi


### printing default options and comments into '.entries.txt.all' und 'grubmenu_all_entries.lst'
echo -e "writing initial parameters to\e[1m .entries.txt.all\e[0m ..."
echo "### #1 specifies display time for subsequent entries" > ${BOOTFILES_DIR}/.entries.txt.all
echo "### #2 specifies display colors for subsequent entries" > ${BOOTFILES_DIR}/.entries.txt.all
echo "### Uncommented lines are GRUB switch choices 1..15 (0x1..0xF)" >> ${BOOTFILES_DIR}/.entries.txt.all
echo "### An empty line leads to the GRUB menu, as does choice 0" >> ${BOOTFILES_DIR}/.entries.txt.all
echo "#1 005" >> ${BOOTFILES_DIR}/.entries.txt.all
echo "#2 white/blue" >> ${BOOTFILES_DIR}.entries.txt.all

### printing title comment into grubmenu_all_entries.lst
echo "### All entries, extracted from current GRUB menu" > ${BOOTFILES_DIR}/grubmenu_all_entries.lst



### read config file
### find menuentry, submenu and closing brace '}' lines
OLD_IFS=$IFS
IFS=$'\t\n'
unset boot_entries
menuentry_open=false
submenu_level=0

# The real magic is extracting all submenu and menuentry titles with regular expressions:
#    zero or more tabs at the beginning of the line
#    the word "submenu" or "menuentry", followed by a space and an apostrophe
#     the title to be extracted: multiple characters, anything but an apostrophe
#    an apostrophe and a space, followed by anything until the end of the line
# Results written one each per line into .entries.txt with submenu hierarchy>
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
		echo $entry_hierarchy >> ${BOOTFILES_DIR}/.entries.txt.all
		echo $entry_hierarchy >> ${BOOTFILES_DIR}/grubmenu_all_entries.lst
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

done < ${CFG_FILE_PATH}
IFS=$OLD_IFS




### make sure that the owner of entry files is local user (should be able to edit)
### FIX: Got to BOOTFILES_DIR/ for ls user extraction
if [ root = $USER ]
then
	cd ${BOOTFILES_DIR}
	local_owner=`ls -ld | awk 'NR==1 {print $3}'` 
	chown $local_owner:$local_owner .entries.txt.all
	chown $local_owner:$local_owner grubmenu_all_entries.lst
	chmod 644 .entries.txt.all
	cd ${CURR_DIR}
fi
