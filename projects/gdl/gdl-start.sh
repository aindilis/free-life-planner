#!/bin/sh

export GGP_BASE_DIR=/var/lib/myfrdcsa/sandbox/ggp-base-20170429/ggp-base-20170429
export SANCHO_DIR=/var/lib/myfrdcsa/sandbox/sancho-20160113/sancho-20160113

echo After Sancho is started, be sure to press the "Create" button twice.
(cd $SANCHO_DIR && ./Sancho.sh &)

echo After GGP-Base is started, be sure to select the "Game" selection box and then press the "Start a new match!" button.
(cd $GGP_BASE_DIR && ./gradlew server -Pmyargs="myTournament connectfour 4 4 127.0.0.1 9147 PlayerOne 127.0.0.1 9148 PlayerTwo" &)

echo If we choose, run such a program from here.
(cd $GGP_BASE_DIR && ./gradlew kiosk &)
