#!/bin/bash


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


# clean leave
function EXIT_WITH_KEYPRESS () {

	echo
	echo "Press any key to return to main menu."
	echo

	local OLD_IFS=$IFS
	IFS=''

	read -s -N 1 KEY
	until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush

	IFS=$OLD_IFS

	exit
} # END function EXIT_WITH_KEYPRESS


function check_sudo {
	local sudostate=`sudo -n whoami 2> /dev/null`
	if [ "$sudostate" = "root" ]
	then
		LAST_SUDO_STATE="ACTIVE"
	else
		LAST_SUDO_STATE="INACTIVE"
	fi
	LAST_SUDO_DATE=`date`
}


function check_request_sudo {
	local sudostate=`sudo -n whoami 2> /dev/null`

	if [ "$sudostate" = "root" ]
	then :
	else
		sudostate=`sudo whoami`
	fi

	if [ "$sudostate" = "root" ]
	then
		LAST_SUDO_STATE="ACTIVE"
	else
		LAST_SUDO_STATE="INACTIVE"
	fi
	LAST_SUDO_DATE=`date`
}


function check_request_sudo_write_state {

	if [ "$SUDO_WRITE_STATE" = "INACTIVE" ]
	then
		echo; echo "This action requires sudo (super user) write access." ;	echo
		sleep 1
		sudo -K
		check_request_sudo
		if [ "$LAST_SUDO_STATE" = "ACTIVE" ]
		then
			SUDO_WRITE_STATE="ACTIVE"
			echo ; echo "sudo write access acquired."
			sleep 2
		else
			echo ; echo "No sudo acquired. Returning to menu."
			sleep 2
		fi
	else # sudo write ACTIVE
		check_sudo
		if [ "$LAST_SUDO_STATE" = "INACTIVE" ]
		then
			echo; echo "This action requires sudo (super user) write access." ;	echo
			sleep 1
			check_request_sudo
			if [ "$LAST_SUDO_STATE" = "ACTIVE" ]
			then
				echo ; echo "sudo write access acquired."
				sleep 2
			else
				echo ; echo "No sudo acquired. Returning to menu."
				sleep 2
			fi
		fi
	fi
}


function get_path_arguments {

	OPTIND=1

	while getopts ":g:s:" callarg
	do
		case ${callarg} in
			g)
				GRUB_CFG_DIR=${OPTARG}
				;;
			s)
				CFG_SCRIPTS_DIR=${OPTARG}
				;;
			*)
				;;
		esac
	done
}



