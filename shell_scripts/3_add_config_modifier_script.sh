#!/bin/bash

### Script to move the update script 99_grub_switch for grub.cfg to correct folder
### (something like /etc/grub.d - will be extracted from existing grub.cfg)

if [[ -n "${1}" ]]
then
	CFG_FILE_PATH=$1
	if [[ -e $CFG_FILE_PATH ]]
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

### BEGIN /path/to/99_file ###
# 0x23: #  (b00100011, \o043)
# 0x2F: /  (b00101111, \o057)
# 0x20:' ' (b00100000, \o040)

# find first comment that indicates a grub.cfg generation script and extract full path except script name itself
SCRIPT_DIR_PATH=`cat ${CFG_FILE_PATH} | grep -m 1 "### BEGIN "| sed -n 's/^### BEGIN\o040\(\o057[^\o040]*\o057\).*###$/\1/p'`
echo $SCRIPT_DIR_PATH

# TODO MUST BE ROOT IN ANY CASE

# install script, make executable
cp ./modifier_script/99_grub_switch $SCRIPT_DIR_PATH/
chmod +x $SCRIPT_DIR_PATH/99_grub_switch

# message to update GRUB
echo "Script installed;"
echo "grub.cfg must be updated manually by"
echo "calling update-grub as root."
echo


