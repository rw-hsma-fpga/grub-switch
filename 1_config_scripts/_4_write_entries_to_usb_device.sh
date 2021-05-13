#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit; fi

. _shared_objects.sh


#### _4_write_entries_to_usb_device.sh ####
### Script to move generated/edited '.entries.txt' file onto the GRUB Switch USB device

clear
echo -e -n "$fBOLD"
echo "4 - Write .entries.txt to GRUBswitch USB device"	
echo "-----------------------------------------------"
echo -e -n "$fPLAIN"

BOOTFILES_DIR="../bootfiles/"

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
	1>&2 echo -e "ERROR: \e[1mGRUBswitch\e[0m block device not present."
	1>&2 echo -e "       Make sure device is plugged into a working USB slot"
	EXIT_WITH_KEYPRESS
fi


check_request_sudo

# checking if root; necessary for mounting
if [ "${LAST_SUDO_STATE}" = "INACTIVE" ]
then
	echo "ERROR: Did not acquire sudo access necessary for mount/unmount"
	EXIT_WITH_KEYPRESS
fi



# unmount if mounted, mount, copying, unmount
sudo umount /dev/disk/by-uuid/1985-1955 2> /dev/null

if mkdir -p ${BOOTFILES_DIR}/grub-switch-mount
then
	echo -e "Creating mount path ${BOOTFILES_DIR}/grub-switch-mount/"	
else
	echo "ERROR: Creating mount path failed"
	EXIT_WITH_KEYPRESS
fi

if sudo mount -U "1985-1955" ${BOOTFILES_DIR}/grub-switch-mount
then
	echo "Mounted GRUBswitch USB device to ${BOOTFILES_DIR}/grub-switch-mount/"
else
	echo "ERROR: Mounting GRUBswitch USB device failed"
	EXIT_WITH_KEYPRESS
fi

if sudo cp "${BOOTFILES_DIR}/.entries.txt" "${BOOTFILES_DIR}/grub-switch-mount/.entries.txt"
then
	echo -e "Copied \e[1m.entries.txt\e[0m to USB device"
else
	echo -e "ERROR: Copying \e[1m.entries.txt\e[0m to USB device failed"
	EXIT_WITH_KEYPRESS
fi

if sudo umount ${BOOTFILES_DIR}/grub-switch-mount/
then
	echo "Unmounted device"
else
	echo "ERROR: Unmounting device failed"
	EXIT_WITH_KEYPRESS
fi

if sudo rmdir ${BOOTFILES_DIR}/grub-switch-mount/
then
	echo -e "Removed mount path ${BOOTFILES_DIR}/grub-switch-mount/"
else
	echo -e "ERROR: Removing mount path failed"
	EXIT_WITH_KEYPRESS
fi

EXIT_WITH_KEYPRESS
