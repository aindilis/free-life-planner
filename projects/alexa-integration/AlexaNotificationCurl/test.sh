#!/bin/bash

INTERVAL=5
CURRENTTIMESTAMP=`date "+%s"`
LASTTIMESTAMP=`cat timestamp.txt`
if [ "x$LASTTIMESTAMP" == "x" ]; then
    echo $CURRENTTIMESTAMP > timestamp.txt
    echo Refreshing Token
else
    if (( $CURRENTTIMESTAMP > ($LASTTIMESTAMP + $INTERVAL) )); then
	echo $CURRENTTIMESTAMP > timestamp.txt	
	echo Refreshing Token
    else
	TOTAL=`expr $LASTTIMESTAMP + $INTERVAL`
	TIMELEFT=`expr $TOTAL - $CURRENTTIMESTAMP`
	echo Do not have to refresh for $TIMELEFT seconds
    fi
fi

