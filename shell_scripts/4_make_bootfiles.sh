#!/bin/bash

CURR_DIR=`pwd`
BOOTFILES_DIR="../bootfiles/"

## read .entries.txt file
## divide been parameters for boot message and boot entries
OLD_IFS=$IFS
IFS=$'\t\n'

unset sleep_secs_entries
unset highlight_color_entries
unset boot_entries

sleep_secs_entries[0]=""
highlight_color_entries[0]=""
entry_count=0

while read unstripped_line
do
	# strip DOS line separator \r
	line="${unstripped_line//$'\r'/}"

	if [[ $line =~ ^#.*$ ]]
	then
		if [[ $line =~ ^#1.*$ ]]
		then
			sleep_secs_entries[$entry_count]=`echo "${line}" | sed -n 's/^#1\o040\([^\o040#]*\).*$/\1/p'`
		fi
		if [[ $line =~ ^#2.*$ ]]
		then
			highlight_color_entries[$entry_count]=`echo "${line}" | sed -n 's/^#2\o040\([^\o040#]*\).*$/\1/p'`
		fi
	else
		if [[ -n "${line}" ]]
		then
			boot_entries+=($line)
		else
			boot_entries+=(".")
		fi
		sleep_secs_buf="${sleep_secs_entries[$entry_count]}"
		highlight_color_buf="${highlight_color_entries[$entry_count]}"
		let "entry_count=entry_count+1"
		sleep_secs_entries[${entry_count}]="$sleep_secs_buf"
		highlight_color_entries[${entry_count}]="$highlight_color_buf"
	fi
done < ${BOOTFILES_DIR}/.entries.txt
IFS=$OLD_IFS


echo
echo -n "Number of boot entries:"
echo "${#boot_entries[@]}"
echo
#######################################


## delete old bootfiles
for j in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
	rm -rf ${BOOTFILES_DIR}/boot.${j}
done


## delete old hashes, remake folder
rm -rf ${BOOTFILES_DIR}/grub_switch_hashes
mkdir -p ${BOOTFILES_DIR}/grub_switch_hashes


## make new bootfiles
for (( i = 0 ; i < ${#boot_entries[@]}; i++ ))
do
	let "j=i+1"

	if [ "${boot_entries[$i]}" != "." ]
	then
		# make bootfile folder
		mkdir -p ${BOOTFILES_DIR}/boot.${j}

		# make SWITCH.GRB file
		echo -e "grubswitch_sleep_secs='${sleep_secs_entries[$i]}'\r"       >  ${BOOTFILES_DIR}/boot.${j}/SWITCH.GRB
		echo -e "grubswitch_choice_color='${highlight_color_entries[$i]}'\r"     >> ${BOOTFILES_DIR}/boot.${j}/SWITCH.GRB
		echo -e "grubswitch_choice='${boot_entries[$i]}'\r" >> ${BOOTFILES_DIR}/boot.${j}/SWITCH.GRB
	
		cat ${BOOTFILES_DIR}/template                           >> ${BOOTFILES_DIR}/boot.${j}/SWITCH.GRB


		# make corresponding hash file
		cd ${BOOTFILES_DIR}/boot.${j}/
		sha512sum SWITCH.GRB > ../grub_switch_hashes/${j}.sha512
		cd ${CURR_DIR}
	fi

	# TODO: Adjust ownerships if currently root


	# maximum number of bootfiles is 15,
	# even if more entries created in .entries.txt
	if [[ $j = "15" ]]
	then break
	fi
done

# make hashfile for empty SWITCH.GRB case
rm -f SWITCH.GRB ## TODO: check, rename existing
touch SWITCH.GRB ## new and empty
sha512sum SWITCH.GRB > ${BOOTFILES_DIR}/grub_switch_hashes/0.sha512
rm -f SWITCH.GRB ## remove again

echo
echo "Bootfiles have been generated."
echo "Either"
echo "a) copy SWITCH.GRB files from the enumerated folders in"
echo "   ../bootfiles to USB drives you want to use as boot choices"
echo " OR"
echo "b) write .entries.txt file to the specialized GRUB Switch"
echo "   USB device. You can use script 2_* for this purpose."
echo
echo "If you want to use hash checking to authenticate boot choices,"
echo "1. remove old hashes with script 5_* (run as root)"
echo " AND"
echo "2. install current hashes with script 6_* (also run as root)"
echo
echo "(if you want no hash checking, run script 5_* anyway (as root)"
echo " to remove old hashes)"



### make sure that the owner of all generated files is local user (easier handling)
if [[ root = $USER ]]
then
	local_owner=`ls -ld | awk 'NR==1 {print $3}'` 
	chown -R $local_owner:$local_owner ${BOOTFILES_DIR}
	chmod -R 744 ${BOOTFILES_DIR}
fi
