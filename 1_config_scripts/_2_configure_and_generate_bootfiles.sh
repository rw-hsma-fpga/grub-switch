#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi

if [[ "`pwd`" =~ ^.*/1_config_scripts$ ]]; then : ; else
	echo -e "ERROR: Script not started from \e[1m1_config_scripts\e[0m directory" >&2
	echo ; exit 13 ; fi  ## ERROR_PERMISSION_DENIED

. _shared_objects.sh


#### _2_configure_and_generate_bootfiles.sh ####
## Pick and order boot entries for GRUBswitch
## configure choice display
## generate bootfiles and hash files

sleep 2


# path variables
CURR_DIR=`pwd`
BOOTFILES_DIR="../bootfiles/"

### check menu entry list availability
if [[ -e "${BOOTFILES_DIR}/grubmenu_all_entries.lst" ]]
then :
else
	echo "ERROR: ${BOOTFILES_DIR}/grubmenu_all_entries.lst  not present." >&2
	EXIT_WITH_KEYPRESS
fi


# make available color combinations and corresponding grubcolor strings
termcolors=() ; grubcolors=();
termcolors+=( "${fgLIGHTGRAY}${bgBLACK}" ) ;    grubcolors+=( "light-gray/black" );
termcolors+=( "${fgWHITE}${bgBLACK}" ) ;    grubcolors+=( "white/black" );
termcolors+=( "${fgWHITE}${bgRED}" ) ;      grubcolors+=( "white/red" );
termcolors+=( "${fgWHITE}${bgGREEN}" ) ;    grubcolors+=( "white/green" );
termcolors+=( "${fgWHITE}${bgBLUE}" ) ;     grubcolors+=( "white/blue" );
termcolors+=( "${fgWHITE}${bgDARKGRAY}" ) ; grubcolors+=( "white/dark-gray" );

termcolors+=( "${fgRED}${bgBLACK}" ) ;      grubcolors+=( "red/black" );
termcolors+=( "${fgLIGHTYELLOW}${bgBLACK}" ) ;   grubcolors+=( "yellow/black" );
termcolors+=( "${fgLIGHTBLUE}${bgBLACK}" ) ;     grubcolors+=( "light-blue/black" );
termcolors+=( "${fgDARKGRAY}${bgBLACK}" ) ; grubcolors+=( "dark-gray/black" );

termcolors+=( "${fgRED}${bgWHITE}" ) ;      grubcolors+=( "red/white" );
termcolors+=( "${fgBLUE}${bgWHITE}" ) ;     grubcolors+=( "blue/white" );
termcolors+=( "${fgDARKGRAY}${bgWHITE}" ) ; grubcolors+=( "dark-gray/white" );

termcolors+=( "${fgLIGHTYELLOW}${bgRED}" ) ;     grubcolors+=( "yellow/red" );
termcolors+=( "${fgLIGHTYELLOW}${bgBLUE}" ) ;    grubcolors+=( "yellow/blue" );
termcolors+=( "${fgLIGHTYELLOW}${bgDARKGRAY}" ) ;grubcolors+=( "yellow/dark-gray" );



## READ ENTRIES ARRAY FROM grubmenu_all_entries.lst
## (throw out comments)
OLD_IFS=$IFS
IFS=$'\t\n'
unset boot_entries

while read unstripped_line
do
	# strip DOS line separator \r
	line="${unstripped_line//$'\r'/}"

	if [[ $line =~ ^#.*$ ]] # comment line
	then	:
	else
		boot_entries+=($line)
	fi

done < ${BOOTFILES_DIR}/grubmenu_all_entries.lst
IFS=$OLD_IFS


## READ LAST CONFIGURATION FROM .entries.txt IF IT EXISTS
## (throw out comments)
unset lastconfig_entries
lastconfig_entries+=("#") ## setting entry 0 for indexing and so it's not empty
lastconfig_time=3
lastconfig_color=""

### check menu entry list availability
OLD_IFS=$IFS
IFS=$'\t\n'
if [[ -e "${BOOTFILES_DIR}/grubmenu_all_entries.lst" ]]
then
	while read unstripped_line
	do
		# strip DOS line separator \r
		line="${unstripped_line//$'\r'/}"

		if [[ $line =~ ^#1.*$ ]] # wait time setting
		then
			lastconfig_time=$(expr ${line:3} + 0)
			echo "Wait time:"
			echo $lastconfig_time
			echo
		fi

		if [[ $line =~ ^#2.*$ ]] # color setting
		then
			lastconfig_color=${line:3}
			echo "Color setting:"
			echo $lastconfig_color
			echo
		fi

		if [[ $line =~ ^#.*$ ]] # comment line
		then	:
		else
			if [ -z "$line" ]
			then
				lastconfig_entries+=(" ")
			else
				lastconfig_entries+=($line)
			fi
		fi
	done < ${BOOTFILES_DIR}/.entries.txt
fi
IFS=$OLD_IFS

NUM_ENTRIES=${#boot_entries[@]}
NUM_LASTCONFIG_ENTRIES=${#lastconfig_entries[@]}
let "MAX_ENTRY=$NUM_ENTRIES - 1"

echo
echo "There are ${NUM_ENTRIES} entries from 0 to ${MAX_ENTRY}"
echo

## PRINT ENTRIES AND NUMBER - ALSO USING THIS TO MAKE CHOICE ARRAY FOR NOW
unset CHOICEPOS
#echo
#echo "Grub menu entries:"
for printline in "${boot_entries[@]}"
do
	CHOICEPOS+=(".")
	#echo "$printline"
done

# echo
# echo "Last config entries:"
# for printline in "${lastconfig_entries[@]}"
# do
# 	echo "$printline"
# done


## PRE-ASSIGN CHOICES BASED ON LAST CONFIG
for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
do
	for (( j = 0 ; j < ${NUM_LASTCONFIG_ENTRIES}; j++ ))
	do
		if [[ "${boot_entries[$i]}" == "${lastconfig_entries[$j]}" ]]
		then
			if [ $j -lt 10 ]
			then
				CHOICEPOS[$i]="$j"
				#echo $j
			else
				hexchar=$((j+87))
				#echo $hexchar
				CHOICEPOS[$i]="$(printf \\$(printf '%03o' $hexchar))"
			fi
		fi
	done
done
#sleep 8


if [[ ${NUM_ENTRIES} = "0" ]]
then
	clear
	echo "No entries found. Aborting."
	EXIT_WITH_KEYPRESS
fi


### CHOICE CONFIGURATION AND LIST CONFIRMATION LOOPS
REPEAT_LIST_CONFIG=true
while [[ ${REPEAT_LIST_CONFIG} = true ]] ; do

	### MAIN EDIT LOOP
	CURRENT_POS=0
	clear
	while [[ true ]] ; do

		echo -e    "* Use ${fBOLD}Cursor Up / Cursor Down / Pos1 / End${fPLAIN} keys to navigate"
		echo -e    "* Assign a GRUB Switch choice position to an entry"
		echo -e    "  by pressing the keys ${fBOLD}1..9${fPLAIN} or ${fBOLD}a..f${fPLAIN}(=10..15)"
		echo -e    "  [The ${fBOLD}0${fPLAIN} position is reserved for the GRUB Menu]"
		echo -e    "* Press ${fBOLD}Delete${fPLAIN} to remove choice position from current entry"
		echo -e    "* Press ${fBOLD}Backspace${fPLAIN} to clear all positions"
		echo -e    "* Press ${fBOLD}Insert${fPLAIN} to assign all positions in order"
		echo -e    "* Press ${fBOLD}q${fPLAIN} to quit without changes"
		echo -e    "* Press ${fBOLD}Enter${fPLAIN} to continue"
		echo       "-----------------------------------------------------------"
		echo -e    " ${fBOLD}POS${fPLAIN} | ${fBOLD}ENTRY${fPLAIN}"
		echo -e    "-----|-------"

		for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
		do
			echo -e -n " ${fBOLD} ${CHOICEPOS[$i]} ${fPLAIN} | " 
			if [[ $CURRENT_POS = ${i} ]]
			then echo -e -n "${fREVERSE}"
			fi
			CURR_ENTRY=${boot_entries[$i]}
			REPL_STRG="${fBOLD}${fDIM} > ${fUNBOLD}"
			CURR_ENTRY=${CURR_ENTRY//>/${REPL_STRG}}
			echo -e "${CURR_ENTRY}${fPLAIN}" 
		done

		GET_KEY

		case ${INPUT} in
			"Pos1")
				let "CURRENT_POS=0" ;;
			"End")
				let "CURRENT_POS=$MAX_ENTRY" ;;
			"CursorDown")
				let "CURRENT_POS=( $CURRENT_POS + 1 ) % $NUM_ENTRIES" ;;
			"CursorUp")
				let "CURRENT_POS=( $CURRENT_POS + $NUM_ENTRIES - 1 ) % $NUM_ENTRIES" ;;
			"q")
				clear
				echo "Exited without saving."
				EXIT_WITH_KEYPRESS ;;
			"Enter")
				break ;;
			"Delete")
				CHOICEPOS[$CURRENT_POS]="."
				;;
			"Insert")
				CHOICEKEYS=( "1" "2" "3" "4" "5" "6" "7" "8" "9" "a" "b" "c" "d" "e" "f" )
				for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
				do
					CHOICEPOS[$i]=${CHOICEKEYS[$i]}
				done
				;;
			"Backspace")
				# remove all if they were all set in order before
				for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
				do
					CHOICEPOS[$i]="."
				done
				;;
			[a-f1-9])
				for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
				do
					if [[ ${CHOICEPOS[$i]} = ${INPUT} ]]
					then CHOICEPOS[$i]="."
					fi
				done
				CHOICEPOS[$CURRENT_POS]=${INPUT}
				;;
		esac
		clear
	done # while - CHOICE CONFIGURATION

	clear
	### CHOICE-ORDER ENTRY OUTPUT TO CONFIRM
	while [[ true ]] ; do
		echo "Switch positions chosen:"
		echo "------------------------"
		echo "0 : GRUB Menu (Fixed)"
		for j in 1 2 3 4 5 6 7 8 9 a b c d e f
		do
			CHOICE_ENTRY=""
			for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
			do
				if [[ ${CHOICEPOS[$i]} = $j ]]
				then
					CHOICE_ENTRY=${boot_entries[$i]}
				fi
			done
			echo $j ":" $CHOICE_ENTRY
		done
		echo       "--------------------------------------------"
		echo -e    "* Press ${fBOLD}Enter${fPLAIN} to confirm selection"
		echo -e    "* Press ${fBOLD}Backspace <-${fPLAIN} to change configuration"
		echo -e    "* Press ${fBOLD}q${fPLAIN} to quit without changes"

		GET_KEY

		case ${INPUT} in
				"q")
					clear
					echo "Exited without saving."
					EXIT_WITH_KEYPRESS ;;
				"Enter")
					REPEAT_LIST_CONFIG=false
					break ;;
				"Backspace")
					REPEAT_LIST_CONFIG=true
					break ;;
		esac
		clear
	done # while - LIST CONFIRMATION

done # while - CHOICE LIST CONFIGURATION AND LIST CONFIRMATION


### DISPLAY SECONDS AND COLOR CONFIGURATION LOOPS
REPEAT_DISPLAY_CONFIG=true

SECS=$lastconfig_time

colorindex=0
NUM_COLORS=${#grubcolors[@]}
for (( i = 0 ; i < ${NUM_COLORS}; i++ ))
do
	if [[ "${lastconfig_color}" == "${grubcolors[$i]}" ]]
	then
		colorindex=$i
	fi
done

while [[ ${REPEAT_DISPLAY_CONFIG} = true ]] ; do

	clear
	while [[ true ]]  ## SETTING DISPLAY SECONDS
	do
		echo       "--------------------------------------------------------"
		echo       "Set boot choice display time:"

		echo -e    "* Press ${fBOLD}Cursor Up / Cursor Down${fPLAIN} to change +/- 10 seconds"
		echo -e    "* Press ${fBOLD}Cursor Right/ Cursor Left${fPLAIN} to change +/- 1 second"

		echo -e    "* Press ${fBOLD}Enter${fPLAIN} to confirm selection"
		echo -e    "* Press ${fBOLD}q${fPLAIN} to quit without changes"
		echo       "--------------------------------------------------------"
		echo

		echo -e -n "Show boot choice for ${fBOLD}"
		printf %03d $SECS
		echo -e    "${fPLAIN} seconds"

		GET_KEY

		case ${INPUT} in
				"q")
					clear
					echo "Exited without saving."
					EXIT_WITH_KEYPRESS ;;
				"Enter")
					#REPEAT_DISPLAY_CONFIG=false
					break ;;

				"CursorUp")
					let "SECS=( $SECS + 10 )"
					[[ $SECS -gt 999 ]] && SECS=999
					;;

				"CursorDown")
					let "SECS=( $SECS - 10 )"
					[[ $SECS -lt 0 ]] && SECS=0
					;;

				"CursorLeft")
					let "SECS=( $SECS - 1 )"
					[[ $SECS -lt 0 ]] && SECS=0
					;;

				"CursorRight")
					let "SECS=( $SECS + 1 )"
					[[ $SECS -gt 999 ]] && SECS=999
					;;

				"Delete")
					let "SECS=0"
					;;
		esac
		clear

	done ## END WHILE - DISPLAY SECONDS


	if [[ $SECS -eq 0 ]] # Display color choice only presented when display is shown (>=1secs)
	then
		REPEAT_DISPLAY_CONFIG=false
	else
		echo -e "${fgDEFAULT}${bgDEFAULT}${fPLAIN}"
		clear
		while [[ true ]]  ## SETTING BOOT CHOICE HIGHLIGHT COLOR
		do
			echo       "-------------------------------------------------------"
			echo       "Choose highlight colors for boot choice display:"

			echo -e    "* Press ${fBOLD}Cursor Right/ Cursor Left${fPLAIN} to change"

			echo -e    "* Press ${fBOLD}Enter${fPLAIN} to confirm selection"
			echo -e    "* Press ${fBOLD}Backspace <-${fPLAIN} to go back and change display time"
			echo -e    "* Press ${fBOLD}q${fPLAIN} to quit without changes"
			echo       "-------------------------------------------------------"
			echo

			echo -e -n "Current choice is ${fBOLD}${grubcolors[$colorindex]}"
			if [[ ${colorindex} -eq 0 ]]
			then
				echo -e -n " (default)"
			fi
			echo -e "${fPLAIN}."
			echo -e "See example below:"
			echo -e "${fgLIGHTGRAY}${bgBLACK}"
			echo 
			echo    -n "   Booting "
			echo -e -n "${termcolors[$colorindex]}"
			echo    -n "FluxCapOS 1.21"
			echo -e    "${fgLIGHTGRAY}${bgBLACK}"
			echo    -n "   continues in "
			printf %03d $SECS
			echo       " seconds"
			echo
			echo -e    "${fgDEFAULT}${bgDEFAULT}${fPLAIN}"
			echo -e		"(Caution: Not all Linux terminals show the"
			echo -e		" exact range of colors that GRUB supports)"
			echo

			GET_KEY

			case ${INPUT} in
					"q")
						clear
						echo "Exited without saving."
						EXIT_WITH_KEYPRESS ;;
					"Enter")
						REPEAT_DISPLAY_CONFIG=false
						break ;;
					"Backspace")
						REPEAT_DISPLAY_CONFIG=true
						break ;;
					"CursorLeft")
						[[ $colorindex -eq 0 ]] && colorindex=${#termcolors[@]} 
						let "colorindex=( $colorindex - 1 )"
						;;
					"CursorRight")
						let "colorindex=( $colorindex + 1 )"
						[[ $colorindex -eq ${#termcolors[@]} ]] && colorindex=0
						;;
			esac
			clear

		done ## END WHILE - HIGHLIGHT COLOR
	fi

done # while - DISPLAY SECONDS AND COLOR CONFIGURATION


## FILE OUTPUT BASED ON CONFIGURATION
clear

cd ${BOOTFILES_DIR}
rm -Rf ./boot.*
rm -Rf ./grub_switch_hashes/
mkdir ./grub_switch_hashes


#### TODO: change to reflect different times and colors for entries
# create and fill .entries.txt (for use with custom grubswitch device)

echo -n "#1 " > ./.entries.txt
	printf %03d $SECS >> ./.entries.txt
echo >> ./.entries.txt
echo -n "#2 " >> ./.entries.txt
	echo "${grubcolors[$colorindex]}" >> ./.entries.txt

for j in 1 2 3 4 5 6 7 8 9 a b c d e f
do
	# find choice entry for position
	CHOICE_ENTRY=""
	for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
	do
		if [[ ${CHOICEPOS[$i]} = $j ]]
		then
			CHOICE_ENTRY=${boot_entries[$i]}
		fi
	done

	# add to .entries.txt
	echo $CHOICE_ENTRY >> ./.entries.txt


	#### TODO: change to reflect different times and colors for entries
	# create individual boot files (for use with regular USB drives)
	if [[ -n ${CHOICE_ENTRY} && -f "./template" ]]
	then
		mkdir ./boot.${j}

		echo -n "grubswitch_sleep_secs='" >> ./boot.${j}/SWITCH.GRB
			printf %03d $SECS >> ./boot.${j}/SWITCH.GRB
			echo "'" >> ./boot.${j}/SWITCH.GRB

		echo -n "grubswitch_choice_color='" >> ./boot.${j}/SWITCH.GRB
			echo -n "${grubcolors[$colorindex]}" >> ./boot.${j}/SWITCH.GRB
			echo "'" >> ./boot.${j}/SWITCH.GRB

		echo -n "grubswitch_choice='" >> ./boot.${j}/SWITCH.GRB
			echo -n "${CHOICE_ENTRY}" >> ./boot.${j}/SWITCH.GRB
			echo "'" >> ./boot.${j}/SWITCH.GRB

		cat ./template >> ./boot.${j}/SWITCH.GRB

		# create hash
		cd boot.${j}
		sha512sum SWITCH.GRB > ../grub_switch_hashes/${j}.sha512
		cd ..
	fi
done

echo "### These comments may be cut off (without harm) if the file gets too large" >> ${BOOTFILES_DIR}/.entries.txt
echo "### (ATmega32u4 has 960 bytes of available space, ATmega16u4 has only 448 bytes)" >> ${BOOTFILES_DIR}/.entries.txt
echo "###" >> ${BOOTFILES_DIR}/.entries.txt
echo "### #1 specifies display time for subsequent entries" >> ${BOOTFILES_DIR}/.entries.txt
echo "### #2 specifies display colors for subsequent entries" >> ${BOOTFILES_DIR}/.entries.txt
echo "###" >> ${BOOTFILES_DIR}/.entries.txt
echo "### Uncommented lines above are GRUB switch choices 1..15 (0x1..0xF)" >> ${BOOTFILES_DIR}/.entries.txt
echo "### An empty line leads to the GRUB menu, as does choice 0" >> ${BOOTFILES_DIR}/.entries.txt



cd ${CURR_DIR}


### print generated '.entries.txt' file content
echo -e "The following data was written to ${fBOLD}${BOOTFILES_DIR}.entries.txt${fPLAIN}:"
echo
cat ${BOOTFILES_DIR}/.entries.txt
echo
echo "--------------------------------------------------------"

EXIT_WITH_KEYPRESS
