#!/bin/bash

### Script to move generated/edited '.entries.txt' file onto the GRUB Switch USB device

${BOOTFILES_DIR}="../bootfiles/"

# checking if GRUB Switch is connected
GRUB_SWITCH_UUID_PRESENT=`lsblk -o UUID | grep 1985-1955`
if [[ "$GRUB_SWITCH_UUID_PRESENT" = "1985-1955" ]]
then	:
else
	1>&2 echo -e "ERROR: \e[1mGRUB Switch\e[0m block device not present."
	1>&2 echo -e "       Make sure device is plugged into a working USB slot"
	return 1
fi

# checking if root; necessary for mounting
if [[ root = $USER ]]
then	:
else
	1>&2 echo -e "ERROR: Script needs to be run as\e[1m root\e[0m (current user is\e[1m ${USER}\e[0m)"
	1>&2 echo -e "       to be able to mount USB device. Start a root shell with\e[1m sudo bash\e[0m"
	1>&2 echo -e "       and try running the script again"
	return 1
fi


# specifying, finding .entries.txt file, checking read access
if [[ -n "${1}" ]]
then
	ENTRY_FILE_PATH=$1
	if [[ -e $ENTRY_FILE_PATH ]]
	then	:
	else
		1>&2 echo -e "ERROR: File \e[1m${ENTRY_FILE_PATH}\e[0m not found"
		return 1
	fi
else
	ENTRY_FILE_PATH=""
	# check default locations
	if [[ -e '${BOOTFILES_DIR}/.entries.txt' ]] # TODO check if single quotes OK
	then
		ENTRY_FILE_PATH="${BOOTFILES_DIR}/.entries.txt"
	else
		1>&2 echo -e "ERROR: No\e[1m .entries.txt\e[0m specified or found locally"
		return 1
	fi
fi


if [[ -r $ENTRY_FILE_PATH ]]
then	:
else
	1>&2 echo -n -e "ERROR: \e[1m${ENTRY_FILE_PATH}\e[0m is not readable by user \e[1m"
	1>&2 whoami
	1>&2 echo -n -e "\e[0m"
	1>&2 echo       "       Try entering root shell with 'sudo bash'"
	1>&2 echo       "       and then execute script again"
	return 1
fi



# mounting, copying, unmounting
echo "mounting GRUB Switch..."
if mkdir -p grub-switch-mount; then :
else 1>&2 echo "ERROR: Creating mount path failed"; return 1; fi

if mount -U 1985-1955 grub-switch-mount; then : 
else 1>&2 echo "ERROR: Mounting failed"; return 1; fi

echo "copying file..."
if cp $ENTRY_FILE_PATH grub-switch-mount/.entries.txt ; then :
else 1>&2 echo "ERROR: copying failed"; return 1; fi

echo "unmounting..."
if umount grub-switch-mount/ ; then :
else 1>&2 echo "ERROR: Unmounting failed"; return 1; fi

if rmdir grub-switch-mount ; then :
else 1>&2 echo "ERROR: Removing mount path failed"; return 1; fi

echo "done."
