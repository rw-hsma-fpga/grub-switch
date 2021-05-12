#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo "ERROR: Script not started from  shell_scripts  directory" >&2
	echo ; exit; fi

. _shared_objects.sh


#### _4_write_entries_to_usb_device.sh ####
### Script to move generated/edited '.entries.txt' file onto the GRUB Switch USB device

clear
echo -e -n "$fBOLD"
echo "4 - Write .entries.txt to GRUBswitch USB device"	
echo "-----------------------------------------------"
echo -e -n "$fPLAIN"
check_request_sudo

${BOOTFILES_DIR}="../bootfiles/"

### check if entry file availability
if [[ -f "${BOOTFILES_DIR}/.entries.txt" ]]
then :
else
	echo "ERROR: ${BOOTFILES_DIR}/.entries.txt  file missing" >&2
	EXIT_WITH_KEYPRESS
fi

# checking if GRUB Switch is connected
GRUB_SWITCH_UUID_PRESENT=`lsblk -o UUID | grep 1985-1955`
if [[ "$GRUB_SWITCH_UUID_PRESENT" = "1985-1955" ]]
then	:
else
	1>&2 echo -e "ERROR: \e[1mGRUB Switch\e[0m block device not present."
	1>&2 echo -e "       Make sure device is plugged into a working USB slot"
	EXIT_WITH_KEYPRESS
fi




# checking if root; necessary for mounting
# TODO








# mounting, copying, unmounting
echo "mounting GRUB Switch..."
if mkdir -p grub-switch-mount; then :
else 1>&2 echo "ERROR: Creating mount path failed"; return 1; fi

if sudo mount -U 1985-1955 grub-switch-mount; then : 
else 1>&2 echo "ERROR: Mounting failed"; return 1; fi

echo "copying file..."
if sudo cp $ENTRY_FILE_PATH grub-switch-mount/.entries.txt ; then :
else 1>&2 echo "ERROR: copying failed"; return 1; fi

echo "unmounting..."
if sudo umount grub-switch-mount/ ; then :
else 1>&2 echo "ERROR: Unmounting failed"; return 1; fi

if sudo rmdir grub-switch-mount ; then :
else 1>&2 echo "ERROR: Removing mount path failed"; return 1; fi

echo "done."
