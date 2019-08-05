#!/bin/sh

### Script to extract all menuentry titles from 'grub.cfg', write them to '.entries.txt' file
### Default parameters for choice display time and choice highlight color will also be added

if [ -n "${1}" ]
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
	if [ -e '/boot/grub2/grub.cfg' ]
	then
		CFG_FILE_PATH="/boot/grub2/grub.cfg"
	fi
	if [ -e '/boot/grub/grub.cfg' ]
	then
		CFG_FILE_PATH="/boot/grub/grub.cfg"
	fi

	if [ -z $CFG_FILE_PATH ]
	then
		1>&2 echo -e "ERROR: No\e[1m grub.cfg\e[0m specified or found in default locations"
		return 1
	fi
fi


if [ -r $CFG_FILE_PATH ]
then	:
else
	1>&2 echo -n -e "ERROR: \e[1m${CFG_FILE_PATH}\e[0m is not readable by user \e[1m"
	1>&2 whoami
	1>&2 echo -n -e "\e[0m"
	1>&2 echo       "       Try entering root shell with 'sudo bash'"
	1>&2 echo       "       and then execute script again"
	return 1
fi

### printing default options and comments into '.entries.txt'
echo "### configuration parameters" > .entries.txt
echo "#1 003        # seconds to display boot choice" >> .entries.txt
echo "#2 white/blue # highlight color that is used for the boot choice" >> .entries.txt
echo "### GRUB switch choices 1..15 (0x1..0xF): an empty, non-comment line" >> .entries.txt
echo "### means that choice leads to the regular GRUB menu, as does 0" >> .entries.txt

### The real magic: extracting all menuentry titles with a regular expression:
###    zero or more tabs at the beginning of the line
###    the word "menuentry", followed by a space and an apostrophe
###     the title to be extracted: multiple characters, anything but an apostrophe
###    an apostrophe and a space, followed by anything until the end of the line
### Results written one each per line into .entries.txt
### Note: '.entries.txt' is hidden and only visible with 'ls -a' option

cat ${CFG_FILE_PATH} | sed -n 's/^\o011*menuentry\o040\o047\([^\o047]*\)\o047\o040.*$/\1/p' >> .entries.txt


### make sure that the owner of .entries.txt is local user (should be able to edit)
if [ root = $USER ]
then
	local_owner=`ls -ld | awk 'NR==1 {print $3}'` 
	chown $local_owner:$local_owner .entries.txt
	chmod 644 .entries.txt
fi

