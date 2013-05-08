#!/bin/bash

# By default log.html and log.logcat files are made in currend directory $PWD.
# If you do not like this change this line.
LOG_DIR="$PWD"

# Uncomment this line if you need to clean logcat before logging to files.
#adb logcat -c

set -o pipefail

adb logcat -v time *:V 2>&1 | coloring_tee \
	--color-schemes=logcat --html="$LOG_DIR/log.html" "$LOG_DIR/log.logcat"
	
exit $?

