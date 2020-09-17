#!/bin/bash


fPLAIN="\e[0m"
fBOLD="\e[1m"
fUNBOLD="\e[22m"
fDIM="\e[2m"
fUNDIM="\e[22m"
fREVERSE="\e[7m"




fgDEFAULT="\e[39m"

fgBLACK="\e[30m" ; fgRED="\e[31m" ; fgGREEN="\e[32m" ; fgYELLOW="\e[33m"
fgBLUE="\e[34m" ; fgMAGENTA="\e[35m" ; fgCYAN="\e[36m" ; fgLIGHTGRAY="\e[37m"
fgDARKGRAY="\e[90m" ; fgLIGHTRED="\e[91m" ; fgLIGHTGREEN="\e[92m" ; fgLIGHTYELLOW="\e[93m"
fgLIGHTBLUE="\e[94m" ; fgLIGHTMAGENTA="\e[95m" ; fgLIGHTCYAN="\e[96m" ; fgWHITE="\e[97m"


bgDEFAULT="\e[49m"

bgBLACK="\e[40m" ; bgRED="\e[41m" ; bgGREEN="\e[42m" ; bgYELLOW="\e[43m"
bgBLUE="\e[44m" ; bgMAGENTA="\e[45m" ; bgCYAN="\e[46m" ; bgLIGHTGRAY="\e[47m"
bgDARKGRAY="\e[100m" ; bgLIGHTRED="\e[101m" ; bgLIGHTGREEN="\e[102m" ; bgLIGHTYELLOW="\e[103m"
bgLIGHTBLUE="\e[104m" ; bgLIGHTMAGENTA="\e[105m" ; bgLIGHTCYAN="\e[106m" ; bgWHITE="\e[107m"

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
termcolors+=( "${fgBLUE}${bgBLACK}" ) ;     grubcolors+=( "blue/black" );
termcolors+=( "${fgDARKGRAY}${bgBLACK}" ) ; grubcolors+=( "dark-gray/black" );

termcolors+=( "${fgRED}${bgWHITE}" ) ;      grubcolors+=( "red/white" );
termcolors+=( "${fgBLUE}${bgWHITE}" ) ;     grubcolors+=( "blue/white" );
termcolors+=( "${fgDARKGRAY}${bgWHITE}" ) ; grubcolors+=( "dark-gray/white" );

termcolors+=( "${fgLIGHTYELLOW}${bgRED}" ) ;     grubcolors+=( "yellow/red" );
termcolors+=( "${fgLIGHTYELLOW}${bgGREEN}" ) ;   grubcolors+=( "yellow/green" );
termcolors+=( "${fgLIGHTYELLOW}${bgBLUE}" ) ;    grubcolors+=( "yellow/blue" );
termcolors+=( "${fgLIGHTYELLOW}${bgDARKGRAY}" ) ;grubcolors+=( "yellow/dark-gray" );

termcolors+=( "${fgBLUE}${bgLIGHTYELLOW}" ) ;    grubcolors+=( "blue/yellow" );
termcolors+=( "${fgDARKGRAY}${bgLIGHTYELLOW}" ) ;grubcolors+=( "dark-gray/yellow" );
##termcolors+=( "${fg}${bg}" ) ; grubcolors+=( "/" );


echo -e -n "${fBOLD}"
for (( i=0 ; i<${#termcolors[@]} ; i++ )); do
	echo -e -n "${fBOLD}"
	echo -e -n "${termcolors[$i]}" 
	echo -e -n "${grubcolors[$i]}"
	echo -e -n "${fPLAIN}     "
	echo -e -n "${termcolors[$i]}" 
	echo -e -n "${grubcolors[$i]}"
	echo -e    "${fgDEFAULT}${bgDEFAULT}"
done
echo -e "\e[39m\e[49m"


return

for i in $fgBLACK $fgRED $fgGREEN $fgYELLOW $fgBLUE $fgMAGENTA $fgCYAN $fgLIGHTGRAY $fgDARKGRAY $fgLIGHTRED $fgLIGHTGREEN $fgLIGHTYELLOW $fgLIGHTBLUE $fgLIGHTMAGENTA $fgLIGHTCYAN $fgWHITE
do
	for j in $bgBLACK $bgRED $bgGREEN $bgYELLOW $bgBLUE $bgMAGENTA $bgCYAN $bgLIGHTGRAY $bgDARKGRAY $bgLIGHTRED $bgLIGHTGREEN $bgLIGHTYELLOW $bgLIGHTBLUE $bgLIGHTMAGENTA $bgLIGHTCYAN $bgWHITE
	do
		echo -e -n "${j}${i} OS "
	done
	echo -e "${fgDEFAULT}${bgDEFAULT}"
	for j in $bgBLACK $bgRED $bgGREEN $bgYELLOW $bgBLUE $bgMAGENTA $bgCYAN $bgLIGHTGRAY $bgDARKGRAY $bgLIGHTRED $bgLIGHTGREEN $bgLIGHTYELLOW $bgLIGHTBLUE $bgLIGHTMAGENTA $bgLIGHTCYAN $bgWHITE
	do
		echo -e -n "${j}${i}    "
	done
	echo -e "${fgDEFAULT}${bgDEFAULT}"
done
echo


return
