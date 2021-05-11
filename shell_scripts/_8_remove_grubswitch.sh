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
echo "8 - Remove GRUBswitch from grub.cfg"	
echo "-----------------------------------"
echo -e -n "$fPLAIN"
check_request_sudo


### parse commandline parameters for grub dirs, check existence/access
GRUB_CFG_DIR=""
CFG_SCRIPTS_DIR=""
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


### check writability of GRUB directories
sudo test -w "${GRUB_CFG_DIR}"
if [ "$?" -ne "0" ]
then
	echo "ERROR: GRUB boot directory not writable" >&2
	exit -1	
fi

sudo test -w "${CFG_SCRIPTS_DIR}"
if [ "$?" -ne "0" ]
then
	echo "ERROR: GRUB script directory not writable" >&2
	exit -1	
fi


### check work path, template availability
CURR_DIR=`pwd`
if [[ "$CURR_DIR" =~ ^.*/shell_scripts$ ]]
then :
else
	echo "ERROR: Script not started from  shell_scripts  directory" >&2
	exit -1
fi



### Ask for write confirmation
echo -e "This action will re-generate the ${fBOLD}grub.cfg${fPLAIN} file by calling ${fBOLD}update-grub${fPLAIN}"
echo -e "Do you want to proceed? (${fBOLD}y${fPLAIN})es / (${fBOLD}n${fPLAIN})o"

while [[ true ]]
do
	GET_KEY
	case ${INPUT} in
			"y")
				sudo rm -rf $CFG_SCRIPTS_DIR/99_grub_switch
				# message to update GRUB
				echo ; echo "GRUBswitch modifier script removed" ; echo
				sudo update-grub
				echo ; echo -e "${fBOLD}grub.cfg${fPLAIN} re-generated without GRUBswitch" ; echo
				break
				;;
			"n")
				echo ; echo "Action canceled." ; echo
				sleep 1
				break
				;;
			"*")
				;;
	esac
done


echo
echo "Press any key to return to main menu."
echo
GET_ANY_KEYPRESS
