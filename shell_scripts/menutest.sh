#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi


# path variables
CURR_DIR=`pwd`
BOOTFILES_DIR="../bootfiles/"
#TODO: Check existence of bootfiles folder, template

# terminal format macros
fPLAIN="\e[0m"
fBOLD="\e[1m"
fUNBOLD="\e[22m"
fDIM="\e[2m"
fUNDIM="\e[22m"
fREVERSE="\e[7m"
# color format macros
fgDEFAULT="\e[39m" ; bgDEFAULT="\e[49m"
fgBLACK="\e[30m" ; fgRED="\e[31m" ; fgGREEN="\e[32m" ; fgYELLOW="\e[33m"
fgBLUE="\e[34m" ; fgMAGENTA="\e[35m" ; fgCYAN="\e[36m" ; fgLIGHTGRAY="\e[37m"
fgDARKGRAY="\e[90m" ; fgLIGHTRED="\e[91m" ; fgLIGHTGREEN="\e[92m" ; fgLIGHTYELLOW="\e[93m"
fgLIGHTBLUE="\e[94m" ; fgLIGHTMAGENTA="\e[95m" ; fgLIGHTCYAN="\e[96m" ; fgWHITE="\e[97m"
bgBLACK="\e[40m" ; bgRED="\e[41m" ; bgGREEN="\e[42m" ; bgYELLOW="\e[43m"
bgBLUE="\e[44m" ; bgMAGENTA="\e[45m" ; bgCYAN="\e[46m" ; bgLIGHTGRAY="\e[47m"
bgDARKGRAY="\e[100m" ; bgLIGHTRED="\e[101m" ; bgLIGHTGREEN="\e[102m" ; bgLIGHTYELLOW="\e[103m"
bgLIGHTBLUE="\e[104m" ; bgLIGHTMAGENTA="\e[105m" ; bgLIGHTCYAN="\e[106m" ; bgWHITE="\e[107m"

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
#termcolors+=( "${fgLIGHTYELLOW}${bgGREEN}" ) ;   grubcolors+=( "yellow/green" );
termcolors+=( "${fgLIGHTYELLOW}${bgBLUE}" ) ;    grubcolors+=( "yellow/blue" );
termcolors+=( "${fgLIGHTYELLOW}${bgDARKGRAY}" ) ;grubcolors+=( "yellow/dark-gray" );

#termcolors+=( "${fgBLUE}${bgLIGHTYELLOW}" ) ;    grubcolors+=( "blue/yellow" );
#termcolors+=( "${fgDARKGRAY}${bgLIGHTYELLOW}" ) ;grubcolors+=( "dark-gray/yellow" );



# keyboard polling function... puts key name in $INPUT; empty if no interesting key
function GET_KEYPRESS () {

	OLD_IFS=$IFS
	IFS=''

	INPUT=""
	read -s -N 1 KEY
	case $KEY in
	[1-9])
		INPUT=$KEY
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	[a-fqA-F])
		KEY=`echo ${KEY,,}`
		INPUT=$KEY
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	$'\n')
		INPUT="Enter"
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	$'\177')
		INPUT="Backspace"
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	$'\e')
		INPUT="Escape"
		read -s -t 0.1 -N 1 KEY # read second char after \e
		if [[ -n ${KEY} ]]
		then
		INPUT=""
			if [[ ${KEY} = '[' ]]
			then
				read -s -t 0.1 -N 1 KEY # read third char after \e[
				case ${KEY} in
					'D')	INPUT="CursorLeft" ;;
					'C')	INPUT="CursorRight" ;;
					'A')	INPUT="CursorUp" ;;
					'B')	INPUT="CursorDown" ;;
					'H')	INPUT="Pos1" ;;
					'F')	INPUT="End" ;;
					# 4-char cases
					[2356])
					    if [[ ${KEY} = '5' ]]; then INPUT="PageUp"; fi
						if [[ ${KEY} = '6' ]]; then INPUT="PageDown"; fi
						if [[ ${KEY} = '3' ]]; then INPUT="Delete"; fi
						if [[ ${KEY} = '2' ]]; then INPUT="Insert"; fi
						# common check for '~' 
						read -s -t 0.1 -N 1 KEY
						if [[ ${KEY} != '~' ]]
						then INPUT=""
						fi
						;;
				esac
			fi
		fi
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	esac

	IFS=$OLD_IFS
} # END function GET_KEYPRESS



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


## PRINT ENTRIES AND NUMBER - ALSO USING THIS TO MAKE CHOICE ARRAY FOR NOW
unset CHOICEPOS
for printline in "${boot_entries[@]}"
do
	CHOICEPOS+=(".")
	echo "$printline"
done

NUM_ENTRIES=${#boot_entries[@]}
let "MAX_ENTRY=$NUM_ENTRIES - 1"

echo
echo "There are ${NUM_ENTRIES} entries from 0 to ${MAX_ENTRY}"
echo

if [[ ${NUM_ENTRIES} = "0" ]]
then
	clear
	echo "No entries found. Aborting."
	exit -1
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
		echo -e    "* Press ${fBOLD}Insert${fPLAIN} to assign/remove all positions in order"
		echo -e    "* Press ${fBOLD}q${fPLAIN} to quit"
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

		GET_KEYPRESS

		case ${INPUT} in
			"Pos1")
				let "CURRENT_POS=0" ;;
			"End")
				let "CURRENT_POS=$MAX_ENTRY" ;;
			"CursorDown")
				let "CURRENT_POS=( $CURRENT_POS + 1 ) % $NUM_ENTRIES" ;;
			"CursorUp")
				let "CURRENT_POS=( $CURRENT_POS + $NUM_ENTRIES - 1 ) % $NUM_ENTRIES" ;;
			"q")	clear
				echo "Exited without saving."
				exit ;;
			"Enter")
				break ;;
			"Delete")
				CHOICEPOS[$CURRENT_POS]="."
				;;
			"Insert")
				CHOICEKEYS=( "1" "2" "3" "4" "5" "6" "7" "8" "9" "a" "b" "c" "d" "e" "f" )
				REMOVE_ALL=true
				for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
				do
					if [[ ${CHOICEPOS[$i]} != ${CHOICEKEYS[$i]} ]]
					then
						REMOVE_ALL=false
					fi
					CHOICEPOS[$i]=${CHOICEKEYS[$i]}
				done
				# remove all if they were all set in order before
				if [[ ${REMOVE_ALL} = true ]]
				then
					for (( i = 0 ; i < ${NUM_ENTRIES}; i++ ))
					do
						CHOICEPOS[$i]="."
					done
				fi
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
		echo "Switch positions chosen assigned:"
		echo "---------------------------------"
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
		echo -e    "* Press ${fBOLD}q${fPLAIN} to quit"

		GET_KEYPRESS

		case ${INPUT} in
				"q")	clear
					echo "Exited without saving."
					exit ;;
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
SECS=0
colorindex=0
while [[ ${REPEAT_DISPLAY_CONFIG} = true ]] ; do

	clear
	while [[ true ]]  ## SETTING DISPLAY SECONDS
	do
		echo       "--------------------------------------------------------"
		echo       "Set boot choice display time:"

		echo -e    "* Press ${fBOLD}Cursor Up / Cursor Down${fPLAIN} to change +/- 10 seconds"
		echo -e    "* Press ${fBOLD}Cursor Right/ Cursor Left${fPLAIN} to change +/- 1 second"

		echo -e    "* Press ${fBOLD}Enter${fPLAIN} to confirm selection"
		echo -e    "* Press ${fBOLD}q${fPLAIN} to quit"
		echo       "--------------------------------------------------------"
		echo

		echo -e -n "Show boot choice for ${fBOLD}"
		printf %03d $SECS
		echo -e    "${fPLAIN} seconds"

		GET_KEYPRESS

		case ${INPUT} in
				"q")	clear
					echo "Exited without saving."
					exit ;;
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
			echo -e    "* Press ${fBOLD}q${fPLAIN} to quit"
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
			echo -e		"(Caution: Not all Linux terminals can display the"
			echo -e		" complete range of colors that GRUB supports)"
			echo

			GET_KEYPRESS

			case ${INPUT} in
					"q")
						clear
						echo "Exited without saving."
						exit ;;
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
rm -Rf ./grub_switch_hashes/*
mkdir ./grub_switch_hashes


#### TODO: change to reflect different times and colors for entries
# create and fill .entries.txt (for use with custom grubswitch device)
echo "### #1 specifies display time for subsequent entries" > ${BOOTFILES_DIR}/.entries.txt
echo "### #2 specifies display colors for subsequent entries" > ${BOOTFILES_DIR}/.entries.txt
echo "### Uncommented lines are GRUB switch choices 1..15 (0x1..0xF)" >> ${BOOTFILES_DIR}/.entries.txt
echo "### An empty line leads to the GRUB menu, as does choice 0" >> ${BOOTFILES_DIR}/.entries.txt

echo -n "#1 " >> ./.entries.txt
	printf %03d $SECS >> ./.entries.txt
	echo "        # seconds to display boot choice" >> ./.entries.txt
echo -n "#2 " >> ./.entries.txt
	echo -n "${grubcolors[$colorindex]}" >> ./.entries.txt
	echo " # highlight color that is used for the boot choice" >> ./.entries.txt

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


### make sure that the owner of files is local user (should be able to edit)
if [ root = $USER ]
then
	local_owner=`ls -ld | awk 'NR==1 {print $3}'` 
	chown -R $local_owner:$local_owner .
	chmod -R 644 .
fi

cd ${CURR_DIR}


### print generated '.entries.txt' file content
echo -e "The following data was written to ${fBOLD}${BOOTFILES_DIR}.entries.txt${fPLAIN}:"
echo
cat ${BOOTFILES_DIR}/.entries.txt
echo

