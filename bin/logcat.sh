#!/bin/bash

# By default log.html and log.logcat files are made in currend directory $PWD.
# If you do not like this change this line.
LOG_DIR="$PWD"

PROGRAM_NAME=$0

while true
do
	if test $# -lt 1
	then
		break
	else
		case $1 in
		-h)
			echo "Usage:
			
	$PROGRAM_NAME [OPTIONS]
 
Options:

	-h		Print this help.
	-c		Clean logcat before loggin.
	
"
			exit 0
			;;
		-c)
			adb logcat -c
			;;
		esac
		shift
	fi
done

set -o pipefail

adb logcat -v time *:V 2>&1 | coloring_tee \
	--color-schemes=logcat --html="$LOG_DIR/log.html" "$LOG_DIR/log.logcat"
	
exit $?

