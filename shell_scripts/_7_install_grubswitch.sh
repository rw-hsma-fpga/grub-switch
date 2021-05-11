#!/bin/bash
if [ "${BASH_SOURCE[0]}" != "$0" ]; then
	echo; echo "Please execute this script, don't source it"
	echo "(sourcing pollutes the environment with variables)."; echo
	return; fi




# terminal format macros
fPLAIN="\e[0m"
fBOLD="\e[1m"

# keypress polling function
function GET_ANY_KEYPRESS () {

	OLD_IFS=$IFS
	IFS=''

	read -s -N 1 KEY
	until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush

	IFS=$OLD_IFS
} # END function GET_ANY_KEYPRESS


# keyboard polling function... puts key name in $INPUT; empty if no interesting key
function GET_KEY () {

	OLD_IFS=$IFS
	IFS=''

	INPUT=""
	read -s -N 1 KEY
	case $KEY in
	[yn])
		KEY=`echo ${KEY,,}`
		INPUT=$KEY
		until [[ -z ${KEY} ]]; do read -s -t 0.1 -N 1 KEY; done # keyboard flush
		;;
	esac

	IFS=$OLD_IFS
} # END function GET_KEY


function check_request_sudo {
	local sudostate=`sudo -n whoami 2> /dev/null`

	if [ "$sudostate" = "root" ]
	then :
	else
		sudostate=`sudo whoami`
	fi

	if [ "$sudostate" = "root" ]
	then
		last_sudo_state="ACTIVE"
	else
		last_sudo_state="INACTIVE"
	fi
	last_sudo_date=`date`
}



unset last_sudo_state
unset last_sudo_date



clear
echo -e -n "$fBOLD"
echo "7 - Install GRUBswitch into grub.cfg"	
echo "------------------------------------"
echo -e -n "$fPLAIN"
check_request_sudo



### parse commandline parameters for grub dirs, check existence/access
OPTIND=1
while getopts ":g:s:" callarg
do
	case ${callarg} in
		g)
			GRUB_CFG_DIR=${OPTARG}
			#echo "grub.cfg dir specified as ${GRUB_CFG_DIR}"
			;;
		s)
			CFG_SCRIPTS_DIR=${OPTARG}
			#echo "script dir for cfg-building specified as ${CFG_SCRIPTS_DIR}"
			;;
		*)
			;;
	esac
done



### check work path, bootfiles availability
CURR_DIR=`pwd`
if [[ "$CURR_DIR" =~ ^.*/shell_scripts$ ]]
then :
else
	echo "ERROR: Script not started from  shell_scripts  directory" >&2
	exit -1
fi








echo
echo "Press any key to return to main menu."
echo
GET_ANY_KEYPRESS
