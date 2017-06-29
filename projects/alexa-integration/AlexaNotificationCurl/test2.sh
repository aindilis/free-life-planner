#!/bin/bash

function playAudioFile {
    if [ $1 == "<ROOM1>" ];
    then
	mplayer -ao pulse::bluez_sink.<BLUETOOTHADDRESS> /var/lib/myfrdcsa/codebases/minor/alexa-integration/data-git/system-sounds/silence-0.4-seconds.mp3 "$2"
    elif [ $1 == "<ROOM2>" ];
    then
	echo /var/lib/myfrdcsa/codebases/minor/alexa-integration/AlexaNotificationCurl/remote-play.pl -o bluetooth -u <USERNAME> -a <IP> -f /var/lib/myfrdcsa/codebases/minor/alexa-integration/data-git/system-sounds/silence-0.4-seconds.mp3 $2
    fi
}

playAudioFile <ROOM1> test.mp3
